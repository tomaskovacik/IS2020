#include "IS2020.h"
#include <Arduino.h>

/*

  Command Format:  Command Command ID  Command Parameters  Return Event
  Make_Call 0x00  data_base_index, phone_number call status

  Description:  This command is used to trigger HF action for making an outgoing call.

  Command Parameters: data_base_index SIZE: 1 BYTE
  Value Parameter Description
  0x00  database 0 for a dedicate link
  0x01  database 1 for a dedicate link

  phone_number  SIZE: 19 BYTES
  Value Parameter Description
  0xXXXX  Phone number of ASCII code. The max length of phone number is 19 byte.

*/
uint8_t IS2020::makeCall(uint8_t deviceId, char phoneNumber[19]) { //return event from BT will be handled elseware??? event call status
  IS2020::getNextEventFromBt();
  IS2020::DBG("MakeCall\n");
  IS2020::sendPacketArrayChar(21, CMD_Make_Call, deviceId, phoneNumber);
  return checkResponce(EVT_Call_Status);
}

/*

  Command Format:  Command Command ID  Command Parameters  Return Event
  Make_Extension_Call 0x01  data_base_index, extension_number

  Description:  This command is used to trigger HF action for making an extension call number.

  Command Parameters: data_base_index SIZE: 1 BYTE
  Value Parameter Description
  0x00  database 0 for a dedicate link
  0x01  database 1 for a dedicate link

  extention_number  SIZE: 10 BYTES
  Value Parameter Description
  0xXXXX  Extension phone number of ASCII code. The max length of phone number is 10 byte


*/
uint8_t IS2020::makeExtensionCall(uint8_t deviceId, char phoneNumber[10]) {
  IS2020::getNextEventFromBt();
  IS2020::DBG("Make_Extension_Call\n");
  IS2020::sendPacketArrayChar(12, CMD_Make_Extension_Call, deviceId, phoneNumber);
  return checkResponce(EVT_Command_ACK);
}

