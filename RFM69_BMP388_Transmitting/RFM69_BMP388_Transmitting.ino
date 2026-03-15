// BMP388 Transmitter code!

#include <SPI.h>
#include <RH_RF69.h>
#include <Adafruit_BMP3XX.h>

// ---- Pin Definitions ----
#define RF69_CS   25
#define RF69_INT  12
#define RF69_RST   9
#define RF69_FREQ  433.0

// ---- RadioHead RF69 driver ----
RH_RF69 rf69(RF69_CS, RF69_INT);

// ---- BMP388 ----
Adafruit_BMP3XX bmp;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Transmitter starting…");

  // ----- Reset RF69 -----
  pinMode(RF69_RST, OUTPUT);
  digitalWrite(RF69_RST, HIGH);
  delay(10);
  digitalWrite(RF69_RST, LOW);
  delay(10);

  // ----- Init RF69 -----
  if (!rf69.init()) {
    Serial.println("RF69 init failed!");
    while (1);
  }
  rf69.setFrequency(RF69_FREQ);
  rf69.setTxPower(20, true);   // high-power HCW mode
  // No encryption needed (RadioHead has none by default)

  Serial.println("RF69 ready.");

  // ----- Init BMP388 -----
  if (!bmp.begin_I2C()) {
    Serial.println("Could not find BMP388!");
    while (1);
  }

  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  Serial.println("BMP388 ready.");
}

void loop() {
  if (!bmp.performReading()) {
    Serial.println("BMP388 read failed!");
    return;
  }

  float temp = bmp.temperature;
  float pres = bmp.pressure / 100.0;
  float alt  = bmp.readAltitude(1013.25);

  char msg[64];
  snprintf(msg, sizeof(msg),
           "T=%.2fC P=%.2fhPa ALT=%.2fm",
           temp, pres, alt);

  Serial.print("Sending: ");
  Serial.println(msg);

  rf69.send((uint8_t*)msg, strlen(msg));
  rf69.waitPacketSent();

  delay(5000);
}
