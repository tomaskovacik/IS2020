#include "IS2020.h"
#include <Arduino.h>
#include "MMI.h"
#include "Music.h"
#include "commands.h"
#include "events.h"

IS2020::IS2020(HardwareSerial *ser) {
  btSerial = ser;
}

/*s
   Destructor
*/
IS2020::~IS2020() {
}

void IS2020::begin(uint8_t resetPin, uint32_t baudrate) {
  _reset = resetPin;
  btSerial -> begin(baudrate);
  pinMode(_reset, OUTPUT);
  IS2020::resetHigh();
  delay(100);
  IS2020::mcuUartRxBufferSize();
}

/*
   debug output
*/
void IS2020::DBG(String text) {
  if (DEBUG) /*return "DBG: ");*/ Serial.print(text);
}

void IS2020::DBG_AVRCP(String text) {
  if (DEBUG_AVRCP) /*return "DBG: ");*/ Serial.print(text);
}

void IS2020::DBG_EVENTS(String text) {
  if (DEBUG_EVENTS) /*return "DBG: ");*/ Serial.print(text);
}


void IS2020::resetLow() {
  digitalWrite(_reset, LOW);
}

void IS2020::resetHigh() {
  digitalWrite(_reset, HIGH);
}

void IS2020::resetModule() {
  DBG(F("reseting module\n"));
  resetLow();
  delayMicroseconds(1000);
  resetHigh();
}

uint8_t IS2020::checkResponce(uint8_t eventId) {
  IS2020::getNextEventFromBt();
  return true;
  //DBG(F("\nChecking responce : ")); decodeEvent(eventId);DBG(F("\n"));
  //uint8_t responceId = IS2020::getNextEventFromBt();
  //DBG(F("Get responce: "));decodeEvent(responceId); DBG(F("\n"));

  //  if (responceId == eventId) {
  //    DBG("OK\n\n");
  //    return true;
  //  } else {
  //    DBG("FAIL: ");
  //    decodeEvent(responceId);
  //    DBG(" != : ");
  //    decodeEvent(eventId);
  //    DBG("\n");
  //    return false;
  //  }
}


/*
*/
void  IS2020::sendPacketInt(uint8_t cmd, uint8_t data) {
  DBG("sending int: ");
  decodeCommand(cmd); DBG("\n");
  btSerial -> write(STARTBYTE); //DBG(String(STARTBYTE, HEX));
  btSerial -> write((byte)0x00);// DBG(F(" 0x00"));
  btSerial -> write(0x02);//DBG(F(" 0x02"));
  uint8_t checkSum = 0x02;
  btSerial -> write(cmd); //DBG(F(" ")); DBG(String(cmd, HEX));
  checkSum += (cmd);
  btSerial -> write(data); //DBG(F(" ")); DBG(String(data, HEX));
  checkSum += data;
  btSerial -> write(0x100 - checkSum); //DBG(F(" ")); DBG(String((0x100 - checkSum), HEX));
}

