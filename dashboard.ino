//https://github.com/coryjfowler/MCP_CAN_lib

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "mcp_can.h"

#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9
#define spiCSPin 9

//Assigned message addresses
#define addr1 500
#define addr2 501

int thirdDataFieldTitle;

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t tx, ty;

MCP_CAN CAN(spiCSPin);

void setup() {
  Serial.begin(115200);

  Serial.println("RA8875 start");
  /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
  if (!tft.begin(RA8875_480x272)) {
    Serial.println("RA8875 Not Found!");
  }

  while (CAN_OK != CAN.begin(CAN_500KBPS)) {
    Serial.println("CAN BUS Init Failed");
    delay(100);
  }
  Serial.println("CAN BUS Init OK!");

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
}

void loop() {
  printTest();
}

void printTest() {
  byte len = 0; //Standard length
  byte buf[8];
  tft.textMode();
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    CAN.readMsgBuf(&len, buf);
    unsigned long canId = CAN.getCanId();
    Serial.println("-----------------------------");
    Serial.print("Data from ID: 0x");
    Serial.println(canId, DEC);

    //CAN ID 500
    if (canId == addr1) {
      drawDataFieldLines();
      //Print values being sent
      for (int j = 0; j < 8; j++) {
        Serial.print(buf[j]);
        Serial.print(" ");
      }
      Serial.println();

      //Set titles of each data field
      dataFieldTitles(buf[6], buf[7], thirdDataFieldTitle);

      //Print values for each data field
      tft.setTextColor(RA8875_BLACK);
      tft.textEnlarge(3);
      firstValuePrint(buf);
      secondValuePrint(buf);
      thirdValuePrint(buf);
    }
    //CAN ID 501
    else if (canId == addr2) {
      thirdDataFieldTitle = buf[0];
      faultFlags(buf);
    }
    Serial.println();
  }
}

//Function to define data fields
void drawDataFieldLines() {
  tft.fillScreen(RA8875_CYAN);
  tft.drawLine(0, 180, 480, 180, RA8875_BLACK);
  tft.drawLine(160, 155, 160, 249, RA8875_BLACK);
  tft.drawLine(320, 155, 320, 249, RA8875_BLACK);
  tft.drawLine(0, 249, 480, 249, RA8875_BLACK);
}

//Function change titles of each data field
void dataFieldTitles(int val1, int val2, int val3) {
  tft.textTransparent(RA8875_BLACK);
  tft.textEnlarge(1);
  if (val1 == 0) {
    tft.textSetCursor(55, 150);
    tft.print("RPM");
  }
  else if (val1 == 1) {
    //Do something
  }
  
  if (val2 == 0) {
    tft.textSetCursor(200, 150);
    tft.print("SPEED");
  }
  else if (val2 == 1) {
    //Do something
  }
  
  if (val3 == 0) {
    tft.textSetCursor(365, 150);
    tft.print("TEMP");
  }
  else if (val3 == 1) {
    //Do something
  }
}

void faultFlags(byte val[]) {
  //Fault flags 1-3 are displayed on the bottom (ticker tape), LOW priority
  //Fault flag 1
  if (val[1] == 1) {
    tft.textTransparent(RA8875_BLACK);
    tft.textEnlarge(1);
    tft.textSetCursor(73, 243);
    tft.print("THE CAR MIGHT BLOW UP");
  }
  //Fault flag 2
  else if (val[2] == 1) {
    tft.textTransparent(RA8875_BLACK);
    tft.textEnlarge(1);
    tft.textSetCursor(73, 243);
    tft.print("OOOF");
  }
  //Fault flag 3
  else if (val[3] == 1) {
    tft.textTransparent(RA8875_BLACK);
    tft.textEnlarge(1);
    tft.textSetCursor(73, 243);
    tft.print("OOOOF");
  }
  //Fault flags 4-5 cover data fields and use a yellow background, MEDIUM priority
  //Fault flag 4
  else if (val[4] == 1) {
    tft.fillScreen(RA8875_YELLOW);
    tft.textTransparent(RA8875_BLACK);
    tft.textEnlarge(3);
    tft.textSetCursor(190, 175);
    tft.print("OOF");
  }
  //Fault flag 5
  else if (val[5] == 1) {
    tft.fillScreen(RA8875_YELLOW);
    tft.textTransparent(RA8875_BLACK);
    tft.textEnlarge(3);
    tft.textSetCursor(200, 175);
    tft.print("OOF");
  }
  //Fault flags 6-7 cover data fields and use a red background, HIGH priority
  //Fault flag 6
  else if (val[6] == 1) {
    tft.fillScreen(RA8875_RED);
    tft.textTransparent(RA8875_BLACK);
    tft.textEnlarge(3);
    tft.textSetCursor(200, 175);
    tft.print("OOF");
  }
  //Fault flag 7
  else if (val[7] == 1) {
    tft.fillScreen(RA8875_RED);
    tft.textTransparent(RA8875_BLACK);
    tft.textEnlarge(3);
    tft.textSetCursor(130, 175);
    tft.print("BIG OOF");
  }
}

//Function to print first data field value
//Adjusts position on display for single, double, and triple digit numbers
void firstValuePrint(byte val[]) {
  byte num1 = (val[0] * 16) + val[1];
  if (num1 >= 0 && num1 <= 9) {
    tft.textSetCursor(63, 180);
  }
  else if (num1 >= 10 && num1 <= 99) {
    tft.textSetCursor(47, 180);
  }
  else if (num1 >= 100 && num1 <= 255) {
    tft.textSetCursor(31, 180);
  }
  tft.print(num1);
}

//Function to print second data field value
//Adjusts position on display for single, double, and triple digit numbers
void secondValuePrint(byte val[]) {
  byte num2 = (val[2] * 16) + val[3];
  if (num2 >= 0 && num2 <= 9) {
    tft.textSetCursor(223, 180);
  }
  else if (num2 >= 10 && num2 <= 99) {
    tft.textSetCursor(207, 180);
  }
  else if (num2 >= 100 && num2 <= 255) {
    tft.textSetCursor(191, 180);
  }
  tft.print(num2);
}

//Function to print third data field value
//Adjusts position on display for single, double, and triple digit numbers
void thirdValuePrint(byte val[]) {
  byte num3 = (val[4] * 16) + val[5];
  if (num3 >= 0 && num3 <= 9) {
    tft.textSetCursor(381, 180);
  }
  else if (num3 >= 10 && num3 <= 99) {
    tft.textSetCursor(365, 180);
  }
  else if (num3 >= 100 && num3 <= 255) {
    tft.textSetCursor(350, 180);
  }
  tft.print(num3);
}
