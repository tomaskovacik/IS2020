#include "events.h"

// thx to vincent for moving meta data support forward, check his RTOS version:
// https://github.com/VincentGijsen/MelbusRtos
/*
  was thinking using SErial event here, but based on comment in code serialEvent is called after loop() so calling function  in loop is the same
*/
uint8_t IS2020::getNextEventFromBt() {
  //delay(100);
  if (btSerial -> available() > 3) { //min is 3, 0x55 + packet size, then we can read in while loop
    //if (DEBUG_EVENTS) DBG_EVENTS(F("Get next Event from BT\n"));
    if (btSerial -> read() == 0xAA) { //start of event

      uint16_t packetSize = (btSerial -> read() << 8) | (btSerial -> read() & 0xff);

      uint8_t event[packetSize + 1]; //data+cksum
      /*
            for (uint8_t i = 0; i < packetSize + 1; i++) {
              event[i] = btSerial -> read();
            }
      */
      //if (DEBUG_EVENTS) DBG_EVENTS(F("Packet size: "));DBG_EVENTS(String(packetSize,DEC));if (DEBUG_EVENTS) DBG_EVENTS(F("\n"));
      uint16_t i = 0;
      while (i < packetSize + 1 ) {
        //read only if buffer has some data
        if (btSerial -> available()) {
          event[i++] = btSerial -> read();
        }

      }
      if (checkCkeckSum(packetSize, event)) {

        switch (event[0]) {
          /*
             Event Format: Event       Event Code  Event Parameters
                           Command ACK 0x00        cmd_id, status

             Description:

             Event Parameters:
                cmd_id       SIZE: 1 BYTE

                Value Parameter Description
                0xXX  the command id to ack

            status  SIZE: 1 BYTE
            Value Parameter Description
            0x00  command complete : BTM can handle this command.
            0x01  command disallow : BTM can not handle this command.
            0x02  unknow command
            0x03  parameters error
            0x04  "BTM is busy:
            This status is used for SPP data cannot be sent out in this moment because of ACL Tx buffer or RFCOMM credit issue. BTM will reply the ""Complete"" status once the SPP data can be processed.
            "
            0x05  "BTM memory is full:
            This status is used for SPP data cannot be sent out in this moment because of os heap memory is full. BTM will reply the ""Complete"" status once the SPP data can be processed. "
            others  RFD

          */
          case EVT_Command_ACK:
            {
              switch (event[2]) {
                case 0x00:
                  if (DEBUG_EVENTS) DBG_EVENTS(F("Command complete"));// : BTM can handle this command.");
                  break;
                case 0x01:
                  if (DEBUG_EVENTS) DBG_EVENTS(F("Command disallowed"));// : BTM can not handle this command.");
                  if (event[1] == CMD_Vendor_AT_Command)  allowedSendATcommands=1;
                  break;
                case 0x02:
                  if (DEBUG_EVENTS) DBG_EVENTS(F("Unknow command"));
                  break;
                case 0x03:
                  if (DEBUG_EVENTS) DBG_EVENTS(F("Parameters error"));
                  break;
                case 0x04:
                  if (DEBUG_EVENTS) DBG_EVENTS(F("BTM is busy"));
                  break;
                case 0x05:
                  if (DEBUG_EVENTS) DBG_EVENTS(F("BTM memory is full"));
                  break;
              }
              if (DEBUG_EVENTS) DBG_EVENTS(F(": "));
              decodeCommandInEvents(event[1]);
              //DBG_EVENTS(" "+String(event[1], HEX));
              if (DEBUG_EVENTS) DBG_EVENTS(F("\n"));
            }
            break;
          /*
            Event Format:  Event Event Code  Event Parameters
            BTM_Status  0x01  state, link_info

            Description:  This event is used to indicate about the BTM status. BTM will send it event while the status was changed.

            Event Parameters: state SIZE: 1 BYTE
            Value Parameter Description
            0x00  Power OFF state
            0x01  pairing state (discoverable mode)
            0x02  Power ON state
            0x03  pairing successful
            0x04  pairing fail
            0x05  HF link established
            0x06  A2DP link established
            0x07  HF link disconnected
            0x08  A2DP link disconnected
            0x09  SCO link connected
            0x0A  SCO link disconnected
            0x0B  AVRCP link established
            0x0C  AVRCP link disconnected
            0x0D  Standard SPP connected
            0x0E  Standard_SPP / iAP disconnected
            0x0F  Standby state
            0x10  iAP connected
            0x11  ACL disconnected

            link_info SIZE: 1 BYTE
            state:0x04
            Value Parameter Description
            0xXX  "For pairing not complete(0x04) case, this parameter indicate not completed reason
            0: time out
            1: fail

            "
            state:0x05,0x06,0x0B,0x0D
            Value Parameter Description
            0xXX  "this parameter indicate both link device and data base information.
            The format is shown as below:
            Bit7~4 : linked device id(0~7)
            Bit3~0 : linked data base(0 or 1)

            "
            state:0x07~0x08,0x0C,0x0E
            Value Parameter Description
            0xXX  "his parameter show the linked_data_base (0 or 1)


            "
            state:0x11
            Value Parameter Description
            0xXX  "0:disconnection
            1:link loss


            "

          */
          case EVT_BTM_Status:
            {
              //IS2020::btmStatusChanged = 1;
              IS2020::btmState = event[1];
              IS2020::btmLinkInfo = event[2];
              switch (btmState) {
                case BTM_STATE_power_OFF:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("Power OFF."));
                  }
                  break;
                case BTM_STATE_discoverable_mode:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("Discoverable mode."));
                  }
                  break;
                case BTM_STATE_power_ON_state:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("Power ON."));
                  }
                  break;
                case BTM_STATE_pairing_successful:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("Pairing successful."));
                  }
                  break;
                case BTM_STATE_pairing_fail:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("Pairing failed."));
                    //                    Serial.println("Pairing failed, reason: " + event[2]?"timeout":"fail");
                    if (pairingFailReason != event[2]) pairingFailReason = event[2];
                  }
                  break;
                case BTM_STATE_HF_link_established:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("HF link established."));
                  }
                  break;
                case BTM_STATE_A2DP_link_established:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("A2DP link established."));

                  }
                  break;
                case BTM_STATE_HF_link_disconnected:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("HF link disconnected."));
                  }
                  break;
                case BTM_STATE_A2DP_link_disconnected:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("AD2P link disconnected."));
                  }
                  break;
                case BTM_STATE_SCO_link_connected:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("SCO link connected."));
                  }
                  break;
                case BTM_STATE_SCO_link_disconnected:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("SCO link disconnected."));
                  }
                  break;
                case BTM_STATE_AVRCP_link_established:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("AVRCP link established."));
                  }
                  break;
                case BTM_STATE_AVRCP_link_disconnected:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("AVRCP link disconnected."));
                  }
                  break;
                case BTM_STATE_SPP_connected:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("SSP link connected."));
                  }
                  break;
                case BTM_STATE_SPP_iAP_disconnected:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("SCO iAP link connected."));
                  }
                  break;
                case BTM_STATE_standby:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("BTM standby."));
                  }
                  break;
                case BTM_STATE_iAP_connected:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("iAP connected."));
                  }
                  break;
                case BTM_STATE_ACL_disconnected:
                  {
                    if (DEBUG_EVENTS) DBG_EVENTS(F("ACL disconnected."));
                  }
                  break;
              }
              if (DEBUG_EVENTS) DBG_EVENTS(F("\n"));
              IS2020::readLinkStatus(); //read info about paired devices
              IS2020::btmStatusChanged = 1;
            }
            break;
          /*
            Event Format:  Event Event Code  Event Parameters
            Call_Status 0x02  data_base_index, call_status

            Description:  This event is used to indicate about the HF call status of BTM.

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            call_status SIZE: 1 BYTE
            Value Parameter Description
            0x00  Idle
            0x01  voice dial
            0x02  incoming call
            0x03  outgoing call
            0x04  call active
            0x05  a call active with a call waiting
            0x06  a call active with a call hold

          */
          case EVT_Call_Status:
            {
              IS2020::btmStatusChanged = 1;
              IS2020::callStatus[event[1]] = event[2];
            }
            break;
          /*
            Event Format:  Event Event Code  Event Parameters
            Caller_ID 0x03  data_base_index, num

            Description:  This event is used to indicate about the caller ID of the incoming call.

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            num SIZE: N OCTETS
            Value Parameter Description
            0xXX..  caller Id or phone number

          */
          case EVT_Caller_ID:
            {
              IS2020::btmStatusChanged = 1;
              for (uint8_t i = 0; i < packetSize; i++) {
                IS2020::callerId[event[1]][packetSize] = event[i + 2]; //no need to any sort of flag bit, cose we have callstatus change flag and in case of incoming call we just read this array
              }
            }
            break;
          /*
            Event Format:  Event Event Code  Event Parameters
            SMS_Received_Indication 0x04  data_base_index, indication

            Description:  This event is used to indicate about the sms status that BTM received from mobile phone.

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            indication  SIZE: 1 BYTE
            Value Parameter Description
            0x00  no new sms received
            0x01  new sms received

          */
          case EVT_SMS_Received_Indication:
            {
              IS2020::btmStatusChanged = 1;
              IS2020::smsStatus[event[1]] = event[2];
            }
            break;
          /*
            Event Format:  Event Event Code  Event Parameters
            Missed_Call_Indication  0x05  data_base_index, information

            Description:  This event is used to indicate about the missed call indication that BTM received from mobile phone

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            information SIZE: 1 BYTE
            Value Parameter Description
            0xXX  reserved

          */
          case EVT_Missed_Call_Indication:
            {
              IS2020::btmStatusChanged = 1;
              IS2020::missedCallStatus[event[1]] = event[2];
            }
            break;
          /*
            Event Format:  Event Event Code  Event Parameters
            Phone_Max_Battery_Level 0x06  data_base_index, battery_level

            Description:  This event is used to indicate about the mobile phone max battery level that BTM received from mobile phone.

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            battery_level SIZE: 1 BYTE
            Value Parameter Description
            0xXX  max battery level

          */
          case EVT_Phone_Max_Battery_Level:
            {
              IS2020::btmStatusChanged = 1;
              IS2020::maxBatteryLevel[event[1]] = event[2];
            }
            break;
          /*
            Event Format:  Event Event Code  Event Parameters
            Phone_Current_Battery_Level 0x07  data_base_index, battery_level

            Description:  This event is used to indicate about the mobile phone current battery level that BTM received from mobile phone.

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            battery_level SIZE: 1 BYTE
            Value Parameter Description
            0xXX  current battery level

          */
          case EVT_Phone_Current_Battery_Level:
            {
              IS2020::btmStatusChanged = 1;
              IS2020::currentBatteryLevel[event[1]] = event[2];
            }
            break;
          /*
            Event Format:  Event Event Code  Event Parameters
            Roaming_Status  0x08  data_base_index, status

            Description:  This event is used to indicate about the roaming status that BTM received from mobile phone.

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            status  SIZE: 1 BYTE
            Value Parameter Description
            0x00  non-roaming
            0x01  roaming

          */
          case EVT_Roaming_Status:
            {
              IS2020::btmStatusChanged = 1;
              IS2020::roamingStatus[event[1]] = event[2];
            }
            break;
          /*
            Event Format: Event Event Code  Event Parameters
            Phone_Max_Signal_Strength_Level 0x09  data_base_index, signal_level

            Description:  This event is used to indicate about the max signal strength level that BTM received from mobile phone.

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            signal_level  SIZE: 1 BYTE
            Value Parameter Description
            0xXX  phone's max signal strength level

          */
          case EVT_Phone_Max_Signal_Strength_Level:
            {
              IS2020::btmStatusChanged = 1;
              IS2020::maxSignalLevel[event[1]] = event[2];
            }
            break;
          /*
            Event Format: Event Event Code  Event Parameters
            Phone_Current_Signal_Strength_Level 0x0A  data_base_index, signal_strength

            Description:  This event is used to indicate about the signal strength level that BTM received from mobile phone.

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            This event is used to indicate the MCU about the current signal strength level that BTM received from mobile phone. SIZE: 1 BYTE
            Value Parameter Description
            0xXX  phone's current signal strength level

          */
          case EVT_Phone_Current_Signal_Strength_Level:
            {
              IS2020::btmStatusChanged = 1;
              IS2020::currentSignalLevel[event[1]] = event[2];
            }
            break;
          /*
            Event Format: Event Event Code  Event Parameters
            Phone_Service_Status  0x0B  data_base_index, service

            Description:  This event is used to indicate about the service status that BTM received from mobile phone.

            Event Parameters: data_base_index SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            service SIZE: 1 BYTE
            Value Parameter Description
            0x00  No service available
            0x01  service available

          */
          case EVT_Phone_Service_Status :
            {
              IS2020::btmStatusChanged = 1;
              IS2020::serviceStatus[event[1]] = event[2];
            }
            break;
          /*
            Event Format: Event               Event Code  Event Parameters
                          BTM_Battery_Status  0x0C        battery_status, voltage_level

            Description:  This event is used to indicate about the BTM's battery status.

            Event Parameters: battery_status  SIZE: 1 BYTE
            Value Parameter Description
            0x00  dangerous level, and will auto shutdown
            0x01  low level
            0x02  normal level
            0x03  high level
            0x04  full level
            0x05  in charging
            0x06  charging complete

            voltage_level SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "0x00: 3.0V
            0x01: 3.1V
            0x02: 3.2V
            0x03: 3.3V
            0x04: 3.4V
            0x05: 3.5V
            0x06: 3.6V
            0x07: 3.7V
            0x08: 3.8V
            0x09: 3.9V
            0x0A: 4.0V
            0x0B: 4.1V
            0x0C: 4.2V"

          */
          case EVT_BTM_Battery_Status :
            {
              IS2020::btmStatusChanged = 1;
              IS2020::btmBatteryStatus[0] = event[1];
              IS2020::btmBatteryStatus[1] = event[2];
            }
            break;
          /*
            Event Format: Event Event Code  Event Parameters
            BTM_Charging_Status 0x0D  charger_status, dummy

            Description:

            Event Parameters: charger_status  SIZE: 1 BYTE
            Value Parameter Description
            0x00  No charger plug in
            0x01  in charging
            0x02  charging complete
            0x03  charging fail

            dummy SIZE: 1 BYTE
            Value Parameter Description
            0xXX  reserved

          */
          case EVT_BTM_Charging_Status:
            {
              IS2020::btmChargingStatus = event[1];
            }
            break;
          /*
            Event Format: Event Event Code  Event Parameters
            Reset_To_Default  0x0E  dummy1,dummy2

            Description:  This event is used to indicate the BTM finish the Master Reset command for the MMI command (0x56) trigger.

            Event Parameters: dummy1  SIZE: 1 BYTE
            Value Parameter Description
            0x00

            dummy2  SIZE: 1 BYTE
            Value Parameter Description
            0x00

          */
          case EVT_Reset_To_Default:
            {
            }
            break;
          /*
            Event Format: Event Event Code  Event Parameters
            Report_HF_Gain_Level  0x0F  database_index, level

            Description:  This is used to report the HF gain level set by remote Audio Gateway (Phone)

            Event Parameters:
            database_index  SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate HF/HS device
            0x01  database 1 for a dedicate HF/HS device

            level     SIZE: 1 BYTE
            Value Parameter Description
            0x00-0x0F gain level that sync. with HF device

          */
          case EVT_Report_HF_Gain_Level:
            {
              IS2020::hfGainLevel[event[1]] = event[2];
            }
            break;
          /*
            Event Format: Event Event Code  Event Parameters
            EQ_Mode_Indication  0x10  mode, dummy

            Description:  This event is used to notice the EQ_mode setting by MMI or EQ_Mode_Setting command

            Event Parameters: mode  SIZE: 1 BYTE
            Value Parameter Description
            0x00  Off Mode
            0x01  Soft Mode
            0x02  Bass Mode
            0x03  Treble Mode
            0x04  Classical Mode
            0x05  Rock Mode
            0x06  Jazz Mode
            0x07  Pop Mode
            0x08  Dance Mode
            0x09  R&B Mode
            0x0A  User Mode 1

            dummy SIZE: 1 BYTE
            Value Parameter Description
            0xXX  reserved

          */
          case EVT_EQ_Mode_Indication:
            {
              IS2020::eqMode = event[1];
            }
            break;
          case EVT_PBAP_Missed_Call_History:
            {
            }
            break;
          case EVT_PBAP_Received_Call_History:
            {
            }
            break;
          case EVT_PBAP_Dialed_Call_History:
            {
            }
            break;
          case EVT_PBAP_Combine_Call_History:
            {
            }
            break;
          /*
            Event Format:	Event	Event Code	Event Parameters
            Get_PB_By_AT_Cmd_Reply	0x15	contact

            Description:

            Event Parameters:	contact	SIZE: N BYTE
            Value	Parameter Description
             0x…	AT Command format phone book contacts
          */
          case EVT_Get_PB_By_AT_Cmd_Reply:
            {
            }
            break;
          /*
            Event Format: Event Event Code  Event Parameters
            PBAP_Access_Finish  0x16  type, reserved

            Description:

            Event Parameters: type  SIZE: 1 BYTE
            Value Parameter Description
            0x0B  Retrieve miss call history records finish
            0x0C  Retrieve answered call history records finish
            0x0D  Retrieve dialed call history records finish
            0x0E  Retrieve combined call(missed, dialed,answered) history records finish
            0x0F  Retrieve phone book contacts finish


            reserved  SIZE: 1 BYTE
            Value Parameter Description
            0xXX  reserved

          */
          case EVT_PBAP_Access_Finish:
            {
              IS2020::pbapAccessFinish = event[1];
            }
            break;
          /*
            Event Format: Event Event Code  Event Parameters
            Read_Linked_Device_Information_Reply  0x17  database_index, type, info

            Description:  This event is used to reply Read_Linked_Device_Information command.

            Event Parameters: database_index  SIZE: 1 BYTE
            Value Parameter Description
            0x00  database 0 for a dedicate link
            0x01  database 1 for a dedicate link

            type  SIZE: 1 BYTE
            Value Parameter Description
            0x00  reply device name
            0x01  reply in-band ringtone status
            0x02  reply if remote device is a iAP device
            0x03  reply if remote device support AVRCP v1.3
            0x04  reply HF&A2DP gain
            0x05  reply Line_In gain
            others  reserved

            info (for device name)  SIZE: N OCTETS
            Value Parameter Description
            0xXX… "N bytes bluetooth name with NULL terminated. (N <= 249 with NULL terminated)

            Note:If remote device response empty name string, then BTM will report name with data NULL terminate(0x00) only."

            info (for in-band ringtone status)  SIZE: 1 BYTE
            Value Parameter Description
            0x00  disable
            0x01  enable

            info (for spp type : iAP device or not) SIZE: 1 BYTE
            Value Parameter Description
            0x00  Standard SPP device
            0x01  iAP device

            info (for AVRCP type : AVRCP v1.3 device or not)  SIZE: 1 BYTE
            Value Parameter Description
            0x00  not support AVRCP v1.3
            0x01  support AVRCP v1.3

            info (for type 0x04)  SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "bit[3:0]: A2DP gain
            bit[7:4]: HF gain"

            info (for type 0x05)  SIZE: 1 BYTE
            Value Parameter Description
            0xXX  Line_In gain

          */
          case EVT_Read_Linked_Device_Information_Reply:
            {
              //if (DEBUG_EVENTS) DBG_EVENTS(F("Read_Linked_Device_Information_Reply\n"));
              IS2020::btmStatusChanged = 1;
              switch (event[2]) { //event[1] is device id
                case 0x00://reply device name
                  {
                    IS2020::deviceName[event[1]] = ""; //clear stored named
                    //if (DEBUG_EVENTS) DBG_EVENTS(F("Reply device name\n"));
                    //N bytes bluetooth name with NULL terminated. (N <= 249 with NULL terminated)

                    for (uint8_t i = 3; i < /*DEVICENAME_LENGHT_SUPPORT + 2*/packetSize - 1; i++) { //event[2] is information type definition, last is checksum
                      //check if data are not NULL
                      if (event[i] == 0x00) break;
                      //deviceName[event[1]][i - 3] = event[i];
                      //Serial3.write(event[i]);
                      IS2020::deviceName[event[1]] = IS2020::deviceName[event[1]] + (char)event[i]; //string append

                    }
                  }
                  break;
                case 0x01://reply in-band ringtone status
                  /*
                    info (for in-band ringtone status) SIZE: 1 BYTE
                    Value Parameter Description
                    0x00  disable
                    0x01  enable
                  */
                  IS2020::deviceInBandRingtone[event[1]] = event[3];
                  break;
                case 0x02://reply if remote device is a iAP device
                  /*
                    info (for spp type : iAP device or not) SIZE: 1 BYTE
                    Value Parameter Description
                    0x00  Standard SPP device
                    0x01  iAP device
                  */
                  IS2020::deviceIsIap[event[1]] = event[3];
                  break;
                case 0x03://reply if remote device support AVRCP v1.3
                  IS2020::deviceSupportAvrcpV13[event[1]] = event[3];
                  break;
                case 0x04://reply HF&A2DP gain
                  /*
                    info (for type 0x04) SIZE: 1 BYTE
                    Value Parameter Description
                    0xXX  "bit[3:0]: A2DP gain
                    bit[7:4]: HF gain"
                  */
                  IS2020::deviceHfAndA2dpGain[event[1]] = event[3];
                  break;
                case 0x05: //reply Line_In gain
                  /*
                    info (for type 0x05)  SIZE: 1 BYTE
                    Value Parameter Description
                    0xXX  Line_In gain
                  */
                  IS2020::deviceLineInGain[event[1]] = event[3];
                  break;
                default:
                  if (DEBUG) {
                    //Serial3.print("Reserved response: ");
                    //Serial3.println(event[1], HEX);
                  }
              }
            }
            break;
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
          case EVT_Read_BTM_Version_Reply:
            {
              if (event[1] == 0x00) IS2020::btmUartVersion = (event[2] << 8) | (event[3] & 0xff);
              if (event[1] == 0x01) IS2020::btmFwVersion = (event[2] << 8) | (event[3] & 0xff);
            }
            break;
          case EVT_Call_List_Report:
            {
            }
            break;
          case EVT_AVRCP_Specific_Rsp:
            {
              decodeAvrcpPdu(event[8]);
              uint8_t deviceID = event[1];
              //event[2] && 0x0F Ctype
              //event[3] && B11111000 Subunit_type
              //event[3] && B00000111 Subunit_ID
              //event[4] Opcode
              //event[5][6][7] Company ID
              //event[8] PduID
              //event[9] && B11111000 Reserved
              //event[9] && B00000111 Packet Type
              //evemt[10][11] packet Length
              //event[12] event ID


              uint16_t parameter_length =   (event[10] << 8) | (event[11] & 0xff);

              if (event[2] == AVRCP_EVENT_RESPONSE_REJECTED) {
                if (DEBUG_AVRCP) DBG_AVRCP(F("\nError: "));
                IS2020::decodeRejectReason(event[12]);
                IS2020::decodeAvrcpPdu(event[8]);
                if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                return false;
              }
              String tmp; //play/pause etc....
              switch (event[8]) //event[9] is always 0x00 10+11=size of sending responce
              {
                case AVRCP_REGISTER_NOTIFICATION:
                  {
                    if (DEBUG_AVRCP) DBG_AVRCP(F("Response: "));
                    switch (event[2])
                    { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                      case AVRCP_EVENT_RESPONSE_STABLE:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("STABLE: "));
                        break;
                      case AVRCP_EVENT_RESPONSE_REJECTED:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("REJECTED: "));
                        break;
                      case AVRCP_EVENT_RESPONSE_NOTIFY:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("NOTIFY: "));
                        break;
                      case AVRCP_EVENT_RESPONSE_INTERIM:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("INTERIM: "));
                        break;
                      case AVRCP_EVENT_RESPONSE_CHANGED:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("CHANGED: "));
                        break;
                    }
                    IS2020::decodeAvrcpEvent(event[12]);
                    switch (event[12]) //eventID
                    {
                      case AVRCP_EVENT_PLAYBACK_STATUS_CHANGED:
                        {
                          /*
                            Description:
                            Indicates the current status of
                            playback

                            Values:
                            0x00: STOPPED
                            0x01: PLAYING
                            0x02: PAUSED
                            0x03: FWD_SEEK
                            0x04: REV_SEEK
                            0xFF: ERROR
                          */
                          switch (event[2])
                          { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM:
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:
                              {
                                IS2020::musicState[deviceID] = event[13];
                                IS2020::btmStatusChanged = 1;
                                if (DEBUG_AVRCP) {
                                  switch (event[13]) {
                                    case 0x00:
                                      if (DEBUG_AVRCP) DBG_AVRCP(F("STOPPED\n"));
                                      break;
                                    case 0x01:
                                      if (DEBUG_AVRCP) DBG_AVRCP(F("PLAYING\n"));
                                      break;
                                    case 0x02:
                                      if (DEBUG_AVRCP) DBG_AVRCP(F("PAUSED\n"));
                                      break;
                                    case 0x03:
                                      if (DEBUG_AVRCP) DBG_AVRCP(F("FWD_SEEK\n"));
                                      break;
                                    case 0x04:
                                      if (DEBUG_AVRCP) DBG_AVRCP(F("REV_SEEK\n"));
                                      break;
                                    case 0xFF:
                                      if (DEBUG_AVRCP) DBG_AVRCP(F("ERROR\n"));
                                      break;
                                  }
                                }
                                IS2020::avrcpRegNotifyPlaybackStatusChanged(deviceID);
                              }
                              break;
                          }
                        }
                        break;
                      case AVRCP_EVENT_TRACK_CHANGED:
                        {
                          /*Description:
                            Unique Identifier to identify an
                            element on TG, as is used for
                            GetElementAttribute command in
                            case Browsing is not supported
                            and GetItemAttribute command in
                            case Browsing is supported.

                            Values:
                            If a track is selected, then return 0x0
                            in the response. If no track is
                            currently selected, then return
                            0xFFFFFFFFFFFFFFFF in the
                            INTERIM response.

                            If Browsing is not supported and a
                            track is selected, then return 0x0 in
                            the response. If Browsing is
                            supported, then 0x0 is not allowed.

                            If Browsing is supported and a track
                            is selected, the identifier shall
                            correspond to the currently played
                            media element as listed in the
                            NowPlaying folder.
                          */
                          switch (event[2])
                          { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM:
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:
                              { //re-active event
                                IS2020::avrcpRegNotifyTrackChanged(deviceID);
                                IS2020::avrcpRegNotifyTrackPositionChanged(deviceID);
                                //IS2020::avrcpGetElementAttributesAll(event[1]);
                                IS2020::avrcpGetElementAttributes(event[1]);
                              }
                              break;
                          }
                        }
                        break;
                      case AVRCP_EVENT_TRACK_REACHED_END:
                        {
                          switch (event[2])
                          { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM:
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:
                              { //re-active event
                                IS2020::avrcpRegNotifyTrackReachedEnd(deviceID);
                                //IS2020::avrcpGetElementAttributesAll(event[1]);
                              }
                              break;
                          }
                        }
                        break;
                      case AVRCP_EVENT_TRACK_REACHED_START:
                        {
                          switch (event[2])
                          { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM:
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:
                              { //re-active event
                                IS2020::avrcpRegNotifyTrackReachedStart(deviceID);
                                //IS2020::avrcpRegNotifyTrackPositionChanged(deviceID);
                                //IS2020::avrcpGetElementAttributesAll(event[1]);
                              }
                              break;
                          }
                        }
                        break;
                      case AVRCP_EVENT_PLAYBACK_POS_CHANGED:
                        {
                          //if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));

                          switch (event[2]) { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM:
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:
                              {
                                //re-active event
                                IS2020::avrcpRegNotifyTrackPositionChanged(deviceID);
                                uint32_t time = ((uint32_t)event[13] << 24) |  ((uint32_t)event[14] << 16) |  ((uint32_t)event[15] << 8) | event[16];
                                if (time != 0xFFFFFFFF) {
                                  /*
                                     this ^^^^^^^^^^^^^ should be in INTERIM responce only but guess what:
                                     Response: CHANGED: Decoded AVRCP event: AVRCP_EVENT_PLAYBACK_POS_CHANGED
                                     [1a,0,d,48,0,0,19,58,31,0,0,5,5,ff,ff,ff,ff,]
                                     4294967.00s

                                     from specs:

                                     If no track currently selected, then
                                     return 0xFFFFFFFF in the
                                     INTERIM response.

                                  */
                                  /*float f_*/uint16_t time_S = time / 1000;
                                  Serial.println(String(time_S / 60) + ":" + String(time_S % 60));
                                }
                              }
                          }
                        }
                        break;
                      case AVRCP_EVENT_BATT_STATUS_CHANGED:
                        {
                          /*
                            NOTE: Battery status notification defined in this specification is expected to be
                            deprecated in favor of Attribute profile specification in the future.
                          */
                          if (DEBUG_AVRCP) DBG_AVRCP(F("Battery status: "));
                          switch (event[13])
                          {
                              /*
                                0x0 – NORMAL – Battery operation is in normal state
                                0x1 – WARNING - unable to operate soon. Is provided when the battery level is going down.
                                0x2 – CRITICAL – Cannot operate any more. Is provided when the battery level is going down.
                                0x3 – EXTERNAL – Plugged to external power supply
                                0x4 - FULL_CHARGE – when the device is completely charged from the external power supply
                              */
                            case 0x00:
                              if (DEBUG_AVRCP) DBG_AVRCP(F("NORMAL\n"));
                              break;
                            case 0x01:
                              if (DEBUG_AVRCP) DBG_AVRCP(F("WARNING\n"));
                              break;
                            case 0x02:
                              if (DEBUG_AVRCP) DBG_AVRCP(F("CRITICAL\n"));
                              break;
                            case 0x03:
                              if (DEBUG_AVRCP) DBG_AVRCP(F("EXTERNAL\n"));
                              break;
                            case 0x04:
                              if (DEBUG_AVRCP) DBG_AVRCP(F("FULL_CHARGE\n"));
                              break;
                          }

                        }
                        break;
                      case AVRCP_EVENT_SYSTEM_STATUS_CHANGED:
                        {
                          switch (event[2]) { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM:
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:

                              break;
                          }
                        }
                        break;
                      case AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:
                        {
                          switch (event[2]) { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM:
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:
                              uint8_t i = 14;
                              while (i < 14 + 2 * event[13]) {
                                IS2020::decodeAvrcpPlayerAtributes(event[i++], event[i++]);
                              }
                              if (DEBUG_AVRCP) DBG_AVRCP(F("\n"));
                              IS2020::avrcpRegNotifyPlayerAppSettingsChanged(deviceID);
                              break;
                          }
                        }
                        break;
                      case AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED:
                        /*6.9.5
                          Notify Now Playing Content Changed
                          The Now Playing Content Changed notification allows a CT to be informed when the content of the
                          NowPlaying folder for the Addressed Player is changed. The notification should not be completed if only
                          the track has changed or the order of the tracks on the now playing list has changed.
                          This is an event which may be used for the Register Notification command described in Section 6.7.2,
                          which is a vendor dependent AV/C Notify. The interim and final responses to the notify shall contain no
                          parameters.
                          An example PDU for this command is given in Section 25.14.
                          Note that to retrieve the content of the NowPlaying folder, the NowPlaying folder can be browsed (see
                          Section 6.10.4.2). If the NowPlaying folder is browsed as reaction to the
                          EVENT_NOW_PLAYING_CONTENT_CHANGED, the CT should register the
                          EVENT_NOW_PLAYING_CONTENT_CHANGED again before browsing the NowPlaying folder in order
                          to get informed about intermediate changes in that folder.
                        */
                        {
                          switch (event[2]) { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM:
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:
                              if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                              IS2020::avrcpRegNotifyNowPlayingContentChanged(deviceID);
                              break;
                          }
                        }
                        break;
                      case AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED:
                        {
                          uint16_t playerID = event[13] << 8 | event[14];
                          switch (event[2]) { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM://currently playing playerid:
                              IS2020::avrcpListPlayerAttributes(deviceID);
                              IS2020::avrcpStorePlayerID(playerID);
                              //IS2020::avrcpSetBrowsedPlayer(deviceID, playerID);
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:
                              { //re-active event
                                //if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                                IS2020::avrcpRegNotifyAddressedPlayerChanged(deviceID);
                                IS2020::avrcpListPlayerAttributes(deviceID);
                                IS2020::avrcpStorePlayerID(playerID);
                                //IS2020::avrcpSetBrowsedPlayer(deviceID, playerID);
                                IS2020::avrcpListPlayerAttributes(deviceID);
                              }
                              break;
                          }
                          if (DEBUG_AVRCP) DBG_AVRCP(F("Current PlayerId:"));
                          DBG_AVRCP(String(avrcpPlayerID[avrcpCurrentPlayerID], HEX));
                          //if (DEBUG_AVRCP) DBG_AVRCP(F(" UID"));
                          //DBG_AVRCP(String(event[15], HEX) + String(event[16], HEX));
                          if (DEBUG_AVRCP) DBG_AVRCP(F("\n"));
                        }
                        break;
                      case AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED:
                        {
                          switch (event[2]) { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                            case AVRCP_EVENT_RESPONSE_NOTIFY:
                              break;
                            case AVRCP_EVENT_RESPONSE_INTERIM:
                              break;
                            case AVRCP_EVENT_RESPONSE_CHANGED:
                              { //re-active event
                                if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                                IS2020::avrcpRegNotifyAvailablePlayersChanged(deviceID);
                              }
                              break;
                          }
                        }
                        break;
                      case AVRCP_EVENT_UIDS_CHANGED:
                        switch (event[2]) { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                          case AVRCP_EVENT_RESPONSE_NOTIFY:
                            break;
                          case AVRCP_EVENT_RESPONSE_INTERIM:
                            break;
                          case AVRCP_EVENT_RESPONSE_CHANGED:
                            { //re-active event
                              if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                              IS2020::avrcpRegNotifyUIDsChanged(deviceID);
                            }
                            break;
                        }
                        break;
                      case AVRCP_EVENT_VOLUME_CHANGED:
                        if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                        switch (event[2]) { //thx to vincent https://github.com/VincentGijsen/MelbusRtos
                          case AVRCP_EVENT_RESPONSE_NOTIFY:
                            break;
                          case AVRCP_EVENT_RESPONSE_INTERIM:
                            break;
                          case AVRCP_EVENT_RESPONSE_CHANGED:
                            { //re-active event
                              //IS2020::avrcpRegNotifyVolumeChanged(deviceID);
                            }
                            break;
                        }
                        break;
                    }
                  }
                  break;
                case AVRCP_GET_CAPABILITIES: //event[12] determinate what capabilities we are receiving, 0x02 = company ID, 0x03=player caps.
                  {
                    if (event[12] == CAP_COMPANY_ID) {//company ID:
                      /*                      if (DEBUG_AVRCP) DBG_AVRCP(F("Company IDs: "));
                                            DBG_AVRCP((String)event[13]);
                                            if (DEBUG_AVRCP) DBG_AVRCP(F(" "));
                                            for (uint8_t numCompID = 0; numCompID < event[13]; numCompID++) { //event[13] = CapabilityCount
                                              for (uint8_t i = 0; i < 3; i++) {
                                                DBG_AVRCP(String(event[14 + (numCompID * 3) + i], HEX));
                                              }
                                              if (DEBUG_AVRCP) DBG_AVRCP(F("\n"));
                                            }*/
                    } else if (event[12] == CAP_EVENTS_SUPPORTED) {
                      for (uint16_t parameter_byte = 14; parameter_byte < (14 + event[13]); parameter_byte++) { //event[13] = CapabilityCount
                        if (event[parameter_byte] == AVRCP_EVENT_PLAYBACK_POS_CHANGED) {
                          IS2020::avrcpRegistrationForNotificationOfEvent(event[1], event[parameter_byte], 0x00, 0x00, 0x13, 0x88);
                        } else {
                          IS2020::avrcpRegistrationForNotificationOfEvent(event[1], event[parameter_byte], 0x00, 0x00, 0x00, 0x00);
                        }
                      }
                    } else {
                      tmp = F("Unknown capabilities ID! ");
                      DBG_EVENTS(tmp + String(event[12], HEX) + "\n");
                    }
                  }
                  break;
                case AVRCP_LIST_PLAYER_ATTRIBUTES:
                  {
                    for (uint8_t i = 0; i < event[12]; i++) {
                      /*switch (event[12] + i) {
                        case AVRCP_ATTRIBUTE_EQUALIZER:
                         // if (DEBUG_AVRCP) DBG_AVRCP(F("Equalizer\n"));
                          break;
                        case AVRCP_ATTRIBUTE_REPEAT_MODE:
                         // if (DEBUG_AVRCP) DBG_AVRCP(F("Repeat\n"));
                          break;
                        case AVRCP_ATTRIBUTE_SHUFFLE:
                         // if (DEBUG_AVRCP) DBG_AVRCP(F("Shuffle\n"));
                          break;
                        case AVRCP_ATTRIBUTE_SCAN:
                         // if (DEBUG_AVRCP) DBG_AVRCP(F("Scan\n"));
                          break;
                        default: //unknown feature id?
                          IS2020::avrcpGetPlayerAttributeText(deviceID, event[12] + i);
                          break;
                        }*/
                      if ((event[12] + i) > 0x04) //non standard attribute
                        IS2020::avrcpGetPlayerAttributeText(deviceID, event[12] + i);
                      else
                        IS2020::avrcpGetCurrentPlayerValue(deviceID, event[12] + i);
                    }
                  }
                  break;
                case AVRCP_LIST_PLAYER_VALUES:
                  {
                    if (DEBUG_AVRCP) DBG_AVRCP(F("Supported settings of this player:\n"));
                    for (uint16_t parameter_byte = 13; parameter_byte < packetSize; parameter_byte++) {
                      IS2020::decodeAvrcpPlayerAtributes(event[12], event[parameter_byte]);
                      // for unknown attributes:
                      if (event[12] > 0x04) IS2020::avrcpGetPlayerValueText(deviceID, event[12], event[parameter_byte]);
                    }
                  }
                  break;
                case AVRCP_GET_CURRENT_PLAYER_VALUE:
                  for (uint16_t parameter_byte = 13; parameter_byte < packetSize; parameter_byte++) {
                    IS2020::decodeAvrcpPlayerAtributes(event[12], event[parameter_byte]);
                    // for unknown attributes:
                    if (event[12] > 0x04) IS2020::avrcpGetPlayerValueText(deviceID, event[12], event[parameter_byte]);
                  }
                  break;
                case AVRCP_SET_PLAYER_VALUE:
                  {
                    if (DEBUG_AVRCP) DBG_AVRCP(F("arvrcp_set_player_value"));
                    if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                  }
                  break;
                case AVRCP_GET_PLAYER_ATTRIBUTE_TEXT:
                  {
                    //if (DEBUG_AVRCP) DBG_AVRCP(F("Text of player attribute:\n"));
                    //if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i<packetSize; i++)   DBG_AVRCP(String(event[i],HEX)+F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                    //event[12] - pocet atributov
                    //event[13] - attrID
                    //event[14][15] = character code
                    //event[16] - string length
                    //event[17]- [x] string
                    uint8_t attribOffset = 13;
                    for (int x = 0; x < event[12]/*numberOfAttribs*/; x++) {
                      uint8_t attributeID = event[attribOffset];
                      //uint16_t codingOfString = event[attribOffset + 1] << 8 | event[attribOffset + 2]; //0x6a == UTF8
                      uint8_t attribValLength = event[attribOffset + 3];
                      for (uint16_t i = 0; i < attribValLength; i++) {
                        //Serial.write(event[attribOffset + 4 + i]);
                        //here we should store this text, cose this is non standard player feature
                      }
                      //Serial.println();
                      attribOffset += 4 + attribValLength;

                    }
                  }
                  break;
                case AVRCP_GET_PLAYER_VALUE_TEXT:
                  {
                    //if (DEBUG_AVRCP) DBG_AVRCP(F("Text of player attribute value:\n"));
                    //if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                    //event[12] - pocet atributov
                    //event[13] - attrID
                    //event[14][15] = character code
                    //event[16] - string length
                    //event[17]- [x] string
                    uint8_t attribOffset = 13;
                    for (int x = 0; x < event[12]/*numberOfAttribs*/; x++) {
                      uint8_t valueID = event[attribOffset];
                      //uint16_t codingOfString = event[attribOffset + 1] << 8 | event[attribOffset + 2]; //0x6a == UTF8
                      uint8_t atribValLength = event[attribOffset + 3];
                      for (uint16_t i = 0; i < atribValLength; i++) {
                        //Serial.write(event[attribOffset + 4 + i]);
                        //here we should store this text, cose this is non standard player feature
                      }
                      //Serial.println();
                      attribOffset += 4 + atribValLength;

                    }
                  }
                  break;
                case AVRCP_GET_ELEMENT_ATTRIBUTES:
                  {
                    //uint8_t numberOfAttribs = event[12];
                    uint8_t attribOffset = 16;
                    for (int x = 0; x < event[12]/*numberOfAttribs*/; x++) {

                      uint8_t mediaAttributeID = event[attribOffset + 0]; //events 13-16, currently in avrcp1.5 only 7 ID's are defined
                      //uint16_t codingOfString = event[attribOffset + 1] << 8 | event[attribOffset + 2]; //will default to only 18... 0x6a == UTF8
                      uint16_t attribValLength = event[attribOffset + 3] << 8 | event[attribOffset + 4];
                      switch (mediaAttributeID) {
                        case AVRCP_MEDIA_ATTRIBUTE_ILLEGAL:
                          Serial.print("Ilegal");
                          break;
                        case AVRCP_MEDIA_ATTRIBUTE_TITLE:
                          Serial.print("Title: ");
                          break;
                        case AVRCP_MEDIA_ATTRIBUTE_ARTIST:
                          Serial.print("Artist: ");
                          break;
                        case AVRCP_MEDIA_ATTRIBUTE_ALBUM:
                          Serial.print("Albun: ");
                          break;
                        case AVRCP_MEDIA_ATTRIBUTE_TRACK:
                          Serial.print("Track: ");
                          break;
                        case AVRCP_MEDIA_ATTRIBUTE_N_TRACKS:
                          Serial.print("Number of tracks: ");
                          break;
                        case AVRCP_MEDIA_ATTRIBUTE_GENRE:
                          Serial.print("Genre: ");
                          break;
                        case AVRCP_MEDIA_ATTRIBUTE_DURATION:
                          Serial.print(F("Duration: "));
                          break;
                        case AVRCP_MEDIA_DEFAULT_COVER_ART: //BIP Image Handle
                          Serial.print(F("Cover: "));
                          break;
                        default:
                          Serial.print(F("\nNot-impl:"));
                      }
                      if (mediaAttributeID == AVRCP_MEDIA_ATTRIBUTE_DURATION)
                      {
                        uint16_t time_MS = 0; //uint32_t for miliseconds....

                        for (uint16_t i = 0; i < attribValLength - 3; i++) { //duration in ms, ascii ,if we need only seconds, then we need only 1st 3 numbers
                          time_MS *= 10;
                          /*if (event[attribOffset + 5 + i] > 0 ) */time_MS += (event[attribOffset + 5 + i] - 0x30);
                        }

                        //uint16_t time_S = time_MS / 1000;
                        //uint8_t time_M = time_S/60; //minutes
                        //time_S = time_S%60;//seconds
                        Serial.print(String(time_MS / 60) + ":" + String(time_MS % 60));
                      } else
                        for (uint16_t i = 0; i < attribValLength; i++) {
                          Serial.write(event[attribOffset + 5 + i]);
                        }
                      Serial.println();
                      //update offset to point to next attribute (if any)
                      attribOffset += 8 + attribValLength;
                    }
                  }
                  break;
                case AVRCP_DISPLAYABLE_CHARSET:
                  if (DEBUG_AVRCP) DBG_AVRCP(F("AVRCP_DISPLAYABLE_CHARSET"));
                  if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                  break;
                case AVRCP_CT_BATTERY_STATUS:
                  if (DEBUG_AVRCP) DBG_AVRCP(F("Battery level: "));
                  switch (event[12]) {
                    case AVRCP_BATTERY_STATUS_NORMAL: if (DEBUG_AVRCP) DBG_AVRCP(F("Normal.\n")); break;
                    case AVRCP_BATTERY_STATUS_WARNING: if (DEBUG_AVRCP) DBG_AVRCP(F("Warning, almost off!\n")); break;
                    case AVRCP_BATTERY_STATUS_CRITICAL: if (DEBUG_AVRCP) DBG_AVRCP(F("Critical, going down!\n")); break;
                    case AVRCP_BATTERY_STATUS_EXTERNAL: if (DEBUG_AVRCP) DBG_AVRCP(F("Charging.\n")); break;
                    case AVRCP_BATTERY_STATUS_FULL_CHARGE: if (DEBUG_AVRCP) DBG_AVRCP(F("Fully charged battery.\n")); break;
                  }
                  break;
                case AVRCP_GET_PLAY_STATUS:
                  {
                    if (DEBUG_AVRCP) DBG_AVRCP(F(" [")); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP(String(event[i], HEX) + F(",")); if (DEBUG_AVRCP) DBG_AVRCP(F("]\n"));
                    if (DEBUG_AVRCP) DBG_AVRCP(F("Song length: "));
                    uint32_t time_MS = 0; //uint32_t for miliseconds....
                    //for (uint16_t i = 0; i < 4; i++) { //duration in ms, ascii ,if we need only seconds, then we need only 1st 3 numbers
                    //time_MS *= 10;
                    //time_MS += (event[12 + i]/* - 0x30*/);
                    //}
                    time_MS = ((uint32_t)event[12] << 24 | (uint32_t)event[13] << 16 | (uint32_t)event[14] << 8 | event[15]) / 1000;
                    //uint16_t time_S = time_MS / 1000;
                    //uint8_t time_M = time_S/60; //minutes
                    //time_S = time_S%60;//seconds
                    DBG_AVRCP(String(time_MS / 60) + ":" + String(time_MS % 60));

                    //Serial.println(tmp + String((event[12] << 24 || event[13] << 16 || event[14] << 8 || event[15]) / 1000, DEC));
                    //DBG_AVRCP(String(event[12], DEC)); DBG_AVRCP(String(event[13], DEC)); DBG_AVRCP(String(event[14], DEC)); DBG_AVRCP(String(event[15], DEC));
                    if (DEBUG_AVRCP) DBG_AVRCP(F("\nSong position: "));
                    /*uint32_t*/ time_MS = 0; //uint32_t for miliseconds....
                    //for (uint16_t i = 0; i < 4; i++) { //duration in ms, ascii ,if we need only seconds, then we need only 1st 3 numbers
                    //time_MS *= 10;
                    //time_MS += (event[16 + i] - 0x30);
                    //}
                    time_MS = ((uint32_t)event[16] << 24 | (uint32_t)event[17] << 16 | (uint32_t)event[18] << 8 | event[19]) / 1000;

                    //uint16_t time_S = time_MS / 1000;
                    //uint8_t time_M = time_S/60; //minutes
                    //time_S = time_S%60;//seconds
                    DBG_AVRCP(String(time_MS / 60) + ":" + String(time_MS % 60));

                    //DBG_AVRCP(String(event[16], DEC)); DBG_AVRCP(String(event[17], DEC)); DBG_AVRCP(String(event[18], DEC)); DBG_AVRCP(String(event[19], DEC));
                    if (DEBUG_AVRCP) DBG_AVRCP(F("\nPlay status: "));
                    switch (event[20]) {
                      case 0x00:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("STOPPED\n"));
                        break;
                      case 0x01:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("PLAYING\n"));
                        //Serial.println("play");
                        //IS2020::avrcpRegistrationForNotificationOfEvent(deviceID, AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED, 0x00, 0x00, 0x00, 0x00);
                        //IS2020::avrcpRegistrationForNotificationOfEvent(deviceID, AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED, 0x00, 0x00, 0x00, 0x00);
                        break;
                      case 0x02:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("PAUSED\n"));
                        break;
                      case 0x03:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("FWD_SEEK\n"));
                        break;
                      case 0x04:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("REV_SEEK\n"));
                        break;
                      case 0xFF:
                        if (DEBUG_AVRCP) DBG_AVRCP(F("ERROR\n"));
                        break;
                    }
                  }
                  break;
                case AVRCP_SET_BROWSED_PLAYER:
                  DBG_AVRCP(String(event[16], DEC)); DBG_AVRCP(String(event[17], DEC)); DBG_AVRCP(String(event[18], DEC)); DBG_AVRCP(String(event[19], DEC));
                  break;
                case AVRCP_REQUEST_CONTINUING:
                  if (DEBUG_AVRCP) DBG_AVRCP(F("AVRCP_REQUEST_CONTINUING"));
                  break;
              }

              //Moved from start to end, so we ask for next data after processing already read data
              //Serial3.print("packet type: ");
              switch (event[9] & 0x07)
              {
                //                case AVRCP_PACKET_TYPE_SINGLE:
                //                  Serial3.println("single packet");
                //                  break;
                //                case AVRCP_PACKET_TYPE_START:
                //                  Serial3.println("start packet");
                //                  break;
                case AVRCP_PACKET_TYPE_CONTINUING:
                  DBG_AVRCP("continue packet");
                  IS2020::avrcpRequestContinuing(event[0], event[8]);// send after all data are processed
                  break;
                  //                case AVRCP_PACKET_TYPE_END:
                  //                  Serial3.println("end packet");
                  //                  break;
                  //                default:
                  //                  Serial3.println(event[3], HEX);
                  //                  break;
              }
            }
            break;
          /*
            Event Format:	Command	Event Code	Event Parameters
          	BTM_Utility_Req	0x1B	action_type, parameter

            Description:	This event is used to ask specific utility request for MCU.

            Event Parameters:	action_type	SIZE: 1 BYTE
          	Value	Parameter Description
          	0x00	BTM ask MCU to control the external amplifier
          	0x01	BTM report the Aux line-in status to Host MCU.
          	others	reserved

          	parameter	SIZE: 1 BYTE
          	action_type=0x00
          	Value	Parameter Description
          	0x00	Mute or switch off amplifier
          	0x01	Unmute or switch on amplifier
          	others	reserved

          	action_type=0x01
          	Value	Parameter Description
          	0x00	Aux line in is unplugged.
          	0x01	Aux line in is plugged.
          	0x02	Aux line in is plugged and with audio signal.
          	0x03	Aux line in is plugged and silence.
          	others	reserved
          */
          case EVT_BTM_Utility_Req:
            {
            }
            break;
          /*
            Event Format:	Event	Event Code	Event Parameters
          	Vendor_AT_Cmd_Rsp	0x01C	data_base_index, status

            Description:	This event is used to reply the Vendor_AT_Cmd command(0x0A).

            Event Parameters:	data_base_index	SIZE: 1 BYTE
          	Value	Parameter Description
          	0x00	database 0 for a dedicate link
          	0x01	database 1 for a dedicate link


          	status	SIZE: 1 BYTE
          	Value	Parameter Description
          	0	AG response OK
          	1	AG response ERROR
          	2	No response from AG
          	others	RFD

          */
          case EVT_Vendor_AT_Cmd_Reply:
            {
              switch (event[2]) {
                case 0x00: DBG(F("AG response OK")); /*allowedSendATcommands=1;*/ break;
                case 0x01: DBG(F("AG response ERROR")); allowedSendATcommands=1; break;
                case 0x02: DBG(F("NO response from AG")); allowedSendATcommands=1; break;
              }
            }
            break;
          /*
            Value	Parameter Description
            0x00	database 0 for a dedicate link
            0x01	database 1 for a dedicate link


            result_payload	SIZE: N BYTES
            Value	Parameter Description
            0xXX…	"result code.
            For example : AG send result code ""+test:1"" , the result code will be ""+test:1"" "

          */
          case EVT_Report_Vendor_AT_Event:
            {
              uint8_t deviceId = event[1];
              //
              if (event[2] == '+') {
                if (DEBUG_EVENTS) DBG_EVENTS(F(" [")); for (uint16_t i = 2; i < packetSize; i++) if (DEBUG_EVENTS)Serial.write(event[i]); if (DEBUG_EVENTS) DBG_EVENTS(F("]\n"));
                switch (event[3]) {
                  case 'C': //command?
                    {
#ifdef PHONEBOOKSUPPORT
                      if (event[4] == 'P' && event[5] == 'B') { //phonebook commands
                        switch (event[6])
                        {
                          case 'S':
                            {
                              //[+CPBS: ("ME","SM","DC","RC","MC")] available phonebooks:
                              for (uint8_t i = 10; i < packetSize; i++) {
                                if (event[i] == '"' && event[i + 3] == '"')
                                {
                                  /*
                                    EN = 1,
                                    FD = 2,
                                    DC = 4,
                                    LD = 8,
                                    MC = 16,
                                    ME = 32,
                                    MT = 64,
                                    ON = 128,
                                    RC = 256,
                                    SM = 512,
                                    SN = 1024
                                  */
                                  if (event[i + 1] == 'E' && event[i + 2] == 'N') bitSet(supportedPBs,EN);
                                  if (event[i + 1] == 'F' && event[i + 2] == 'D') bitSet(supportedPBs,FD);
                                  if (event[i + 1] == 'D' && event[i + 2] == 'C') bitSet(supportedPBs,DC);
                                  if (event[i + 1] == 'L' && event[i + 2] == 'D') bitSet(supportedPBs,LD);
                                  if (event[i + 1] == 'M' && event[i + 2] == 'C') bitSet(supportedPBs,MC);
                                  if (event[i + 1] == 'M' && event[i + 2] == 'E') bitSet(supportedPBs,ME);
                                  if (event[i + 1] == 'M' && event[i + 2] == 'T') bitSet(supportedPBs,MT);
                                  if (event[i + 1] == 'O' && event[i + 2] == 'N') bitSet(supportedPBs,ON);
                                  if (event[i + 1] == 'R' && event[i + 2] == 'C') bitSet(supportedPBs,RC);
                                  if (event[i + 1] == 'S' && event[i + 2] == 'M') bitSet(supportedPBs,SM);
                                  if (event[i + 1] == 'S' && event[i + 2] == 'N') bitSet(supportedPBs,SN);
                                  //Serial.println(supportedPBs,BIN);
                                  i+=4;
                                }
                              }
                            }
                        }
                        break;

                      }
#endif
                    }
                    break;
                }
              }
              allowedSendATcommands=1;
            }
            break;
          /*
            Event Format:  Event    Event Code     Event Parameters
            Read_Link_Status_Reply  0x1E           device_state,
            database0_connect_status,
            database1_connect_status,
            database0_play_status,
            database1_play_status,
            database0_stream_status,
            database1_stream_status,

            Description:  This event is used to reply the Read_Link_Status command.

            Event Parameters:
            device_state  SIZE: 1 BYTE
            Value Parameter Description
            0x00  Power OFF state
            0x01  pairing state (discoverable mode)
            0x02  standby state
            0x03  Connected state with only HF profile connected
            0x04  Connected state with only A2DP profile connected
            0x05  Connected state with only SPP profile connected
            0x06  Connected state with multi-profile connected

            database0_connect_status  SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "1 indicate connected
            Bit0 : A2DP profile signaling channel connected
            Bit1 : A2DP profile stream channel connected
            Bit2 : AVRCP profile connected
            Bit3 : HF profile connected
            Bit4 : SPP connected"

            database1_connect_status  SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "1 indicate connected
            Bit0 : A2DP profile signaling channel connected
            Bit1 : A2DP profile stream channel connected
            Bit2 : AVRCP profile connected
            Bit3 : HF profile connected
            Bit4 : SPP connected"

            "database0_play_status,
            database1_play_status, " SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "0x00: STOP
            0x01: PLAYING
            0x02: PAUSED
            0x03: FWD_SEEK
            0x04: REV_SEEK
            0x05: FAST_FWD
            0x06: REWIND
            0x07: WAIT_TO_PLAY
            0x08: WAIT_TO_PAUSE"

            "database0_stream_status,
            database1_stream_status, " SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "1 indicate connected
            0x00: no stream
            0x01: stream on - going"

          */
          case EVT_Read_Link_Status_Reply:
            {

              for (uint8_t i = 0; i < 7; i++) {
                if (linkStatus[i] != event[i + 1])
                {
                  linkStatus[i] = event[i + 1]; //link status is array of 7 bytes, like response is 7bytes.
                  btmStatusChanged = 1;
                }
              }
              if (linkStatus[1] > 0) {
                IS2020::queryDeviceName(0x00);
                //IS2020::queryInBandRingtone_status(0x00);
                //IS2020::queryIfRemoteDeviceIsIAPDevice(0x00);
                //IS2020::queryIfRemoteDeviceSupportAavrcpV13(0x00);
                //IS2020::queryHfA2DPGain(0x00);
                //IS2020::queryLineInGain(0x00);
                //IS2020::avrcpDisplayableCharset(0x00);
                //IS2020::avrcpGetCapabilities(0x00, 0x02); //Get Capability command for Company ID
                IS2020::avrcpGetCapabilities(0x00, 0x03); //Get Capability command for Events
                IS2020::avrcpListPlayerAttributes(0x00);
                //IS2020::registerAllEvents(0x00);
              } else {
                removeInfoAboutDevice(0);
              }
              if (linkStatus[2] > 0) {
                IS2020::queryDeviceName(1);
                //IS2020::queryInBandRingtoneStatus(0x01);
                //IS2020::queryIfRemoteDeviceIsIAPDevice(0x01);
                //IS2020::queryIfRemoteDeviceSupportAvrcpV13(0x01);
                //IS2020::queryHfA2DPGain(0x01);
                //IS2020::queryLineInGain(0x01);
                //IS2020::avrcpDisplayableCharset(0x01);
                //IS2020::avrcpGetCapabilities(0x01, 0x02);
                IS2020::avrcpGetCapabilities(0x01, 0x03);
                IS2020::avrcpListPlayerAttributes(0x01);
                // IS2020::registerAllEvents(0x01);
              } else {
                IS2020::removeInfoAboutDevice(1);
              }
            }
            break;
          /*
            Event Format:   Event                             Event Code    Event Parameters
            Read_Paired_Device_Record_Reply   0x1F          paired_device_number, paired_record

            Description:  This event is used to reply the  Read_Paired_Device_Information command.

            Event Parameters: paired_de3vice_number  SIZE: 1 BYTE
            Parameter Description
            byte0 the paired device number.

            paired_record : 7bytes per record SIZE: (7*total_record) BYTE
            Parameter Description
            byte0 link priority : 1 is the highest(newest device) and 4 is the lowest(oldest device)
            byte1~byte6 linked device BD address (6 bytes with low byte first)
            …
            notes:
            cmd event[0]
            paired_device_id event[1]
            ink_priority event[2]
            bt_5 event[3]
            bt_4 event[4]
            bt_3 event[5]
            bt_2 event[6]
            bt-1 event[7]
            bt_0 event[8]
          */
          case EVT_Read_Paired_Device_Record_Reply:
            {
              for (uint8_t i = 0; i < 8; i++) {
                for (uint8_t y = 0; y < 6; y++)
                  btAddress[i][y] = 0;
              }
              for (uint8_t dev = event[1]; dev > 0; dev--)
              {
                uint8_t pos = dev - 1; //0,1,2,...
                uint8_t offset = pos * 7;
                btAddress[pos][0] = event[offset + 8];
                btAddress[pos][1] = event[offset + 7];
                btAddress[pos][2] = event[offset + 6];
                btAddress[pos][3] = event[offset + 5];
                btAddress[pos][4] = event[offset + 4];
                btAddress[pos][5] = event[offset + 3];
              }/*
                                  for (uint8_t i=0;i<8;i++){
                                  Serial.print("Device ");
                                  Serial.print(i);
  Serial.print(": ");
                                  Serial.print(btAddress[i][0],HEX);
  Serial.print(": ");
                                  Serial.print(btAddress[i][1],HEX);
  Serial.print(": ");
                                  Serial.print(btAddress[i][2],HEX);
  Serial.print(": ");
                                  Serial.print(btAddress[i][3],HEX);
  Serial.print(": ");
                                  Serial.print(btAddress[i][4],HEX);
  Serial.print(": ");
                                  Serial.println(btAddress[i][5],HEX);
                                }*/
            }
            break;
          case EVT_Read_Local_BD_Address_Reply:
            {
              if (DEBUG_EVENTS) DBG_EVENTS(F("Local BT adr: "));
              /*for (uint8_t _byte = 0; _byte < 6; _byte++) {
                IS2020::moduleBtAddress[5 - _byte] = event[_byte + 1];
                }*/
              for (uint8_t _byte = 0; _byte < 6; _byte++) {
                IS2020::moduleBtAddress[_byte] = event[6 - _byte];
                DBG_EVENTS(String(IS2020::moduleBtAddress[_byte], HEX));
                if (_byte < 5) if (DEBUG_EVENTS) DBG_EVENTS(F(": "));
              }
              if (DEBUG_EVENTS) DBG_EVENTS(F("\n"));
            }
            break;
          case EVT_Read_Local_Device_Name_Reply:
            {
              IS2020::localDeviceName = "";
              //if (DEBUG_EVENTS) DBG_EVENTS(F("size: "));
              //DBG_EVENTS(String(event[1])+"\n");
              for (uint8_t _byte = 0; _byte < event[1]; _byte++) {
                //Serial3.write(event[_byte+2]);
                IS2020::localDeviceName += (char)event[_byte + 2];
              }
              //if (DEBUG_EVENTS) DBG_EVENTS(F("BT name: "));
              //DBG_EVENTS(LocalDeviceName);
            }
            break;
          case EVT_Report_SPP_iAP_Data:
            {
            }
            break;
          case EVT_Report_Link_Back_Status:
            {
            }
            break;
          case EVT_Ringtone_Finish_Indicate:
            {
            }
            break;
          case EVT_User_Confrim_SSP_Req:
            {
            }
            break;
          case EVT_Report_AVRCP_Vol_Ctrl:
            {
            }
            break;
          case EVT_Report_Input_Signal_Level:
            {
            }
            break;
          case EVT_Report_iAP_Info:
            {
            }
            break;
          case EVT_REPORT_AVRCP_ABS_VOL_CTRL:
            {
            }
            break;
          case EVT_Report_Voice_Prompt_Status:
            {
            }
            break;
          case EVT_Report_MAP_Data:
            {
            }
            break;
          case EVT_Security_Bonding_Res:
            {
            }
            break;
          case EVT_Report_Type_Codec:
            {
            }
            break;
          default:
            {
              /*Serial3.println();
                Serial3.print("Unknown  BYTE: ");
                for (uint8_t i = 0; i < packetSize; i++) {
                Serial3.print(event[i], HEX);
                }
                Serial3.println();*/
            }
        }
        return event[0];
      }
    }
  }
  return false;
}