/*

*/
void  IS2020::sendPacketString(uint8_t cmd, String str) {
  //DBG("sending string: ");
  String tmp = F(" String: ");
  decodeCommand(cmd);
  DBG(tmp + str + "\n");
  uint16_t packetSize = str.length() + 1; //length of string + cmd
  btSerial -> write(STARTBYTE); //DBG(String(STARTBYTE, HEX));
  btSerial -> write(packetSize >> 8); //DBG(F(" ")); DBG(String((packetSize >> 8), HEX));
  uint8_t checkSum = packetSize >> 8;
  btSerial -> write(packetSize & 0xFF); //DBG(F(" ")); DBG(String((packetSize & 0xFF), HEX));
  checkSum += (packetSize & 0xFF);
  btSerial -> write(cmd); DBG(F(" ")); //DBG(String(cmd, HEX));
  checkSum += (cmd);
  for (uint16_t dataPos = 0; dataPos < packetSize - 1; dataPos++) {
    btSerial -> write(str[dataPos]); DBG(F("|")); DBG(String(str[dataPos],HEX));
    checkSum += str[dataPos];
  }
  btSerial -> write(0x100 - checkSum);// DBG(F(" ")); DBG(String(0x100 - checkSum, HEX));
}
/*

*/
void  IS2020::sendPacketArrayInt (uint16_t packetSize, uint8_t cmd, uint8_t deviceId, uint8_t data[]) {
  //DBG("sending array int: ");
  decodeCommand(cmd); DBG(F(": "));
  if (cmd == CMD_MMI_Action) IS2020::decodeMMI(data[3]);
  btSerial -> write(STARTBYTE); DBG(String(STARTBYTE, HEX));
  btSerial -> write(packetSize >> 8); DBG(F(" ")); DBG(String((packetSize >> 8), HEX));
  uint8_t checkSum = packetSize >> 8;
  btSerial -> write(packetSize & 0xFF); DBG(F(" ")); DBG(String((packetSize & 0xFF), HEX));
  checkSum += (packetSize & 0xFF);
  btSerial -> write(cmd); DBG(F(" ")); DBG(String(cmd, HEX));
  checkSum += (cmd);
  btSerial -> write(deviceId); DBG(F(" ")); //DBG(String(deviceId, HEX));
  checkSum += (deviceId);
  for (uint16_t dataPos = 0; dataPos < packetSize - 2; dataPos++) {
    btSerial -> write(data[dataPos]); DBG(F(" ")); DBG(String(data[dataPos], HEX));
    checkSum += data[dataPos];
  }
  btSerial -> write(0x100 - checkSum); DBG(F(" ")); DBG(String(0x100 - checkSum, HEX) + "\n");
}

//void  IS2020::sendPacketArrayInt_P(uint16_t packetSize, uint8_t cmd, uint8_t deviceId, uint8_t * data) {
//  //DBG("sending array int: ");
// // decodeCommand(cmd); DBG(F(": "));
// // if (cmd == CMD_MMI_Action) IS2020::decodeMMI(*data + 4);
//  btSerial -> write(STARTBYTE); //DBG(String(STARTBYTE, HEX));
//  btSerial -> write(packetSize >> 8); //DBG(F(" ")); DBG(String((packetSize >> 8), HEX));
//  uint8_t checkSum = packetSize >> 8;
//  btSerial -> write(packetSize & 0xFF);// DBG(F(" ")); DBG(String((packetSize & 0xFF), HEX));
//  checkSum += (packetSize & 0xFF);
//  btSerial -> write(cmd);// DBG(F(" ")); DBG(String(cmd, HEX));
//  checkSum += (cmd);
//  btSerial -> write(deviceId);// DBG(F(" ")); DBG(String(deviceId, HEX));
//  checkSum += (deviceId);
//  for (uint16_t dataPos = 0; dataPos < packetSize - 2; dataPos++) {
//    btSerial -> write(data); /*DBG(F(" "));*/ Serial.write(*data);//DBG(String(*data+dataPos));
//    checkSum += *data++;
//  }
//  btSerial -> write(0x100 - checkSum); //DBG(F(" ")); DBG(String(0x100 - checkSum, HEX) + "\n");
//}

void  IS2020::sendPacketArrayChar (uint16_t packetSize, uint8_t cmd, uint8_t deviceId, char data[]) {
  DBG("sending array char: ");
  decodeCommand(cmd);DBG("\n");
  btSerial -> write(STARTBYTE);//DBG(String(STARTBYTE, HEX));
  btSerial -> write(packetSize >> 8);// DBG(F(" ")); DBG(String((packetSize >> 8), HEX));
  uint8_t checkSum = packetSize >> 8;
  btSerial -> write(packetSize & 0xFF);// DBG(F(" ")); DBG(String((packetSize & 0xFF), HEX));
  checkSum += (packetSize & 0xFF);
  btSerial -> write(cmd); //DBG(F(" ")); //DBG(String(cmd, HEX));
  checkSum += (cmd);
  btSerial -> write(deviceId);DBG(String(deviceId, HEX));DBG(F("|"));
  checkSum += (deviceId);
  for (uint16_t dataPos = 0; dataPos < packetSize - 2; dataPos++) {
    btSerial -> write(data[dataPos]);DBG(String(data[dataPos]));DBG(F("|"));
    checkSum += data[dataPos];
  }
  btSerial -> write(0x100 - checkSum);DBG(String((0x100 - checkSum), HEX));
}