/*

  Command Format:   Command Command ID  Command Parameters      Return Event
  MMI_Action        0x02                data_base_index, action

  Description:  MMI action

  Command Parameters: data_base_index SIZE: 1 BYTEf
  Value Parameter Description
  0x00  database 0 that related to a dedicate HF device
  0x01  database 1 that related to a dedicate HF device

  action  SIZE: 1 BYTE
  Value Parameter Description
  0x01  add/remove SCO link
  0x02  force end active call
  0x04  Accept an incoming call
  0x05  Reject an incoming call
  0x06  1. End call if SCO exist. 2. Voice transfer to headset if SCO not exist.
  0x07  1. Mute microphone if microphone is not mute 2. Active microphone if microphone is mute
  0x08  Mute microphone
  0x09  Active microphone
  0x0A  voice dial
  0x0B  cancel voice dial
  0x0C  last number redial
  0x0D  Set the active call on hold and active the hold call
  0x0E  voice transfer
  0x0F  Query call list information(CLCC)
  0x10  three way call
  0x11  release the waiting call or on hold call
  0x12  accept the waiting call or active the on hold call and release the active call
  0x16  initiate HF connection
  0x17  disconnect HF link
  0x24  increase microphone gain
  0x25  decrease microphone gain
  0x26  switch primary HF device and secondary HF device role
  0x30  increase speaker gain
  0x31  decrease speaker gain
  0x34  Next song
  0x35  previous song
  0x3B  Disconnect A2DP link
  0x3C  next audio effect
  0x3D  previous audio effect
  0x50  enter pairing mode (from power off state)
  0x51  power on button press
  0x52  power on button release
  0x56  Reset some eeprom setting to default setting
  0x5D  fast enter pairing mode (from non-off mode)
  0x5E  switch power off:  to execute the power_off process directly, actually, the combine command set, power off button press and release, could be replace by this command.
  0x60  Enable buzzer if buzzer is OFF Disable buzzer if buzzer is ON
  0x61  Disable buzzer
  0x62  Enable buzzer Disable buzzer if buzzer is ON
  0x63  Change tone set (SPK module support two sets of tone)
  0x6A  Indicate battery status
  0x6B  Exit pairing mode
  0x6C  link last device
  0x6D  disconnect all link


  Note1:Query call list information(CLCC)
  action  SIZE: 1 BYTE
  Value Parameter Description
  0x0f  trigger SPK Module to query call list information


*/
uint8_t  IS2020::mmiAction(uint8_t deviceId, uint8_t action) {
  IS2020::getNextEventFromBt();
  IS2020::DBG("MMI_Action\n");
  uint8_t data[1] = {action};
  IS2020::sendPacketArrayInt(0x03, CMD_MMI_Action, deviceId, data);
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:     Command Command ID  Command Parameters  Return Event
  Event_Mask_Setting  0x03                Mask

  Description:  This command is used to set the specific event mask that host MCU would not like to receive it from BTM.

  Command Parameters: Mask  SIZE: 4 BYTES
  Value Parameter Description
  0xXXXXXXXX  "Refer to Report Mask Table(bit mask)
  0 : BTM will send this event
  1 : BTM will not send this event"

  Report Mask Table
  Byte 0  Parameter Description

  Bit 0 reserved
  Bit 1 SPK Module state
  Bit 2 call status
  Bit 3 incoming call number or caller id
  Bit 4 SMS received
  Bit 5 Missed call
  Bit 6 Max cell phone battery level
  Bit 7 current cell phone battery level

  Byte 1  Parameter Description
  Bit 0 cell phone roamming
  Bit 1 Max cell phone signal strength
  Bit 2 current cell phone signal strength
  Bit 3 cell phone service status
  Bit 4 BTM battery level
  Bit 5 BTM charging status
  Bit 6 BMT reset to default setting OK
  Bit 7 BTM DAC gain level

  Byte 2  Parameter Description
  Bit 0 EQ mode
  Bit 2 AVRCP specific response
  Bit 3 unknown AT command result code
  Bit 4 Page status
  Bit 5 Ringtone status
  Bit 6 amp indication
  Bit 7 line in status

  Byte 3  Parameter Description
  Bit 0 reserved
  Bit 1 reserved
  Bit 2 reserved
  Bit 3 reserved
  Bit 4 reserved
  Bit 5 reserved
  Bit 6 reserved
  Bit 7 reserved

*/
uint8_t  IS2020::eventMaskSetting() {
  IS2020::getNextEventFromBt();
  IS2020::DBG("Event_Mask_Setting\n");
  uint8_t data[3]={
    ((IS2020::EventMask >> 16) & 0xFF),
    ((IS2020::EventMask >> 8) & 0xFF),
    (IS2020::EventMask & 0xFF)
  };
  IS2020::sendPacketArrayInt (5, CMD_Event_Mask_Setting, ((EventMask>>24) & 0xFF), data);
  return checkResponce(EVT_Command_ACK);
}
/*

  Command Format:  Command Command ID  Command Parameters  Return Event
  Music_Control 0x04  data_base_index, action

  Description:  This command is used to trigger AVRCP command for music control.

  Command Parameters:
  data_base_index SIZE: 1 BYTE
  Value Parameter Description
  0x00  reserved

  action  SIZE: 1 BYTE
  Value Parameter Description
  0x00  Stop fast forward or rewind
  0x01  fast forward
  0x02  fast forward with repeat send fast forward command every 800ms
  0x03  rewind
  0x04  rewind with repeat send rewind command every 800ms
  0x05  PLAY command
  0x06  PAUSE command
  0x07  PLAY PAUSE toggle
  0x08  STOP command


*/

uint8_t  IS2020::musicControl(uint8_t deviceId, uint8_t action) {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("MusicControl :"));
  switch (action) {
    case 0x00:
      DBG(F("STOP_FFW"));
      break;
    case 0x01:
      DBG(F("FFW"));
      break;
    case 0x02:
      DBG(F("REPFFW"));
      break;
    case 0x03:
      DBG(F("RWD"));
      break;
    case 0x04:
      DBG(F("REPRWD"));
      break;
    case 0x05:
      DBG(F("PLAY"));
      break;
    case 0x06:
      DBG(F("PAUSE"));
      break;
    case 0x07:
      DBG(F("TOGLE_PLAY_PAUSE"));
      break;
    case 0x08:
      DBG(F("STOP"));
      break;
  }
  DBG("\n");
  uint8_t data[1] = {action};
  IS2020::sendPacketArrayInt(0x03, CMD_Music_Control, deviceId, data);
  return checkResponce(EVT_Command_ACK);
}
/*

*/
uint8_t  IS2020::changeDeviceName(String name) {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Change Device Name\n"));
  IS2020::sendPacketString(CMD_Change_Device_Name, name);
  return checkResponce(EVT_Command_ACK);
}
/*

*/
uint8_t  IS2020::changePinCode() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Change PIN Code\n"));
  return checkResponce(EVT_Command_ACK);
}
/*

*/
uint8_t  IS2020::btmParameterSetting() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("BTM Parameter Setting\n"));
  return checkResponce(EVT_Command_ACK);
}
/*

*/
uint8_t  IS2020::readBtmVersion() {
  IS2020::getNextEventFromBt();
  //IS2020::DBG("Read BTM Version\n");
  IS2020::sendPacketInt(CMD_Read_BTM_Version, 0x00);
  IS2020::sendPacketInt(CMD_Read_BTM_Version, 0x01);
  //if (checkResponce(EVT_Command_ACK))
  return checkResponce(EVT_Read_BTM_Version_Reply);
  //return false;
}

/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  Get_PB_By_AT_Cmd  0x09  data_base_index Get_PB_By_AT_Cmd_Reply

  Description:  This command is used to access the contact list of mobile  phone via AT command.


  Command Parameters: data_base_index SIZE: 1 BYTE
  Value Parameter Description
  0x00  database 0 for a dedicate link
  0x01  database 1 for a dedicate link

*/
uint8_t  IS2020::getPbByAtCmd  (uint8_t deviceId)
{
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Get PB By AT Cmd\n"));
  IS2020::sendPacketInt(CMD_Get_PB_By_AT_Cmd, deviceId);
  return checkResponce(EVT_Command_ACK);
//  if (checkResponce(EVT_Command_ACK))
//    return checkResponce(EVT_Get_PB_By_AT_Cmd_Reply);
//  return false;
}

uint8_t  IS2020::vendorAtCommand() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Vendor_AT_Command\n"));
  return checkResponce(EVT_Command_ACK);
}

uint8_t  IS2020::avrcpSpecificCmd() {
  IS2020::getNextEventFromBt();
  //IS2020::DBG("AVRCP_Specific_Cmd\n");
  uint8_t data[17] = {0x20, //6
                      0x00, //7
                      0x00, 0x0d, // 8-9 D => 13 bytes
                      0x00, 0x00, 0x00, 0x00,//10,11,12,13 Identifier 8
                      0x02, //14 NumAttributes (N) 1
                      0x00, 0x00, 0x00, 0x01, //15,16,17,18,19,20,21,22
                      0x00, 0x00, 0x00, 0x07
                     }; //23,24,25,26
  IS2020::sendPacketArrayInt(19, CMD_AVRCP_Specific_Cmd, 0, data);
  return checkResponce(EVT_Command_ACK);
//  if (checkResponce(EVT_Command_ACK))
//    return checkResponce(EVT_AVRCP_Specific_Rsp);
//  return false;
}
/*

*/
uint8_t  IS2020::avrcpGroupNavigation() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("AVRCP_Group_Navigation\n"));
  return checkResponce(EVT_Command_ACK);
}

