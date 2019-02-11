
#include "FlowSerialRead.h"

#define MESSAGE_HEADER 0x03

int ENABLED_BUTTONS_COUNT = 0;
int TM1638_ENABLEDMODULES = 0;
int MAX7221_ENABLEDMODULES = 0;
int TM1637_ENABLEDMODULES = 0;
int ENABLE_ADA_HT16K33_7SEGMENTS = 0;
int WS2812B_RGBLEDCOUNT = 0;
int WS2801_RGBLEDCOUNT = 0;

String DEVICE_NAME = String("SimHub Gear");

uint8_t header = 0;
char opt;

int clockPin = 1;
int dataPin = 0;
int latchPin = 4;
int enablePin = 3;
bool light_state = true;

int dataArray[12];

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  analogWrite(enablePin, 220);

  dataArray[0] = 0b11111100;
  dataArray[1] = 0b01100000;
  dataArray[2] = 0b11011010;
  dataArray[3] = 0b11110010;
  dataArray[4] = 0b01100110;
  dataArray[5] = 0b10110110;
  dataArray[6] = 0b10111110;
  dataArray[7] = 0b11100000;
  dataArray[8] = 0b11111110;
  dataArray[9] = 0b11110110;
  dataArray[10] = 0b00101010;
  dataArray[11] = 0b00001010;

  shiftOut(dataPin, clockPin, MSBFIRST, 255);
  shiftOut(dataPin, clockPin, MSBFIRST, 255);
  shiftOut(dataPin, clockPin, MSBFIRST, 255);
  shiftOut(dataPin, clockPin, MSBFIRST, 255);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
  delay(500);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);



  FlowSerialBegin(19200);
  while (!Serial) {
    ;
  }
}

void SetBaudrate() {
  int br = FlowSerialTimedRead();

  delay(200);

  if (br == 1) FlowSerialBegin(300);
  if (br == 2) FlowSerialBegin(1200);
  if (br == 3) FlowSerialBegin(2400);
  if (br == 4) FlowSerialBegin(4800);
  if (br == 5) FlowSerialBegin(9600);
  if (br == 6) FlowSerialBegin(14400);
  if (br == 7) FlowSerialBegin(19200);
  if (br == 8) FlowSerialBegin(28800);
  if (br == 9) FlowSerialBegin(38400);
  if (br == 10) FlowSerialBegin(57600);
  if (br == 11) FlowSerialBegin(115200);
  if (br == 12) FlowSerialBegin(230400);
  if (br == 13) FlowSerialBegin(250000);
  if (br == 14) FlowSerialBegin(1000000);
  if (br == 15) FlowSerialBegin(2000000);
  if (br == 16) FlowSerialBegin(200000);
}