void IS2020::decodeEvent(uint8_t Event) {
  switch (Event) {
    case 0x00:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Command_ACK"));
      break;
    case 0x01:
      if (DEBUG_EVENTS) DBG_EVENTS(F("BTM_Status"));
      break;
    case 0x02:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Call_Status"));
      break;
    case 0x03:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Caller_ID"));
      break;
    case 0x04:
      if (DEBUG_EVENTS) DBG_EVENTS(F("SMS_Received_Indication"));
      break;
    case 0x05:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Missed_Call_Indication"));
      break;
    case 0x06:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Phone_Max_Battery_Level"));
      break;
    case 0x07:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Phone_Current_Battery_Level"));
      break;
    case 0x08:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Roaming_Status"));
      break;
    case 0x09:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Phone_Max_Signal_Strength_Level"));
      break;
    case 0x0A:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Phone_Current_Signal_Strength_Level"));
      break;
    case 0x0B:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Phone_Service_Status"));
      break;
    case 0x0C:
      if (DEBUG_EVENTS) DBG_EVENTS(F("BTM_Battery_Status"));
      break;
    case 0x0D:
      if (DEBUG_EVENTS) DBG_EVENTS(F("BTM_Charging_Status"));
      break;
    case 0x0E:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Reset_To_Default"));
      break;
    case 0x0F:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_HF_Gain_Level"));
      break;
    case 0x10:
      if (DEBUG_EVENTS) DBG_EVENTS(F("EQ_Mode_Indication"));
      break;
    case 0x11:
      if (DEBUG_EVENTS) DBG_EVENTS(F("PBAP_Missed_Call_History"));
      break;
    case 0x12:
      if (DEBUG_EVENTS) DBG_EVENTS(F("PBAP_Received_Call_History"));
      break;
    case 0x13:
      if (DEBUG_EVENTS) DBG_EVENTS(F("PBAP_Dialed_Call_History"));
      break;
    case 0x14:
      if (DEBUG_EVENTS) DBG_EVENTS(F("PBAP_Combine_Call_History"));
      break;
    case 0x15:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Get_PB_By_AT_Cmd_Reply"));
      break;
    case 0x16:
      if (DEBUG_EVENTS) DBG_EVENTS(F("PBAP_Access_Finish"));
      break;
    case 0x17:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read_Linked_Device_Information_Reply"));
      break;
    case 0x18:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read_BTM_Version_Reply"));
      break;
    case 0x19:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Call_List_Report"));
      break;
    case 0x1A:
      if (DEBUG_EVENTS) DBG_EVENTS(F("AVRCP_Specific_Rsp"));
      break;
    case 0x1B:
      if (DEBUG_EVENTS) DBG_EVENTS(F("BTM_Utility_Req"));
      break;
    case 0x1C:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Vendor_AT_Cmd_Reply"));
      break;
    case 0x1D:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_Vendor_AT_Event"));
      break;
    case 0x1E:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read_Link_Status_Reply"));
      break;
    case 0x1F:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read_Paired_Device_Record_Reply"));
      break;
    case 0x20:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read_Local_BD_Address_Reply"));
      break;
    case 0x21:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read_Local_Device_Name_Reply"));
      break;
    case 0x22:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_SPP_iAP_Data"));
      break;
    case 0x23:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_Link_Back_Status"));
      break;
    case 0x24:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Ringtone_Finish_Indicate"));
      break;
    case 0x25:
      if (DEBUG_EVENTS) DBG_EVENTS(F("User_Confrim_SSP_Req"));
      break;
    case 0x26:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_AVRCP_Vol_Ctrl"));
      break;
    case 0x27:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_Input_Signal_Level"));
      break;
    case 0x28:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_iAP_Info"));
      break;
    case 0x29:
      if (DEBUG_EVENTS) DBG_EVENTS(F("REPORT_AVRCP_ABS_VOL_CTRL"));
      break;
    case 0x2A:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_Voice_Prompt_Status"));
      break;
    case 0x2B:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_MAP_Data"));
      break;
    case 0x2C:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Security_Bonding_Res"));
      break;
    case 0x2D:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Report_Type_Codec"));
      break;
  }
}