/*
  Command Format: Command Command ID  Command Parameters  Return Event
  Read_Link_Status  0x0D  dummy_byte  Read_Link_Status_Reply

  Description:  It is used to query device and profile link status

  Command Parameters: dummy_byte  SIZE: 1 BYTE
  Value Parameter Description
  0x00  reserved

*/
uint8_t  IS2020::readLinkStatus() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Read_Link_Status\n"));
  IS2020::sendPacketInt(CMD_Read_Link_Status, DUMMYBYTE);
  return checkResponce(EVT_Command_ACK);
//  if (checkResponce(EVT_Command_ACK)) {
//    return checkResponce(EVT_Read_Link_Status_Reply);
//  } else {
//    return false;
//  }
}

/*

  Command Format:  Command Command ID  Command Parameters  Return Event
  Read_Paired_Device_Record 0x0E  dummy_byte  Read_Paired_Device_Record_Reply

  Description:  This command is used to read the paired device information of BTM recorded.

  Command Parameters: dummy_byte  SIZE: 1 BYTE
    Parameter Description
  dummy_byte  RFD


  Event Format:  Event Event Code  Event Parameters
  Read_Paired_Device_Record_Reply 0x1F  paired_device_number, paired_record
  uint8_t RX, uint8_t TX
  Description:  This event is used to reply the  Read_Paired_Device_Information command.


  -----------------Read_Paired_Device_Record_Reply--------------------------------------------------
  Event Parameters: paired_device_number  SIZE: 1 BYTE
    Parameter Description
  byte0 the paired device number.

  paired_record : 7bytes per record SIZE: (7*total_record) BYTE
    Parameter Description
  byte0 link priority : 1 is the highest(newest device) and 4 is the lowest(oldest device)
  byte1~byte6 linked device BD address (6 bytes with low byte first)
  …
  --------------------------------------------------------------------------------------------

*/
uint8_t  IS2020::readPairedDeviceRecord() {
  IS2020::getNextEventFromBt();
  IS2020::DBG_AVRCP(F("Read_Paired_Device_Record\n"));
  IS2020::sendPacketInt(CMD_Read_Paired_Device_Record, DUMMYBYTE);
  return checkResponce(EVT_Command_ACK);
//    if (checkResponce(EVT_Command_ACK))
//    return checkResponce(EVT_Read_Paired_Device_Record_Reply);
}

/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  Read_Local_BT_Address 0x0F  dummy_byte  Read_Local_BT_Address_Reply

  Description:  It is used to read local BD Address

  Command Parameters: dummy_byte  SIZE: 1 BYTE
    Parameter Description
  dummy_byte  RFD

*/
uint8_t  IS2020::readLocalBtAddress() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Read_Local_BT_Address\n"));
  IS2020::sendPacketInt(CMD_Read_Local_BT_Address, DUMMYBYTE);
  return checkResponce(EVT_Command_ACK);
//  if (checkResponce(EVT_Command_ACK))
//    return checkResponce(EVT_Read_Local_BT_Address_Reply);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  Read_Local_Device_Name  0x10  dummy_byte  Read_Local_Device_Name_Reply

  Description:  Used to query local device name

  Command Parameters: dummy_byte  SIZE: 1 BYTE
    Parameter Description
  dummy_byte  RFD

*/
uint8_t  IS2020::readLocalDeviceName() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Read_Local_Device_Name\n"));
  IS2020::sendPacketInt(CMD_Read_Local_Device_Name, DUMMYBYTE);
  return checkResponce(EVT_Command_ACK);
//  if (checkResponce(EVT_Command_ACK))
//    return checkResponce(EVT_Read_Local_Device_Name_Reply);
}
/*
  Command Format: Command Command ID  Command Parameters  Return Event
  Set_Access_PB_Method  0x11  method

  Description:  Used set access phone book contacts or call history method.

  Command Parameters: method  SIZE: 1 BYTE
    Parameter Description
  0xXX  "Bit[5:0] : Reserved
  Bit6 : enable use AT command to retrieve phone book contacts or call history.
  Bit7 : enable use PBAP to retrieve phone book contacts or call history

  PS : if both AT and PBAP enabled, the SPK will try PBAP firstly."

*/
uint8_t  IS2020::setAccessPbMethod() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Set_Access_PB_Method\n"));
  IS2020::sendPacketInt(CMD_Set_Access_PB_Method, 0x18);
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  Send_SPP/iAP_Data 0x12  data_base_index, type, total_length, payload_length, payload

  Description:  This command is used to send the SPP/iAP data to remote BT devices.

  Command Parameters: data_base_index SIZE: 1 BYTE
  Value Parameter Description
  0x00  database 0 that related to a dedicate SPP link
  0x01  database 1 that related to a dedicate SPP link

  type  SIZE: 1 BYTE
  Value Parameter Description
  0x00  single packet
  0x01  fragmented start packet
  0x02  fragmented continue packet
  0x03  fragmented end packet

  total_length  SIZE: 2 BYTES
  Value Parameter Description
  0xXXXX  total payload length

  payload_length  SIZE: 2 BYTES
  Value Parameter Description
  0xXXXX  payload length in this packet

  payload SIZE: N BYTES
  Value Parameter Description
  0xXXXX  the payload in this packet