void loop() {
  // Wait for data
  if (FlowSerialAvailable() > 0) {

    // Read header
    header = FlowSerialRead();

    if (header == MESSAGE_HEADER) {

      // Read command
      opt = FlowSerialTimedRead();

      // Hello command
      if (opt == '1') {
        FlowSerialTrigger = FlowSerialTimedRead();
        delay(10);
        FlowSerialPrint('h');
        FlowSerialFlush();
      }

      // Set Baudrate
      if (opt == '8') {
        SetBaudrate();
        while (!Serial) {
          ;
        }
      }


      // Simple buttons count
      if (opt == 'J') {
        FlowSerialWrite((byte)(ENABLED_BUTTONS_COUNT));
        FlowSerialFlush();
      }

      //  Module count command
      if (opt == '2') {
        FlowSerialWrite((byte)(TM1638_ENABLEDMODULES));
        FlowSerialFlush();
      }

      //  SIMPLE Module count command
      if (opt == 'B') {
        FlowSerialWrite((byte)(MAX7221_ENABLEDMODULES + TM1637_ENABLEDMODULES + ENABLE_ADA_HT16K33_7SEGMENTS));
        FlowSerialFlush();
      }

      // ACQ Packet
      if (opt == 'A') {
        FlowSerialWrite(0x03);
        FlowSerialFlush();
      }

      // Device Name
      if (opt == 'N') {
        FlowSerialPrint(DEVICE_NAME);
        FlowSerialPrint("\n");
        FlowSerialFlush();
      }

      // Features Command
      if (opt == '0') {
        delay(10);
        //name
        FlowSerialPrint("N");
        // Custom Protocol Support
        FlowSerialPrint("P");

        FlowSerialPrint("\n");
        FlowSerialFlush();
      }

      if (opt == '4') {
        FlowSerialWrite((byte)(WS2812B_RGBLEDCOUNT + WS2801_RGBLEDCOUNT));
        FlowSerialFlush();
      }
      if (opt == 'P') {
        // NCALC: isnull([DataCorePlugin.GameData.NewData.Gear],'N') + ';' + [DataCorePlugin.GameData.NewData.CarSettings_RPMShiftLight1] + ';' + [DataCorePlugin.GameData.NewData.CarSettings_RPMShiftLight2] + ';' + [DataCorePlugin.GameData.NewData.CarSettings_RPMRedLineReached] + ';' + [DataCorePlugin.GameData.NewData.FilteredRpms]


        String gear = FlowSerialReadStringUntil(';');
        int ShiftLight1 = FlowSerialReadStringUntil(';').toInt();
        float ShiftLight2 = FlowSerialReadStringUntil(';').toFloat();
        int RedLine = FlowSerialReadStringUntil(';').toInt();
        float RPM = FlowSerialReadStringUntil('\n').toFloat();
        //FlowSerialDebugPrintLn("Gear: " + gear);
        //FlowSerialDebugPrintLn("Shift1: " + String(ShiftLight1));
        //FlowSerialDebugPrintLn("Shift2: " + String(ShiftLight2));
        //FlowSerialDebugPrintLn("Redline: " + String(RedLine));
        if (gear == "N") {
          shiftOut(dataPin, clockPin, MSBFIRST, dataArray[10]);
          digitalWrite(latchPin, HIGH);
          digitalWrite(latchPin, LOW);
          //shiftOut(dataPin, clockPin, MSBFIRST, 0);
        }
        else if (gear == "R") {
          shiftOut(dataPin, clockPin, MSBFIRST, dataArray[11]);
          digitalWrite(latchPin, HIGH);
          digitalWrite(latchPin, LOW);
          //shiftOut(dataPin, clockPin, MSBFIRST, 0);
        }
        else {
          int gear_num = gear.toInt();
          shiftOut(dataPin, clockPin, MSBFIRST, dataArray[gear_num]);
          digitalWrite(latchPin, HIGH);
          digitalWrite(latchPin, LOW);
          //shiftOut(dataPin, clockPin, MSBFIRST, 0);
        }

        int RPMState;
        if (ShiftLight1 == 0) {
          shiftOut(dataPin, clockPin, MSBFIRST, 0);
          digitalWrite(latchPin, HIGH);
          digitalWrite(latchPin, LOW);
          //FlowSerialDebugPrintLn("Low");
          RPMState = 0;
        }
        if (RedLine == 1) {
          if (light_state == true) {
            shiftOut(dataPin, clockPin, MSBFIRST, 0b10010010);
            digitalWrite(latchPin, HIGH);
            digitalWrite(latchPin, LOW);;
            light_state = false;
          } else {
            shiftOut(dataPin, clockPin, MSBFIRST, 0);
            digitalWrite(latchPin, HIGH);
            digitalWrite(latchPin, LOW);
            light_state = true;
          }
          FlowSerialDebugPrintLn("Light3");
          RPMState = 3;
        }
        else if (ShiftLight2 > 0.5) {
          shiftOut(dataPin, clockPin, MSBFIRST, 0b00010010);
          digitalWrite(latchPin, HIGH);
          digitalWrite(latchPin, LOW);
          //FlowSerialDebugPrintLn("Light2");
          RPMState = 2;
        }
        else if (ShiftLight1 == 1) {
          shiftOut(dataPin, clockPin, MSBFIRST, 0b00010000);
          digitalWrite(latchPin, HIGH);
          digitalWrite(latchPin, LOW);
          //FlowSerialDebugPrintLn("Light1");
          RPMState = 1;
        }
        if (RPMState != 3) {
          light_state = true;
        }
      }
    }
  }
}