void IS2020::decodeCommandInEvents(uint8_t cmd) {
  switch (cmd) {
    case CMD_Make_Call:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Make Call"));
      break;
    case CMD_Make_Extension_Call:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Make Extension Call"));
      break;
    case CMD_MMI_Action:
      if (DEBUG_EVENTS) DBG_EVENTS(F("MMI Action"));
      break;
    case CMD_Event_Mask_Setting:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Event Mask Setting"));
      break;
    case CMD_Music_Control:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Music Control"));
      break;
    case CMD_Change_Device_Name:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Change Device Name"));
      break;
    case CMD_Change_PIN_Code:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Change PIN Code"));
      break;
    case CMD_BTM_Parameter_Setting:
      if (DEBUG_EVENTS) DBG_EVENTS(F("BTM_Parameter_Setting"));
      break;
    case CMD_Read_BTM_Version:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read_BTM_Version"));
      break;
    case CMD_Get_PB_By_AT_Cmd:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Get PB By AT Cmd"));
      break;
    case CMD_Vendor_AT_Command:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Vendor AT Command"));
      break;
    case CMD_AVRCP_Specific_Cmd:
      if (DEBUG_EVENTS) DBG_EVENTS(F("AVRCP Specific Cmd"));
      break;
    case CMD_AVRCP_Group_Navigation:
      if (DEBUG_EVENTS) DBG_EVENTS(F("AVRCP Group Navigation"));
      break;
    case CMD_Read_Link_Status:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read Link Status"));
      break;
    case CMD_Read_Paired_Device_Record:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read Paired Device Record"));
      break;
    case CMD_Read_Local_BT_Address:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read Local BT Address"));
      break;
    case CMD_Read_Local_Device_Name:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read Local Device Name"));
      break;
    case CMD_Set_Access_PB_Method:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Set Access PB Method"));
      break;
    case CMD_Send_SPP_iAP_Data:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Send SPP iAP Data"));
      break;
    case CMD_BTM_Utility_Function:
      if (DEBUG_EVENTS) DBG_EVENTS(F("BTM Utility Function"));
      break;
    case CMD_Event_ACK:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Event ACK"));
      break;
    case CMD_Additional_Profiles_Link_Setup:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Additional Profiles Link Setup"));
      break;
    case CMD_Read_Linked_Device_Information:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Read Linked Device Information"));
      break;
    case CMD_Profile_Link_Back:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Profile Link Back"));
      break;
    case CMD_Disconnect:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Disconnect"));
      break;
    case CMD_MCU_Status_Indication:
      if (DEBUG_EVENTS) DBG_EVENTS(F("MCU Status Indication"));
      break;
    case CMD_User_Confirm_SPP_Req_Reply:
      if (DEBUG_EVENTS) DBG_EVENTS(F("User Confirm SPP Req Reply"));
      break;
    case CMD_Set_HF_Gain_Level:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Set HF Gain Level"));
      break;
    case CMD_EQ_Mode_Setting:
      if (DEBUG_EVENTS) DBG_EVENTS(F("EQ Mode Setting"));
      break;
    case CMD_DSP_NR_CTRL:
      if (DEBUG_EVENTS) DBG_EVENTS(F("DSP NR CTRL"));
      break;
    case CMD_GPIO_Control:
      if (DEBUG_EVENTS) DBG_EVENTS(F("GPIO Control"));
      break;
    case CMD_MCU_UART_Rx_Buffer_Size:
      if (DEBUG_EVENTS) DBG_EVENTS(F("MCU UART Rx Buffer Size"));
      break;
    case CMD_Voice_Prompt_Cmd:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Voice Prompt Cmd"));
      break;
    case CMD_MAP_REQUEST:
      if (DEBUG_EVENTS) DBG_EVENTS(F("MAP REQUEST"));
      break;
    case CMD_Security_Bonding_Req:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Security Bonding Req"));
      break;
    case CMD_Set_Overall_Gain:
      if (DEBUG_EVENTS) DBG_EVENTS(F("Set Overall Gain"));
      break;
    default:
      DBG_EVENTS("Unknown CMD command " + String(cmd, HEX));
      break;
  }
}