*/
uint8_t  IS2020::sendSppIapData() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Send_SPP_iAP_Data\n"));
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  BTM_Utility_Function  0x13  utility_function_type, parameter

  Description:  This command is used to indicate BTM to execute the specific utility function.

  Command Parameters: utility_function_type SIZE: 1 BYTE
    Parameter Description
  0x00  Host MCU ask BTM to process NFC detected function.
  0x01  Notice BTM the aux line in is detected. BTM will process build-in aux line-in detection procedure.
  0x02  Ask BTM to generate the specific tone.
  0x03  Forced nonconnectable setting.
  0x05  external TTS indication.

  parameter SIZE: 1 BYTE
  utility_function_type=0x00
    Parameter Description
  0xXX  reserved

  utility_function_type=0x01
    Parameter Description
  0x00  "audio in disable
  "
  0x01  audio in enable

  utility_function_type=0x02
    Parameter Description
  0xXX  "tone type

  utility_function_type=0x03 :
  0x00 : force BTM into Inactive mode (do not Rx)
  others : resume BTM to normal mode

  utility_function_type=0x04 :
  0x00 : to indicate BTM leave CONNECTABLE mode
  0x01 : to indicate BTM enter CONNECTABLE mode "

  utility_function_type=0x03
    Parameter Description
  0x00  " force BTM into nonconnectable mode
  "
  0x01  " resume BTM to normal mode
  "

  utility_function_type=0x05
    Parameter Description
  0x01  to indicate BTM that remote device supported TTS engine. The BTM shall disable internal TTS engine.
  others  reserved

  tone type
  Index Freq  Duration/note
  0 N/A 0
  1 200Hz 100msec
  2 500Hz 100msec
  3 1KHz  100msec
  4 1.5KHz  100msec
  5 2KHz  100msec
  6 200Hz 1 sec
  7 500Hz 1 sec
  8 1KHz  1 sec
  9 1.5KHz  1 sec
  0x0A  2KHz  1 sec
  0x0B  200Hz / mute / 200Hz  100msec for each tone
  0x0C  500Hz / mute /500Hz 100msec for each tone
  0x0D  1KHz / mute /1KHz 100msec for each tone
  0x0E  1.5KHz / mute /1.5KHz 100msec for each tone
  0x0F  2KHz / mute / 2KHz  100msec for each tone
  0x10  200Hz / mute /200Hz / mute /200Hz 100msec for each tone
  0x11  500Hz / mute /500Hz / mute /500Hz 100msec for each tone
  0x12  1KHz / mute /1KHz / mute /1KHz  100msec for each tone
  0x13  1.5KHz / mute /1.5KHz / mute /1.5KHz  100msec for each tone
  0x14  2KHz / mute /2KHz / mute /2KHz  100msec for each tone
  0x15  200Hz / mute /200Hz / mute /200Hz mute / / 200Hz  100msec for each tone
  0x16  500Hz / mute /500Hz / mute /500Hz / mute /500Hz 100msec for each tone
  0x17  1KHz / mute /1KHz / mute /1KHz / mute /1KHz 100msec for each tone
  0x18  1.5KHz / mute /1.5KHz / mute /1.5KHz / mute /1.5KHz 100msec for each tone
  0x19  2KHz / mute /2KHz / mute /2KHz / mute /2KHz 100msec for each tone
  0x1A  500Hz / 400Hz / 300Hz / 200Hz 50msec for each tone
  0x1B  200Hz / 300Hz / 400Hz / 500Hz 50msec for each tone
  0x1C  400Hz / 300Hz 150msec for each tone
  0x1D  300Hz / 400Hz 150msec for each tone
  0x1E  300Hz / mute / 400Hz / mute / 500Hz / mute / 1000Hz 100msec for each tone
  0x1F  1000Hz / mute /500Hz / mute / 400Hz / mute /300Hz 100msec for each tone

  0x20  ROM build-in multi tone melody
  0x21
  0x22
  0x23
  0x24
  0x25
  0x26

*/
uint8_t  IS2020::btmUtilityFunction() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("BTM_Utility_Function\n"));
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  Event_ACK 0x14  event_id

  Description:  This command is used for MCU to ack the received BTM's EVENT.


  Command Parameters: event_id  SIZE: 1 BYTE
    Parameter Description
  0xXX  the acked event_id.