int IS2020::serialAvailable () {
  return btSerial -> available();
}

int IS2020::serialRead() {
  return btSerial -> read();
}

uint8_t IS2020::checkCkeckSum(int size, uint8_t data[]) {
  DBG_EVENTS(F("\nEvent from module: "));
  decodeEvent(data[0]); DBG_EVENTS(F(" ["));

  uint8_t csum = (size >> 8);
  csum += (size & 0xFF);
  //csum += data[0];
  for (uint8_t i = 0; i < size; i++) { //
    csum += data[i]; DBG_EVENTS(String(data[i], HEX) + F(","));
  }
  DBG_EVENTS(F("]\n"));

  if (data[size] == (0x100 - csum) ) {
    //DBG(F("Checksum OK\n"));
    return true;
  } else {
    //DBG(F("Bad checksum\n"));
    return false;
  }
}

String IS2020::connectionStatus(uint8_t deviceId) {
  String ConnectionStatus = "";
  /*
            Value Parameter Description
            0xXX  "1 indicate connected
            0x01 Bit0 : A2DP profile signaling channel connected
            0x02 Bit1 : A2DP profile stream channel connected
            0x04 Bit2 : AVRCP profile connected
            0x08 Bit3 : HF profile connected
            0x10 Bit4 : SPP connected"
  */
  if (linkStatus[1 + deviceId] & (1 << A2DP_profile_signaling_channel_connected)) ConnectionStatus += F("A2DP profile signaling channel connected\n");
  if (linkStatus[1 + deviceId] & (1 << A2DP_profile_stream_channel_connected)) ConnectionStatus += F("A2DP profile stream channel connected\n");
  if (linkStatus[1 + deviceId] & (1 << AVRCP_profile_connected)) ConnectionStatus += F("AVRCP profile connected\n");
  if (linkStatus[1 + deviceId] & (1 << HF_profile_connected)) ConnectionStatus += F("HF profile connected\n");
  if (linkStatus[1 + deviceId] & (1 << SPP_connected)) ConnectionStatus += F("SPP connected\n");
  if (linkStatus[1 + deviceId] == 0) ConnectionStatus = F("Disconnected\n");
  return ConnectionStatus;
}

String IS2020::musicStatus(uint8_t deviceId) {
  /*
            database1_play_status," SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "0x00:STOP
            0x01:PLAYING
            0x02:PAUSED
            0x03:FWD_SEEK
            0x04:REV_SEEK
            0x05:FAST_FWD
            0x06:REWIND
            0x07:WAIT_TO_PLAY
            0x08:WAIT_TO_PAUSE"
  */
  //  switch (linkStatus[3 + deviceId]) {
  switch (musicState[deviceId]) {
    case 0x00:
      return (F("STOP"));
      break;
    case 0x01:
      return (F("PLAYING"));
      break;
    case 0x02:
      return (F("PAUSED"));
      break;
    case 0x03:
      return (F("FWD SEEK"));
      break;
    case 0x04:
      return (F("REV SEEK"));
      break;
    case 0x05:
      return (F("FAST FWD"));
      break;
    case 0x06:
      return (F("REWIND"));
      break;
    case 0x07:
      return (F("WAIT_TO_PLAY"));
      break;
    case 0x08:
      return (F("WAIT_TO_PAUSE"));
      break;
  }
}

String IS2020::streamStatus(uint8_t deviceId) {
  /*
             database1_stream_status," SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "1 indicate connected
            0x00: no stream
            0x01: stream on-going"
  */
  if (linkStatus[5 + deviceId]) return (F("Streaming"));
  return (F("No stream"));
}

uint8_t IS2020::batteryLevel(uint8_t deviceId) {
  return (currentBatteryLevel[deviceId] * 100) / maxBatteryLevel[deviceId];
}

