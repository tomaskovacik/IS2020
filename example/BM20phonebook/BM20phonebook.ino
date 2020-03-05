
#include "IS2020.h"

#define DEBUG 0
#define RESET PB14
#define BTSerial Serial3 //Serial2 => TX = PA2, RX = PA3

IS2020 BT(&BTSerial);

void deviceInfo(uint8_t dMeviceId);
void module_info();


void setup() {
  Serial.begin(115200); //TX = PB10, RX=PB11
  BT.begin(RESET);
}
char c;
void loop() {
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    c = Serial.read();
    switch (c)
    {
      case 'h': //help
        Serial.println("0 disconnect()\n1 linkLastDevice\n2 send AT command        A then type ... or ACOMMAND(including +!)");
        Serial.println("3 get available phonebooks at phone\n4 select phonebook\n5 search for item in selected phonebook 5 then type or 5 text");
        Serial.println("6 get selected phonebook on phone;");
        break;
      case 'r':
        BT.resetModule();
      break;
      case '0':
        BT.disconnect();
        break;
      case '1':
        BT.linkLastDevice(0);
        break;
      case '2':
        {
          c = Serial.read();
          char data[20];
          uint8_t i = 0;

          if (c == '\r' || c == '\n') {
            if (c == '\r') Serial.read(); //read \n if previous command was \r
            Serial.println(F("type command (without AT) to by send:"));
            while (!Serial.available()) {}; //wait until user input something ... then read it:
          } else
            data[i++] = c;


          while (Serial.available() > 0) {
            c = Serial.read();
            if ( c == 0xD ) {
              break;
            } else {
              data[i++] = c;
            }
            c = 0;
          }
          BT.vendorAtCommand(0, data);
          i = 0;
        }
        break;
      case '3':
        {
          BT.getAvailablePhonebooks(0);
          BT.getSelectedPhonebook(0);          
        }
        break;
      case '4':
        {
          BT.getAvailablePhonebooks(0);
          BT.getNextEventFromBt();
          BT.getNextEventFromBt();
          c;
          char pb[2];
          uint8_t i = 0;
          Serial.println("Select PB, type 2 leters code to select phonebook:");
          BT.printSupportedPB();
          while (!Serial.available()) {}; //wait until user input something ... then read it:
          while (Serial.available() > 0) {
            c = Serial.read();
            if ( c == 0xD ) {
              break;
            } else {
              pb[i++] = c;
            }
            c = 0;
          }
          if (i > 3) {
            Serial.println("2leters code only!");
            break;
          }
          BT.setPhonebook(0, pb);
          BT.getSelectedPhonebook(0);
        }
        break;
      case '5':
        {
          char c = Serial.read();
          char data[20];
          uint8_t i = 0;

          if (c == '\r' || c == '\n') {
            if (c == '\r') Serial.read(); //read \n if previous command was \r
            Serial.println(F("type search string:"));
            while (!Serial.available()) {}; //wait until user input something ... then read it:
          } else
            data[i++] = c;


          while (Serial.available() > 0) {
            c = Serial.read();
            if ( c == 0xD ) {
              break;
            } else {
              data[i++] = c;
            }
            c = 0;
          }
          Serial.println(ATFindPhonebookEntries);
          Serial.println("searching for: " + String(data));
          BT.findItemInPhonebook(0, data);
          i = 0;
        }
        break;
        case '6':
        {
          BT.getSelectedPhonebook(0);
          BT.printSupportedPB();
        }
        break;
    }
  }
  BT.getNextEventFromBt();
}

void deviceInfo(uint8_t deviceId) {
  Serial.print(F("=================================")); Serial.println();
  Serial.print(F("Device ")); Serial.print(deviceId); Serial.print(F("info:\n"));
  Serial.print(BT.connectionStatus(deviceId)); Serial.println();
  Serial.print(F("Name: ")); Serial.println(BT.deviceName[deviceId]);
  Serial.print(F("Music Status: ")); Serial.println(BT.musicStatus(deviceId));
  Serial.print(BT.streamStatus(deviceId)); Serial.println();
  Serial.print(F("Battery level dev0: ")); Serial.print(BT.currentBatteryLevel[deviceId]); Serial.print("/"); Serial.println(BT.maxBatteryLevel[deviceId]);
  Serial.print(F("current signal strength : ")); Serial.print(BT.currentSignalLevel[deviceId]); Serial.print("/"); Serial.println(BT.maxSignalLevel[deviceId]);
  Serial.print(F(" info:"));
  if (BT.deviceInBandRingtone[deviceId])
    Serial.print(F("Has in Band Rington."));
  if (BT.deviceIsIap[deviceId]) {
    Serial.print(F("- iAP device"));
  } else {
    Serial.print(F("- SPP device"));
  }

  if (BT.deviceSupportAvrcpV13[deviceId])
    Serial.print(F("- AVRCP 1.3 supported")); //reply if remote device support AVRCP v1.3
  if (BT.deviceHfAndA2dpGain[deviceId] != 0x00 ) {
    Serial.print("A2DP Gain: ");
    Serial.println(BT.deviceHfAndA2dpGain[deviceId] << 4);
    Serial.print("HF Gain: ");
    Serial.println(BT.deviceHfAndA2dpGain[deviceId] & 0x0F);
  }
  if ( BT.deviceLineInGain[deviceId] != 0x00 ) {
    Serial.print("Line in Gain: ");
    Serial.println(BT.deviceLineInGain[deviceId]);
  }
}
