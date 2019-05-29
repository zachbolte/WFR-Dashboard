//https://github.com/coryjfowler/MCP_CAN_lib

#include <SPI.h>
#include <mcp_can.h>

const int spiCSPin = 10;

MCP_CAN CAN(spiCSPin);

void setup() {
  Serial.begin(115200);

  while (CAN_OK != CAN.begin(CAN_500KBPS)) {
    Serial.println("CAN BUS init Failed");
    delay(100);
  }
  Serial.println("CAN BUS Shield Init OK!");
}
//byte stmp1[8] = {255, 0, 0, 0, 0, 0, 0, 0};
byte stmp1[8] = {0x3, 0xB, 0x5, 0xF, 0x1, 0xD, 0, 0};
byte stmp2[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void loop() {
  Serial.println("In loop");

//  for (int i = 0; i <= 255; i++) {
//    stmp1[4] = i;
//    stmp1[2] = i;
//    CAN.sendMsgBuf(500, 0, 8, stmp1);
//    delay(20);
//  }
  CAN.sendMsgBuf(500, 0, 8, stmp1);
  delay(1000);
  CAN.sendMsgBuf(501, 0, 8, stmp2);
  delay(1000);

}