String IS2020::moduleState() {
  /*
            Event Parameters: device_state  SIZE: 1 BYTE
            Value Parameter Description
            0x00  Power OFF state
            0x01  pairing state (discoverable mode)
            0x02  standby state
            0x03  Connected state with only HF profile connected
            0x04  Connected state with only A2DP profile connected
            0x05  Connected state with only SPP profile connected
            0x06  Connected state with multi-profile connected
  */
  switch (linkStatus[0]) {
    case 0x00:
      return (F("Power OFF"));
      break;
    case 0x01:
      return (F("pairing state (discoverable mode)"));
      break;
    case 0x02:
      return (F("standby state"));
      break;
    case 0x03:
      return (F("Connected state with only HF profile connected"));
      break;
    case 0x04:
      return (F("Connected state with only A2DP profile connected"));
      break;
    case 0x05:
      return (F("Connected state with only SPP profile connected"));
      break;
    case 0x06:
      return (F("Connected state with HP and A2DP profile connected"));
      break;
  }
}

String IS2020::btStatus() {

  switch (btmState) {
    case 0x00:
      //return (F("Power OFF state"));
      return (F("Disconected!"));
      break;
    case 0x01:
      return (F("Pairing state (discoverable mode)"));
      break;
    case 0x02:
      return (F("Power ON state"));
      break;
    case 0x03:
      return (F("pairing successful"));
      break;
    case 0x04:
      return (F("pairing fail"));
      break;
    case 0x05:
      return (F("HF link established"));
      break;
    case 0x06:
      return (F("A2DP link established"));
      break;
    case 0x07:
      return (F("HF link disconnected"));
      break;
    case 0x08:
      return (F("A2DP link disconnected"));
      break;
    case 0x09:
      return (F("SCO link connected"));
      break;
    case 0x0A:
      return (F("SCO link disconnected"));
      break;
    case 0x0B:
      return (F("AVRCP link established"));
      break;
    case 0x0C:
      return (F("AVRCP link disconnected"));
      break;
    case 0x0D:
      return (F("Standard SPP connected"));
      break;
    case 0x0E:
      return (F("Standard_SPP / iAP disconnected"));
      break;
    case 0x0F:
      return (F("Standby state"));
      break;
    case 0x10:
      return (F("iAP connected"));
      break;
    case 0x11:
      return (F("ACL disconnected"));
      break;
    default:
      String tmp = F("Unknown stat");
      //return ("unknown state" + (char)btmState);
      return (tmp + (char)btmState);
      break;
  }
}


void IS2020::setEventMask(uint32_t mask) {
  EventMask |= mask;
  //Serial3.println(EventMask,HEX);
}

void IS2020::enableAllSettingEvent() {
  //0 : BTM will send this event
  //1 : BTM will not send this event"
  EventMask = 0x00000000;
  //bit0 reserved
  /*setEventMask(EMB_SPK_Module_state);
    setEventMask(EMB_call_status);
    setEventMask(EMB_incoming_call_number_or_caller_id);
    setEventMask(EMB_SMS_received);
    setEventMask(EMB_Missed_call);
    setEventMask(EMB_Max_cell_phone_battery_level);
    setEventMask(EMB_current_cell_phone_battery_level);

    setEventMask(EMB_cell_phone_roamming);
    setEventMask(EMB_Max_cell_phone_signal_strength);
    setEventMask(EMB_current_cell_phone_signal_strength);
    setEventMask(EMB_cell_phone_service_status);*/
  setEventMask(EMB_BTM_battery_level);
  setEventMask(EMB_BTM_charging_status);
  setEventMask(EMB_BMT_reset_to_default_setting_OK);
  setEventMask(EMB_BTM_DAC_gain_level);

  /* setEventMask(EMB_EQ_mode);
    setEventMask(EMB_remote_device_friendly_name);
    setEventMask(EMB_AVC_specific_response);
    setEventMask(EMB_unknown_AT_command_result_code);
    setEventMask(EMB_Page_status);
    setEventMask(EMB_Ringtone_status);
    setEventMask(EMB_amp_indication);
    setEventMask(EMB_line_in_status);*/
}


void IS2020::removeInfoAboutDevice(uint8_t deviceId) {
  deviceName[deviceId] = "";
  deviceInBandRingtone[deviceId] = 0;
  deviceIsIap[deviceId] = 0;
  deviceSupportAvrcpV13[deviceId] = 0;
  deviceHfAndA2dpGain[deviceId] = 0;
  deviceLineInGain[deviceId] = 0;
  maxBatteryLevel[deviceId] = 0;
  currentBatteryLevel[deviceId] = 0;

  btmStatusChanged = 1;
}
