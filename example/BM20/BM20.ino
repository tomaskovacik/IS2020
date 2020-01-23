#include "IS2020.h"

#define DEBUG 0
#define RESET PA1
#define BTSerial Serial3 //Serial2 => TX = PA2, RX = PA3

IS2020 BT(&BTSerial);

uint8_t selectedDevID = 0x00;
uint8_t ff = 0;
uint8_t rw = 0;
uint8_t repeat_mode = 0x01;

void deviceInfo(uint8_t dMeviceId);
void module_info();


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200); //TX = PB10, RX=PB11
  //Serial.print(F("begin"));
  // delay(1000);

  //  while (!Serial3) {
  //    ; // wait for serial port to connect. Needed for native USB port only
  //  }
  BT.begin(RESET);
  //BT.btmStatusChanged = 1;
  
  
  BT.readLocalDeviceName();
  BT.readLocalBtAddress();
  BT.readBtmVersion();
  BT.readLinkStatus();
  BT.readPairedDeviceRecord();
}

void loop() { // run over and over
Serial.println(BT.btStatus());
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    char c = Serial.read();
    switch (c)
    {
      case 'a':
        BT.avrcpGetPlayStatus(0);
        break;
      case 'b':
        if (BT.linkStatus[1] > 0) {
          BT.batteryStatus(0x00);
          BT.getNextEventFromBt();
          Serial.println(BT.maxBatteryLevel[0]);
          Serial.println(BT.currentBatteryLevel[0]);
        }
        if (BT.linkStatus[2] > 0) {
          BT.batteryStatus(0x00);
          Serial.println(BT.maxBatteryLevel[1]);
          Serial.println(BT.currentBatteryLevel[1]);
        }
        break;
      case 'c':
        BT.linkLastDevice(Serial.read());
        break;
      case 'e':
        BT.eventMaskSetting();
        break;
      case 'o':
        break;
      case 'd':
        BT.readLocalDeviceName();
        BT.readLocalBtAddress();
        BT.readBtmVersion();
        BT.readLinkStatus();
        BT.readPairedDeviceRecord();
        break;
      case 'n':
        BT.queryDeviceName(0x00);
        break;
      case '=':
        // NEXT SONG
        BT.nextSong(0x00);
        break;
      case '-':
        // PREVIOUS SONG
        BT.previousSong(0x00);
        break;
      case 'D':
        BT.eepromToDefaults(0);
        break;
      case 'E':
        BT.enableAllSettingEvent();
        BT.eventMaskSetting();
        break;

      case 'P':
        // seek rewind
        BT.play(0x00);
        break;
      case 'p':
        // PLAY/PAUSE
        BT.togglePlayPause(0x00);

        break;
      // seek forward            f
      // seek rewind             r
      // scan mode               s
      // shuffle mode            h
      case 'f':
        BT.ffw(0x00);
        break;
      case 'F':
        //  seek frward
        if (!ff) {
          ff = 1;
          BT.repFfw(0x00);
        } else {
          ff = 0;
          BT.stopFfwRwd(0x00);
        }
        break;
      case 'r':
        // seek rewind
        BT.rwd(0x00);
        break;
      case 'A':
        BT.avrcpGetElementAttributesAll(0x00);
        break;
      case 'R':
        if (!rw) {
          rw = 1;
          BT.repRwd(0x00);
        } else {
          rw = 0;
          
          BT.stopFfwRwd(0x00);
        }
        break;
      case 'S':
        // seek rewind
        BT.stop(0x00);
        break;
      case 'C':
        {
          delay(1000);
          char pn[19];
          uint8_t i = 0;
          while (Serial.available() > 0) {
            pn[i++] = Serial.read();
          }
          BT.makeCall(0x00, pn);
        }
        break;
      case 'l':
        if (BT.readLocalDeviceName()) Serial.print("local device name: "); Serial.println(BT.localDeviceName);
        break;
      case 'L':
        {
          delay(1000);
          String str;
          c = 0;
          while (Serial.available() > 0) {
            c = Serial.read();
            str += c;
          }
          BT.changeDeviceName(str);
          if (BT.readLocalDeviceName()) Serial.print("local device name: "); Serial.println(BT.localDeviceName);
        }
        break;
      case 'h': //help
        Serial.println("Query avrcp status                   a");   
        Serial.println("Battery status                       b");
        Serial.println("Link last device                     c");
        Serial.println("Enable defualt Events                e");
        Serial.println("Enable All Events                    E");
        Serial.println("Next track button                    =");
        Serial.println("Previous track button                -");
        Serial.println("Play/Pause                           p");
        Serial.println("PLAY                                 P");
        Serial.println("STOP                                 S");
        Serial.println("Seek forward/repeat ff               f/F");
        Serial.println("Seek rewind/repeate rw               r/R");
        Serial.println("Reset module                         q");
        Serial.println("Call                                 C");
        Serial.println("Read local device name               l");
        Serial.println("Help                                 h");
        Serial.println("Read dev info                        d");
        Serial.println("Eeprom_to_defaults                   D");
        Serial.println("Read info about connected phone      n");
        Serial.println("Link info                            I");
        Serial.println("Switch_primary_seconday_HF           s");
        break;
      case 'i': //info
        {
          moduleInfo();
          deviceInfo(0);
          deviceInfo(1);
        }
        break;
      case 's':
        {
          BT.switchPrimarySecondayHf(1);
        }
        break;
      case 'I':
        {
          BT.readBtmVersion();

        }
        break;
      case 'q':
        BT.resetModule();
      break;
    }
  }

  BT.getNextEventFromBt();
  if (BT.btmStatusChanged) {
    //    moduleInfo();
    //    deviceInfo(0);
    //    deviceInfo(1);
    BT.btmStatusChanged = 0;
  }
}

