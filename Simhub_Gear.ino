
#include "FlowSerialRead.h"
#include "FastLED.h"

#define NUM_LEDS 10
#define PIXEL_PIN 31

CRGB leds[NUM_LEDS];

#define MESSAGE_HEADER 0x03

int ENABLED_BUTTONS_COUNT = 0;
int TM1638_ENABLEDMODULES = 0;
int MAX7221_ENABLEDMODULES = 0;
int TM1637_ENABLEDMODULES = 0;
int ENABLE_ADA_HT16K33_7SEGMENTS = 0;
int WS2812B_RGBLEDCOUNT = 0;
int WS2801_RGBLEDCOUNT = 0;

String DEVICE_NAME = String("G29_SimHub_display-V1.0");

uint8_t header = 0;
char opt;

int clockPin = 1;
int dataPin = 0;
int latchPin = 4;
int enablePin = 3;
bool light_state = true;

int dataArray[12];

void setup() {
  delay(2000);
  FastLED.addLeds<WS2812B, PIXEL_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(32);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  analogWrite(enablePin, 240);

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
  delay(1000);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);

  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < 4) {
      leds[i] = CRGB::Green;
    } else if (i < 7) {
      leds[i] = CRGB::Blue;
    } else {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
    delay(50);
  }

  for (int i = NUM_LEDS - 1 ; i >= 0; i--) {
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(50);
  }



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
        float ShiftLight1 = FlowSerialReadStringUntil(';').toFloat();
        float ShiftLight2 = FlowSerialReadStringUntil(';').toFloat();
        int RedLine = FlowSerialReadStringUntil(';').toInt();
        int speedkmh = FlowSerialReadStringUntil('\n').toInt();
        //FlowSerialDebugPrintLn("Gear: " + gear);
        //FlowSerialDebugPrintLn("Shift1: " + String(ShiftLight1));
        //FlowSerialDebugPrintLn("Shift2: " + String(ShiftLight2));
        //FlowSerialDebugPrintLn("Redline: " + String(RedLine));
        shiftOut(dataPin, clockPin, MSBFIRST, dataArray[speedkmh % 10]);
        shiftOut(dataPin, clockPin, MSBFIRST, dataArray[(speedkmh/10) % 10]);
        shiftOut(dataPin, clockPin, MSBFIRST, dataArray[(speedkmh/100) % 10]);
        if (gear == "N") {
          shiftOut(dataPin, clockPin, MSBFIRST, dataArray[10]);
          //shiftOut(dataPin, clockPin, MSBFIRST, 0);
        }
        else if (gear == "R") {
          shiftOut(dataPin, clockPin, MSBFIRST, dataArray[11]);
          //shiftOut(dataPin, clockPin, MSBFIRST, 0);
        }
        else {
          int gear_num = gear.toInt();
          shiftOut(dataPin, clockPin, MSBFIRST, dataArray[gear_num]);

          //shiftOut(dataPin, clockPin, MSBFIRST, 0);
        }
          digitalWrite(latchPin, HIGH);
          digitalWrite(latchPin, LOW);
        int RPMState;
        if (ShiftLight1 == 0) {
          for (int i = 0; i < NUM_LEDS; i ++) {
            leds[i] = CRGB::Black;
          }
          FastLED.show();
          RPMState = 0;
        }
        if (RedLine == 1) {
          if (light_state == true) {
            for (int i = 0; i < NUM_LEDS; i++) {
              if (i < 4) {
                leds[i] = CRGB::Green;
              } else if (i < 7) {
                leds[i] = CRGB::Blue;
              } else {
                leds[i] = CRGB::Red;
              }
            }
            FastLED.show();
            light_state = false;
          } else {
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB::Black;
            }
            FastLED.show();
            light_state = true;
          }
          FlowSerialDebugPrintLn("Light3");
          RPMState = 3;
        }
        else if (ShiftLight2 > 0.6) {
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i < 4) {
              leds[i] = CRGB::Green;
            } else if (i < 7) {
              leds[i] = CRGB::Blue;
            } else {
              leds[i] = CRGB::Red;
            }
          }
          FastLED.show();
        }
        else if (ShiftLight2 > 0.25) {
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i < 4) {
              leds[i] = CRGB::Green;
            } else if (i < 7) {
              leds[i] = CRGB::Blue;
            } else {
              leds[i] = CRGB::Black;
            }
          }
          RPMState = 2;
          FastLED.show();
        }
        else if (ShiftLight1 > 0.9) {
          for (int i = 0; i < NUM_LEDS; i++) {
            if (i < 4) {
              leds[i] = CRGB::Green;
            } else {
              leds[i] = CRGB::Black;
            }
          }
          FastLED.show();
          RPMState = 1;
        }
        if (RPMState != 3) {
          light_state = true;
        }
      }
    }
  }
}
