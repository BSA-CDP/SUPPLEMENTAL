#include <SPI.h>
#include <RH_RF69.h>

#define RF69_FREQ 433.0

#define RFM69_CS   25
#define RFM69_INT  12
#define RFM69_RST  9

RH_RF69 rf69(RFM69_CS, RFM69_INT);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(1);
  Serial.println("Booting...");

  // Reset
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  Serial.println("Calling rf69.init()...");

  if (!rf69.init()) {
    Serial.println("RFM69 init failed!");
    while (1);
  }

  Serial.println("RFM69 init OK!");
  rf69.setFrequency(RF69_FREQ);
  rf69.setTxPower(20, true); // HCW radios
}

void loop() {
  if (rf69.available()) {
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf69.recv(buf, &len)) {
      Serial.print("Received: ");
      Serial.write(buf, len);
      Serial.print("  RSSI=");
      Serial.println(rf69.lastRssi());
    }
  }
}