void moduleInfo() {
  Serial.print(F("Module info:"));
  Serial.print(F("BT module name: ")); Serial.println(BT.localDeviceName);
  Serial.print(F("BT module addr: "));
  for (uint8_t _byte = 0; _byte < 6; _byte++) {
    Serial.print(BT.moduleBtAddress[_byte], HEX);
    if (_byte < 5) Serial.print(":");
  }
  Serial.println();
  Serial.println(F("Paired devices: "));
  for (uint8_t dev = 1; dev < 4; dev++) {
    Serial.print(dev); Serial.print(". ");
    for (uint8_t _byte = 0; _byte < 7; _byte++) {
      Serial.print(BT.btAddress[dev][_byte], HEX);
      if (_byte < 5) Serial.print(":");
    }
    Serial.println();
  }
  Serial.println();
  Serial.println(BT.moduleState());
  Serial.println(BT.btStatus());

  /*
    Event Format: Event Event Code  Event Parameters
    Read_BTM_Version_Reply  0x18  type, version

    Description:

    Event Parameters: type  SIZE: 1 BYTE
    Value Parameter Description
    0x00  uart version
    0x01  BTM FW version

    version SIZE: 2 Octets
    Value Parameter Description
    0xXXYY
    1st byte bit[7:5]: flash version
    1st byte bit[4:0]: rom version
    2nd byte bit[7:4] : flash sub version
    2nd byte bit[3:0] : flash control version
    for example 00 07 means version 0.07

  */
  Serial.println();
  Serial.println(F("       UART version: ")); Serial.println(BT.btmUartVersion, HEX);
  Serial.print(F("        flash version: ")); Serial.println((uint8_t)(BT.btmUartVersion >> 13), HEX);
  Serial.print(F("          rom version: ")); Serial.println((uint8_t)((BT.btmUartVersion >> 8) & 0x1F), HEX);
  Serial.print(F("    flash sub version: ")); Serial.println(((uint8_t)(BT.btmUartVersion >> 4) & 0x0F), HEX);
  Serial.print(F("flash control version: ")); Serial.println((uint8_t)(BT.btmUartVersion & 0x0F), HEX);
  Serial.println();
  Serial.println(F("      BT FW version: ")); Serial.println(BT.btmFwVersion >> 13, HEX);
  Serial.print(F("        flash version: ")); Serial.println((uint8_t)(BT.btmFwVersion >> 13), HEX);
  Serial.print(F("          rom version: ")); Serial.println((uint8_t)((BT.btmFwVersion >> 8) & 0x1F), HEX);
  Serial.print(F("    flash sub version: ")); Serial.println(((uint8_t)(BT.btmFwVersion >> 4) & 0x0F), HEX);
  Serial.print(F("flash control version: ")); Serial.println((uint8_t)(BT.btmFwVersion & 0x0F), HEX);

}

void deviceInfo(uint8_t deviceId) {
  Serial.print(F("================================="));Serial.println();
  Serial.print(F("Device ")); Serial.print(deviceId); Serial.print(F("info:\n"));
  Serial.print(BT.connectionStatus(deviceId));Serial.println();
  Serial.print(F("Name: ")); Serial.println(BT.deviceName[deviceId]);
  Serial.print(F("Music Status: ")); Serial.println(BT.musicStatus(deviceId));
  Serial.print(BT.streamStatus(deviceId));Serial.println();
  Serial.print(F("Battery level dev0: ")); Serial.print(BT.currentBatteryLevel[deviceId]); Serial.print("/"); Serial.println(BT.maxBatteryLevel[deviceId]);
  Serial.print(F("current signal strength : ")); Serial.print(BT.currentSignalLevel[deviceId]); Serial.print("/"); Serial.println(BT.maxSignalLevel[deviceId]);
  //    Serial.print(F(" info:");
  //    if (BT.deviceInBandRingtone[deviceId])
  //      Serial.print(F("Has in Band Rington."));
  //    if (BT.deviceIsIap[deviceId]) {
  //      Serial.print(F("- iAP device");
  //    } else {
  //      Serial.print(F("- SPP device");
  //    }
  //
  //    if (BT.deviceSupportAvrcpV13[deviceId])
  //      Serial.print(F("- AVRCP 1.3 supported")); //reply if remote device support AVRCP v1.3
  //    if (BT.deviceHfAndA2dpGain[deviceId] != 0x00 ) {
  //      Serial.print("A2DP Gain: ");
  //      Serial.println(BT.deviceHfAndA2dpGain[deviceId] << 4);
  //      Serial.print("HF Gain: ");
  //      Serial.println(BT.deviceHfAndA2dpGain[deviceId] & 0x0F);
  //    }
  //    if ( BT.deviceLineInGain[deviceId] != 0x00 ) {
  //      Serial.print("Line in Gain: ");
  //      Serial.println(BT.deviceLineInGain[deviceId]);
  //    }
  //  }


}