void IS2020::decodeRejectReason(uint8_t event) {
  switch (event)
  {
    case AVRCP_STATUS_INVALID_COMMAND: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("INVALID COMMAND: "));
      } break;
    case AVRCP_STATUS_INVALID_PARAM: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("INVALID PARAM: "));
      } break;
    case AVRCP_STATUS_PARAM_NOT_FOUND: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("PARAM NOT FOUND: "));
      } break;
    case AVRCP_STATUS_INTERNAL_ERROR: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("INTERNAL ERROR: "));
      } break;
    case AVRCP_STATUS_SUCCESS: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("SUCCESS"));
      } break;
    case AVRCP_STATUS_UID_CHANGED: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("UID CHANGED"));
      } break;
    case AVRCP_STATUS_INVALID_DIRECTION: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("INVALID DIRECTION"));
      } break;
    case AVRCP_STATUS_NOT_DIRECTORY: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("NOT DIRECTORY"));
      } break;
    case AVRCP_STATUS_NOT_EXIST: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("NOT EXIST"));
      } break;
    case AVRCP_STATUS_INVALID_SCOPE: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("INVALID SCOPE"));
      } break;
    case AVRCP_STATUS_RANGE_OUT_OF_BOUNDS: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("RANGE OUT OF BOUNDS"));
      } break;
    case AVRCP_STATUS_FOLDER_NOT_PLAYABLE: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("FOLDER NOT PLAYABLE"));
      } break;
    case AVRCP_STATUS_MEDIA_IN_USE: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("MEDIA IN USE"));
      } break;
    case AVRCP_STATUS_NOW_PLAYING_LIST_FULL: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("NOW PLAYING LIST FULL"));
      } break;
    case AVRCP_STATUS_SEARCH_NOT_SUPPORTED: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("SEARCH NOT SUPPORTED"));
      } break;
    case AVRCP_STATUS_SEARCH_IN_PROGRESS: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("SEARCH IN PROGRESS"));
      } break;
    case AVRCP_STATUS_INVALID_PLAYER_ID: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("INVALID PLAYER ID"));
      } break;
    case AVRCP_STATUS_PLAYER_NOT_BROWSABLE: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("PLAYER NOT BROWSABLE"));
      } break;
    case AVRCP_STATUS_PLAYER_NOT_ADDRESSED: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("PLAYER NOT ADDRESSED"));
      } break;
    case AVRCP_STATUS_NO_VALID_SEARCH_RESULT: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("NO VALID SEARCH RESULT"));
      } break;
    case AVRCP_STATUS_NO_AVAILABLE_PLAYERS: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("NO AVAILABLE PLAYERS"));
      } break;
    case AVRCP_STATUS_ADDRESSED_PLAYER_CHANGED: {
        if (DEBUG_AVRCP) DBG_AVRCP(F("ADDRESSED PLAYER CHANGED"));
      } break;
  }
}