*/
uint8_t  IS2020::eventAck(uint8_t eventId) {
  //IS2020::getNextEventFromBt();
  //IS2020::DBG(F("Event ACK: "));decodeEvent(eventId);
  IS2020::sendPacketInt(CMD_Event_ACK, eventId);
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  Additional_Profiles_Link_Setup  0x15  database_index, linked_profile

  Description:  This command is used to ask BTM initiate other profile connection base on already exist linked profiles.

  Command Parameters: database_index  SIZE: 1 BYTE
  Value Parameter Description
  0x00  database 0 that linked_profile occupied
  0x01  database 1 that linked_profile occupied

  linked_profile  SIZE: 1 BYTE
    Parameter Description
  0x00  HF/HS profile
  0x01  A2DP profile
  0x02  iAP/SPP profile

*/
uint8_t  IS2020::additionalProfilesLinkSetup() {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("Additional Profiles Link Setup\n"));
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  readLinkedDeviceInformation  0x16  database_index, type  readLinkedDeviceInformation_Reply

  Description:  MCU can use this to retrieve linked device bluetooth info, such as device name and in-band ringtone status.

  Command Parameters: database_index  SIZE: 1 BYTE
  Value Parameter Description
  0x00  database 0 for a dedicate link
  0x01  database 1 for a dedicate link

  type  SIZE: 1 BYTE
  Value Parameter Description
  0x00  query device name
  0x01  query in-band ringtone status
  0x02  query if remote device is iAP device
  0x03  query if remote device support AVRCP v1.3
  0x04  query HF&A2DP gain
  0x05  query Line_In gain
  others  reserved

*/
uint8_t  IS2020::readLinkedDeviceInformation(uint8_t deviceId, uint8_t query) {
  IS2020::getNextEventFromBt();
  IS2020::DBG(F("readLinkedDeviceInformation "));IS2020::DBG(String(deviceId));
  uint8_t data[1] = {query};
  IS2020::sendPacketArrayInt(0x03, CMD_Read_Linked_Device_Information, deviceId, data);
  return checkResponce(EVT_Command_ACK);
//  if (checkResponce(EVT_Command_ACK))
//    return checkResponce(EVT_readLinkedDeviceInformation_Reply);
//  return false;
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  Profile_Link_Back 0x17  type, device_index, profile

  Description:  This command is used to trigger the link back behavior for specific profiles.

  Command Parameters: type  SIZE: 1 BYTE
    Parameter Description
  0x00  connect to last device : if last device supports HF/HS, then initiate HF/HS connection, otherwise initiate A2DP connection
  0x01  initiate HF/HS connection to last HF/HS device
  0x02  initiate A2DP connection to last A2DP device
  0x03  initiate SPP/iAP connection to last SPP/iAP device
  0x04  initiate connection to dedicate device. <<<< previously paired!

  device_index  SIZE: 1 BYTE
  type: 0x04
    Parameter Description
  0x00  the range of device index is from 0 to 7.

  profile SIZE: 1 BYTE
  type: 0x04
    Parameter Description
  0x00  "0: the profile determined by BTM's e2prom record. => 0x00
  bit0 is HS profile. => 0x01
  bit1 is HF profile. => 0x02
  bit2 is A2DP profile."  => 0x04

defines in command.h:
#define LAST_DEVICE 0x00        //connect to last device : if last device supports HF/HS, then initiate HF/HS connection, otherwise initiate A2DP connection
#define INIT_HF_HS_CONNECTION 0x01      //initiate HF/HS connection to last HF/HS device
#define INIT_A2DP_CONNECTION 0x02       //initiate A2DP connection to last A2DP device
#define INIT_SPP_iAP_CONNECTION 0x03    //initiate SPP/iAP connection to last SPP/iAP device
#define INIT_DEDICATED_CONNECTION 0x04  //initiate connection to dedicate device.

*/
uint8_t  IS2020::profileLinkBack(uint8_t type, uint8_t deviceId, uint8_t profile) {
  IS2020::getNextEventFromBt();
//  DBG(F("Trying to connect device id: "));DBG(String(deviceId));DBG(F(" profile: "));DBG(String(profile));
  uint8_t data[2] = {deviceId,profile};
  IS2020::sendPacketArrayInt(0x03, CMD_Profile_Link_Back, type, data);
  return checkResponce(EVT_Command_ACK);
}

uint8_t IS2020::connectLastDevice(){
	IS2020::profileLinkBack(0,0,7);
}

/*
  Action Format:  Command Command ID  Command Parameters  Return Event
  DISCONNECT  0x18  disconnection flag

  Description:  This command is used to cancel the ongoing link back procedure or disconnect the linked profiles. BTM will disconnect ACL link if all of the profiles is disconnected.

  Command Parameters: disconnection flag  SIZE: 1 BYTE
    Parameter Description
  bit 0 Cancel page before ACL connection has been created.
  bit 1 Disconnect HF.
  bit 2 Disconnect A2DP.
  bit 3 Disconnect SPP.

*/
uint8_t  IS2020::disconnect(uint8_t flag) {
  IS2020::getNextEventFromBt();
  IS2020::sendPacketInt(CMD_Disconnect,flag);
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:       Command Command ID  Command Parameters    Return Event
  MCU_Status_Indication 0x19                status_type, status

  Description:  It is use to notify the MCU status to BTM.

  Command Parameters: status_type SIZE: 1 BYTE
  Value Parameter Description
  0x00  amplifier status
  others  RFD

  status  SIZE: 1 BYTE
   status_type =0x00
    Parameter Description
  0x00  Amplifier Mute or OFF finished
  0x01  Amplifier Unmute or ON finished
  others  reserved

*/
uint8_t  IS2020::mcuStatusIndication() {
  IS2020::getNextEventFromBt();
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:             Command Command ID  Command Parameters            Return Event
  User_Confirm_SPP_Req_Reply  0x1A                database_index, User_Response

  Description:  This command is used to reply to a User_Confirm_SPP_Req_Reply event and indicates that the user selected "yes" or "no".

  Command Parameters:
  database_index  SIZE: 1 BYTE
  Value Parameter Description
  0x00  database 0 that simple pairing is ongoing
  0x01  database 1 that simple pairing is ongoing

  User_Response SIZE: 1 BYTE
    Parameter Description
  0x00  User selected "yes"
  0x01  User selected "no"

*/
uint8_t  IS2020::userConfirmSppReqReply() {
  IS2020::getNextEventFromBt();
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  Set_HF_Speaker_Gain_Level 0x1B  database_index, gain_level

  Description:  This command is used to set HF gain of BTM. BTM will send AT command to notice AG

  Command Parameters:
  database_index  SIZE: 1 BYTE
  Value Parameter Description
  0x00  database 0 that related to a dedicate HF device
  0x01  database 1 that related to a dedicate HF device


  gain_level  SIZE: 1 BYTE
    Parameter Description
  0x00 - 0x0F Set HF speaker gain level.

*/
uint8_t  IS2020::setHfGainLevel() {
  IS2020::getNextEventFromBt();
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  EQ_Mode_Setting 0x1C  EQ_mode,dummy EQ_Mode_Indication(0x10)

  Description:  It is used to set the EQ mode of BTM for audio playing.

  Command Parameters:
  EQ_mode SIZE: 1 BYTE
  Value Parameter Description
  0x00  EQ_MODE_OFF
  0x01  EQ_MODE_SOFT
  0x02  EQ_MODE_BASS
  0x03  EQ_MODE_TREBLE
  0x04  EQ_MODE_CLASSICAL
  0x05  EQ_MODE_ROCK
  0x06  EQ_MODE_JAZZ
  0x07  EQ_MODE_POP
  0x08  EQ_MODE_DANCE
  0x09  EQ_MODE_RNB
  0x0A  EQ_MODE_USER1
  others  reserved

*/
uint8_t  IS2020::eqModeSetting() {
  IS2020::getNextEventFromBt();
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  DSP_NR_CTRL 0x1D  cmd_type

  Description:  It is used to set the noise reduction for voice link.

  Command Parameters:
  cmd_type  SIZE: 1 BYTE
  Value Parameter Description
  0x18  ENABLE_Mic_NR
  0x19  DISABLE_Mic_NR
  0x1b  ENABLE_SPK_NR
  0x1c  DISABLE_SPK_NR
  others  reserved

*/
#define ENABLE_MIC_NR 0x18
#define DISABLE_MIC_NR 0x19
#define ENABLE_SPK_NR 0x1b
#define DISABLE_SPK_NR 0x1c

uint8_t  IS2020::dspNrCtrl(uint8_t type) {
  IS2020::getNextEventFromBt();
  IS2020::sendPacketInt(CMD_DSP_NR_CTRL,type);
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  GPIO_Control  0x1E  "IO_Ctrl_Mask_P0,
  IO_Ctrl_Mask_P1,
  IO_Ctrl_Mask_P2,
  IO_Ctrl_Mask_P3,
  IO_Setting_P0,
  IO_Setting_P1,
  IO_Setting_P2,
  IO_Setting_P3,
  Output_Value_P0,
  Output_Value_P1,
  Output_Value_P2,
  Output_Value_P3," Report_Input_Signal_Level(0x27) , Note:if any IO be set to input mode, this event shall be send under two condition, first condition is when BTM receive input setting command , the second condition is when input IO state change,

  Description:  It is used to control the specific GPIOs as input level detecton or output level drive. For input level detection configuration, BTM will report input signal to MCU when input signal level was changed.

  Command Parameters:
  IO_Ctrl_Mask_P0 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  "Bit mask of P0 for IO control setting.
  Bit 0: P0_0 IO control setting mask.
  Bit 1: P0_1 IO control setting mask.
  ……"
  IO_Ctrl_Mask_P1 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  "Bit mask of P1 for IO control setting.
  Bit 0: P1_0 IO control setting mask.
  Bit 1: P1_1 IO control setting mask.
  ……"
  IO_Ctrl_Mask_P2 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  "Bit mask of P2 for IO control setting.
  Bit 0: P2_0 IO control setting mask.
  Bit 1: P2_1 IO control setting mask.
  ……"
  IO_Ctrl_Mask_P3 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  "Bit mask of P3 for IO control setting.
  Bit 0: P3_0 IO control setting mask.
  Bit 1: P3_1 IO control setting mask.
  ……"

  IO_Setting_P0 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  "setting P0 GPIO as iuput or output mode
  IO bit setting of P0 for input or output configuration. 0: input. 1: output
  Bit 0: P0_0 IO control setting.
  Bit 1: P0_1 IO control setting.
  ……"
  IO_Setting_P1 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  "setting P1 GPIO pin as iuput or output mode
  IO bit setting of P0 for input or output configuration. 0: input. 1: output
  Bit 0: P1_0 IO control setting.
  Bit 1: P1_1 IO control setting.
  ……"
  IO_Setting_P2 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  "setting P2 GPIO pin as iuput or output mode
  IO bit setting of P0 for input or output configuration. 0: input. 1: output
  Bit 0: P2_0 IO control setting.
  Bit 1: P2_1 IO control setting.
  ……"
  IO_Setting_P3 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  "setting P3 GPIO pin as iuput or output mode
  IO bit setting of P0 for input or output configuration. 0: input. 1: output
  Bit 0: P3_0 IO control setting.
  Bit 1: P3_1 IO control setting.
  ……"

  Output_value_P0 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  Output level of P0.x GPIOs setting. It is used for IO_Setting_P0.x as output only.
  Output_value_P1 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  Output level of P1.x GPIOs setting. It is used for IO_Setting_P1.x as output only.
  Output_value_P2 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  Output level of P2.x GPIOs setting. It is used for IO_Setting_P2.x as output only.
  Output_value_P3 SIZE: 1 BYTE
  Value Parameter Description
  0bXXXXXXXX  Output level of P3.x GPIOs setting. It is used for IO_Setting_P3.x as output only.

*/
uint8_t  IS2020::gpioControl() {
  IS2020::getNextEventFromBt();
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  MCU_UART_Rx_Buffer_Size 0x1F  MCU_UART_rx_buffer_size,

  Description:  Host MCU indicate the BTM about the max UART Rx buffer size. The default value is 256 Bytes.

  Command Parameters:
  UART_rx_buffer_size SIZE: 2 BYTE
  Value Parameter Description
  0xXXXX  "The max UART Rx buffer size of Host MCU.
  "

*/
uint8_t  IS2020::mcuUartRxBufferSize(uint8_t buffer) {
  IS2020::getNextEventFromBt();
  //arduino use SERIAL_RX_BUFFER_SIZE define, so lets try use it! hope arduino.h is compiled 1st before this file
  IS2020::sendPacketInt(CMD_MCU_UART_Rx_Buffer_Size,buffer);
  Serial.println("Setting buffer of MCU to "+String(buffer,DEC));
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:   Command Command ID  Command Parameters                                    Return Event
  Voice_Prompt_CMD  0x20                cmd_type, parameter, voice_data                       EVT_Report_Voice_Prompt_Status "BTM shall return the event,REPORT_TTS_STATUS, when received the cmd_type is 0x00"

  Description:  "This command is used to control BTM voice prompt function.
  Host MCU shall set voice prompt control parameter firstly.
  And then send voice prompt data after received REPORT_TTS_STATUS with ready status."

  Command Parameters:
  cmd_type  SIZE: 1 BYTE
  Value Parameter Description
  0x00  Voice prompt Setting.
  0x01  Voice prompt data
  others  reserved

  parameter SIZE: 1 BYTE
  for cmd_type: 0x00
  Value Parameter Description
  0x00  stop to play voice prompt and skip the before voice data.
  0x01  initial, high priority:force to stop current tone and clean tone queue then play the new tone
  0x02  initial, low priority: just put the new tone into tone queue
  others  reserved
  for cmd_type: 0x01
  Value Parameter Description
  0x00  single packet, if voice_data size less than 480
  0x01  fragmented start packet
  0x02  fragmented continue packet
  0x03  fragmented end packet
  others  reserved

  voice_data  SIZE: N BYTES
  for cmd_type: 0x00
  Value Parameter Description
  0xXXXX  invalid
  for cmd_type: 0x01
  Value Parameter Description
  0xXXXX  the data just for voice prompt

*/
uint8_t  IS2020::voicePromptCmd() {
  IS2020::getNextEventFromBt();
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID  Command Parameters  Return Event
  MAP_REQUEST 0x21  "type,
  payload structure"

  Description:  This command is used to send the MAP Requeset to remote BT devices.

  Command Parameters: type  SIZE: 1 BYTE
  Value Parameter Description
  0x00  reserved
  0x01  reserved
  0x02  SetFolder_Request
  0x03  GetFolderList_Request
  0x04  GetMessage_Request
  0x05  GetMessageList_Request
  0x06  SetMessageStatus_Request
  0x07  SetNotification_Request
  0x08  Abort Request

  payload structure SIZE: Varialbe Bytes
  type  Structure Description
  SetFolder_Request SetFolder_Requeset structure
  GetFolderList_Request GetFolderList_Request structure
  GetMessage_Request  GetMessage_Request structure
  GetMessageList_Request  GetMessageList_Request structure
  SetMessageStatus_Request  SetMessageStatus_Request structure
  SetNotification_Request SetNotification_Request structure

*/
uint8_t  IS2020::mapRequest() {
  IS2020::getNextEventFromBt();
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:  Command Command ID   Command Parameters        Return Event
  MAP_REQUEST      0x21                 type,payload structure

  Description:  This command is used to send the MAP Requeset to remote BT devices.

  Command Parameters: type  SIZE: 1 BYTE
  Value Parameter Description
  0x00  reserved
  0x01  reserved
  0x02  SetFolder_Request
  0x03  GetFolderList_Request
  0x04  GetMessage_Request
  0x05  GetMessageList_Request
  0x06  SetMessageStatus_Request
  0x07  SetNotification_Request
  0x08  Abort Request

  payload structure SIZE: Varialbe Bytes
  type  Structure Description
  SetFolder_Request SetFolder_Requeset structure
  GetFolderList_Request GetFolderList_Request structure
  GetMessage_Request  GetMessage_Request structure
  GetMessageList_Request  GetMessageList_Request structure
  SetMessageStatus_Request  SetMessageStatus_Request structure
  SetNotification_Request SetNotification_Request structure

*/
uint8_t  IS2020::securityBondingReq() {
  IS2020::getNextEventFromBt();
  return checkResponce(EVT_Command_ACK);
}
/*
  Command Format:   Command Command ID  Command Parameters                                        Return Event
  Set_Overall_Gain  0x23                database_index, mask,type,HF_gain,A2DP_gain,LineIn_gain

  Description:  This command is used to set overall gain that include hf, a2dp and line_in

  Command Parameters:
  database_index  SIZE: 1 BYTE
  Value Parameter Description
  0x00  database 0
  0x01  database 1

  mask  SIZE: 1 BYTE
    Parameter Description
  0xXX  "set '1' to indicate the gain need to change.
  bit0:A2DP gain
  bit1:HF gain
  bit2:Line_In gain"

  type  SIZE: 1 BYTE
    Parameter Description
  0xXX  "0x01: volume up
  0x02: volume down
  0x03: set absolute gain"

  A2DP_gain SIZE: 1 BYTE
    Parameter Description
  0xXX  0x00~0x0F

  HF_gain SIZE: 1 BYTE
    Parameter Description
  0xXX  0x00~0x0F

  Line_In_gain  SIZE: 1 BYTE
    Parameter Description
  0xXX  0x00~0x0F

*/
uint8_t  IS2020::setOverallGain() {
  IS2020::getNextEventFromBt();
  return checkResponce(EVT_Command_ACK);
}

void IS2020::decodeCommand(uint8_t cmd){
    switch(cmd){
      case CMD_Make_Call:
        DBG(F("Make Call"));
      break;
      case CMD_Make_Extension_Call:
        DBG(F("Make Extension Call"));
      break;
      case CMD_MMI_Action:
        DBG(F("MMI Action"));
      break;
      case CMD_Event_Mask_Setting:
        DBG(F("Event Mask Setting"));
      break;
      case CMD_Music_Control:
        DBG(F("Music Control"));
      break;
      case CMD_Change_Device_Name:
        DBG(F("Change Device Name"));
      break;
      case CMD_Change_PIN_Code:
        DBG(F("Change PIN Code"));
      break;
      case CMD_BTM_Parameter_Setting:
        DBG(F("BTM_Parameter_Setting"));
      break;
      case CMD_Read_BTM_Version:
        DBG(F("Read_BTM_Version"));
      break;
      case CMD_Get_PB_By_AT_Cmd:
        DBG(F("Get PB By AT Cmd"));
      break;
      case CMD_Vendor_AT_Command:
        DBG(F("Vendor AT Command"));
      break;
      case CMD_AVRCP_Specific_Cmd:
        DBG(F("AVRCP Specific Cmd"));
      break;
      case CMD_AVRCP_Group_Navigation:
        DBG(F("AVRCP Group Navigation"));
      break;
      case CMD_Read_Link_Status:
        DBG(F("Read Link Status"));
      break;
      case CMD_Read_Paired_Device_Record:
        DBG(F("Read Paired Device Record"));
      break;
      case CMD_Read_Local_BT_Address:
        DBG(F("Read Local BT Address"));
      break;
      case CMD_Read_Local_Device_Name:
        DBG(F("Read Local Device Name"));
      break;
      case CMD_Set_Access_PB_Method:
        DBG(F("Set Access PB Method"));
      break;
      case CMD_Send_SPP_iAP_Data:
        DBG(F("Send SPP iAP Data"));
      break;
      case CMD_BTM_Utility_Function:
        DBG(F("BTM Utility Function"));
      break;
      case CMD_Event_ACK:
        DBG(F("Event ACK"));
      break;
      case CMD_Additional_Profiles_Link_Setup:
        DBG(F("Additional Profiles Link Setup"));
      break;
      case CMD_Read_Linked_Device_Information:
        DBG(F("Read Linked Device Information"));
      break;
      case CMD_Profile_Link_Back:
        DBG(F("Profile Link Back"));
      break;
      case CMD_Disconnect:
        DBG(F("Disconnect"));
      break;
      case CMD_MCU_Status_Indication:
        DBG(F("MCU Status Indication"));
      break;
      case CMD_User_Confirm_SPP_Req_Reply:
        DBG(F("User Confirm SPP Req Reply"));
      break;
      case CMD_Set_HF_Gain_Level:
        DBG(F("Set HF Gain Level"));
      break;
      case CMD_EQ_Mode_Setting:
        DBG(F("EQ Mode Setting"));
      break;
      case CMD_DSP_NR_CTRL:
        DBG(F("DSP NR CTRL"));
      break;
      case CMD_GPIO_Control:
        DBG(F("GPIO Control"));
      break;
      case CMD_MCU_UART_Rx_Buffer_Size:
        DBG(F("MCU UART Rx Buffer Size"));
      break;
      case CMD_Voice_Prompt_Cmd:
        DBG(F("Voice Prompt Cmd"));
      break;
      case CMD_MAP_REQUEST:
        DBG(F("MAP REQUEST"));
      break;
      case CMD_Security_Bonding_Req:
        DBG(F("Security Bonding Req"));
      break;
      case CMD_Set_Overall_Gain:
        DBG(F("Set Overall Gain"));
      break;
      default:
        DBG("Unknown CMD command "+String(cmd,HEX));
      break;
    }
  }

uint8_t IS2020::queryDeviceName(uint8_t deviceId){
  IS2020::getNextEventFromBt();
  IS2020::readLinkedDeviceInformation(deviceId,CRLDI_query_device_name);
  return checkResponce(EVT_Command_ACK);
}

uint8_t IS2020::queryInBandRingtoneStatus(uint8_t deviceId){
  IS2020::getNextEventFromBt();
  IS2020::readLinkedDeviceInformation(deviceId,CRLDI_query_in_band_ringtone_status);
  return checkResponce(EVT_Command_ACK);
}

uint8_t IS2020::queryIfRemoteDeviceIsIapDevice(uint8_t deviceId){
  IS2020::getNextEventFromBt();
  IS2020::readLinkedDeviceInformation(deviceId,CRLDI_query_if_remote_device_is_iAP_device);
  return checkResponce(EVT_Command_ACK);
}

uint8_t IS2020::queryIfRemoteDeviceSupportAvrcpV13(uint8_t deviceId){
  IS2020::getNextEventFromBt();
  IS2020::readLinkedDeviceInformation(deviceId,CRLDI_query_if_remote_device_support_AVRCP_v13);
  return checkResponce(EVT_Command_ACK);
}

uint8_t IS2020::queryHfA2dpGain(uint8_t deviceId){
  IS2020::getNextEventFromBt();
  IS2020::readLinkedDeviceInformation(deviceId,CRLDI_query_HF_A2DP_gain);
  return checkResponce(EVT_Command_ACK);
}

uint8_t IS2020::queryLineInGain(uint8_t deviceId){
  IS2020::getNextEventFromBt();
  IS2020::readLinkedDeviceInformation(deviceId,CRLDI_query_Line_In_gain);
  return checkResponce(EVT_Command_ACK);
}
