#include "IS2020.h"
#include <Arduino.h>
#include "MMI.h"

/*

  Command Format:   Command Command ID  Command Parameters      Return Event
  mmiAction        0x02                data_base_index, action

  Description:  MMI action

  Command Parameters: data_base_index SIZE: 1 BYTE
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
  for BM62(like):
  0x18  Enable RX noise reduction
  0x19  Disable RX noise reduction
  0x1A  Switch RX noise reduction
  0x1B  Enable TX noise reduction
  0x1C  Disable TX noise reduction
  0x1D  Switch TX noise reduction
  0x1E  Enable AEC
  0x1F  Disalbe AEC
  0x20  Switch AEC enable/disable
  0x21  Enable AEC RX noise reduction
  0x22  Disable AEC RX noise reduction
  0x23  Switch AEC RX noise reduction
  0x32  Play/Pause
  0x33  Stop
  0x36  Faseforward
  0x37  Rewind
  0x38  EQ mode up
  0x39  EQ mode down
  0x3A  Lock button
  0x3E  Toggle 3D effect
  0x3F  report current EQ mode
  0x40  report current audio effect status
  0x53  power off button press
  0x54  power off button release
  0x55  Reverse panel
  0x57  Force speaker gain toggle
  0x58  Toggle button indicatoin
  0x59  Combine function 0
  0x5A  Combine function 1
  0x5B  Combine function 2
  0x5C  Combine function 3
  0x64  Retrieve phonebook
  0x65  Retrieve MCH
  0x66  Retrieve ICH
  0x67  Retrieve OCH
  0x68  Retrieve CCH
  0x69  Cancel access PBAP
  0x6E  OHS event 1
  0x6F  OHS event 2
  0x70  OHS event 3
  0x71  OHS event 4
  0x72  SHS_SEND_USER_DATA_1 (for embedded application mode)
  0x73  SHS_SEND_USER_DATA_2 (for embedded application mode)
  0x74  SHS_SEND_USER_DATA_3 (for embedded application mode)
  0x75  SHS_SEND_USER_DATA_4 (for embedded application mode)
  0x76  SHS_SEND_USER_DATA_5 (for embedded application mode)
  0x77  report current RX NR status
  0x78  report current TX NR status
  0x79  force buzzer alarm
  0x7A  Cancel all BT paging
  0xE0  Trigger NSPK Master
  0xE1  Trigger NSPK Slave
  0xE2  NSPK one key connect/disconnect
  0xE3  Cancel NSPK creation
  0xE4  Terminate NSPK link
  0xE5  Terminate / Cancel NSPK connection
  0xE6  NSPK Master enter Aux-in 44.1K PCM Encoder mode
  0xE7  NSPK Master enter Aux-in 48K PCM Encoder mode
  0xE8  NSPK Master exit Aux-in PCM Encoder mode
  0xE9  NSPK Master enter Aux-in SBC Encoder mode
  0xEA  NSPK Master exit Aux-in SBC Encoder mode
  0xEB  NSPK dynamic creation
  0xEC  NSPK switch channel
  0xED  NSPK power off all speakers
  0xEE  NSPK AFH SBCENCODING AUDIOSYNC
  0xF0  NSPK MASTER PAGE SLAVE FOR NEW SLAVE
  0xF1  NSPK SLAVE ENABLE PAGE SCAN FOR NEW MASTER
  0xF2  NSPK SLAVE USE SLOW PAGE SCAN
  0xF3  NSPK SLAVE USE FAST PAGE SCAN
  0xF4  NSPK_ENTER_NSPK_MODE
  0xF5  NSPK_ENTER_BROADCAST_MODE


  Note1:Query call list information(CLCC)
  action  SIZE: 1 BYTE
  Value Parameter Description
  0x0f  trigger SPK Module to query call list information

  Note2:Fast enter pairing mode (0x5D)
  data_base_index SIZE: 1 BYTE
  Value Parameter Description
  0x00  BTM generate tone when enter pairing mode
  others  BTM doesn't generate tone when enter pairing mode


*/

void IS2020::decodeMMI(uint8_t mmiAction) {
  switch (mmiAction) {
    case MMI_add_remove_SCO_link:   DBG(F("Add remove SCO link."));    break;
    case MMI_force_end_active_call: DBG(F("Force and active call.")); break;
    case MMI_accept_incoming_call:  DBG(F("Accept incoming call.")); break;
    case MMI_reject_incoming_call:  DBG(F("Reject incoming call.")); break;
    case MMI_end_call_transfer_to_headset: DBG(F("End call transfer to headset.")); break;
    case MMI_toggle_mic_mute: DBG(F("Toggle mic mute.")); break;
    case MMI_mute_mic: DBG(F("Mute mic.")); break;
    case MMI_unmute_mic:            DBG(F("Unmute mic.")); break;
    case MMI_voice_dial: DBG(F("Voice dial.")); break;
    case MMI_cancel_voice_dial: DBG(F("Cancel voice dial.")); break;
    case MMI_last_number_redial: DBG(F("Last number redial.")); break;
    case MMI_toggle_active_hold_call: DBG(F("Toggle active hold call.")); break;
    case MMI_voice_transfer: DBG(F("Voice transfer.")); break;
    case MMI_query_call_list_info: DBG(F("Query call list info.")); break;
    case MMI_three_way_call: DBG(F("Three way call.")); break;
    case MMI_release_waiting_or_hold_call: DBG(F("Release waiting or hold call.")); break;
    case MMI_end_active_call_accept_waiting_or_held_call: DBG(F("End active call accept waiting or held call.")); break;
    case MMI_initiate_HF_connection: DBG(F("Initiate HF connection.")); break;
    case MMI_disconnect_HF_link: DBG(F("Disconnect HF link.")); break;
    case MMI_increase_microphone_gain: DBG(F("Increase microphone gain.")); break;
    case MMI_decrease_microphone_gain: DBG(F("Decrease microphone gain.")); break;
    case MMI_switch_primary_secondary_HF: DBG(F("Switch primary secondary HF")); break;
    case MMI_increase_speaker_gain: DBG(F("Increase speaker gain.")); break;
    case MMI_decrease_speaker_gain: DBG(F("Decrease speaker gain.")); break;
    case MMI_nextSong: DBG(F("Next song.")); break;
    case MMI_previousSong: DBG(F("Previous song.")); break;
    case MMI_disconnect_A2DP_link: DBG(F("Disconnect A2DP link.")); break;
    case MMI_next_audio_efect: DBG(F("Next audio efect.")); break;
    case MMI_previous_audio_efect: DBG(F("Previous audio efect.")); break;
    case MMI_enter_pairing_mode: DBG(F("Enter pairing mode.")); break;
    case MMI_power_on_button_press: DBG(F("Power on button press.")); break;
    case MMI_power_on_button_release: DBG(F("Power on button release.")); break;
    case MMI_eeprom_to_defaults: DBG(F("Eeprom to defaults.")); break;
    case MMI_enter_pairing_mode_fast: DBG(F("Enter pairing mode fast.")); break;
    case MMI_power_off: DBG(F("Power off.")); break;
    case MMI_toggle_buzzer: DBG(F("Toggle buzzer.")); break;
    case MMI_disable_buzzer: DBG(F("Disable buzzer.")); break;
    case MMI_toggle_buzzer2: DBG(F("Toggle buzzer2.")); break;
    case MMI_change_tone: DBG(F("Change tone.")); break;
    case MMI_battery_status: DBG(F("Battery status.")); break;
    case MMI_exit_pairing_mode: DBG(F("Exit pairing mode.")); break;
    case MMI_link_last_device: DBG(F("Link last device.")); break;
    case MMI_disconnect_all_link: DBG(F("Disconnect all link.")); break;
    //#ifdef BM62
    case MMI_enableRXnoiseReduction: DBG(F("Enable RX noise reduction")); break;
    case MMI_disableRXnoiseReduction: DBG(F("Disable RX noise reduction")); break;
    case MMI_switchRXnoiseReduction: DBG(F("Switch RX noise reduction")); break;
    case MMI_enableTXnoiseReduction: DBG(F("Enable TX noise reduction")); break;
    case MMI_disableTXnoiseReduction: DBG(F("Disable TX noise reduction")); break;
    case MMI_switchTXnoiseReduction: DBG(F("Switch TX noise reduction")); break;
    case MMI_enableAEC: DBG(F("Enable AEC")); break;
    case MMI_disableAEC: DBG(F("Disalbe AEC")); break;
    case MMI_switchAEC: DBG(F("Switch AEC enable/disable")); break;
    case MMI_enableAECRXnoiseReduction: DBG(F("Enable AEC RX noise reduction")); break;
    case MMI_disableAECRXnoiseReduction: DBG(F("Disable AEC RX noise reduction")); break;
    case MMI_switchAECRXnoiseReduction: DBG(F("Switch AEC RX noise reduction")); break;
    case MMI_togglePlayPause: DBG(F("Play/Pause")); break;
    case MMI_stop: DBG(F("Stop")); break;
    case MMI_FastForward: DBG(F("Faseforward")); break;
    case MMI_Rewind: DBG(F("Rewind")); break;
    case MMI_EQmodeUp: DBG(F("EQ mode up")); break;
    case MMI_EQmodeDown: DBG(F("EQ mode down")); break;
    case MMI_LockButton: DBG(F("Lock button")); break;
    case MMI_toggle3Deffect: DBG(F("Toggle 3D effect")); break;
    case MMI_currentEQmode: DBG(F("report current EQ mode")); break;
    case MMI_currentAudioEffectStatus: DBG(F("report current audio effect status")); break;
    case MMI_powerOffButtonPress: DBG(F("power off button press")); break;
    case MMI_powerOffButtonRelease: DBG(F("power off button release")); break;
    case MMI_reversePanel: DBG(F("Reverse panel")); break;
    case MMI_forceSpeakerGainToggle: DBG(F("Force speaker gain toggle")); break;
    case MMI_toggleButtonIndication: DBG(F("Toggle button indicatoin")); break;
    case MMI_combineFunction0: DBG(F("Combine function 0")); break;
    case MMI_combineFunction1: DBG(F("Combine function 1")); break;
    case MMI_combineFunction2: DBG(F("Combine function 2")); break;
    case MMI_combineFunction3: DBG(F("Combine function 3")); break;
    case MMI_retrievePhonebook: DBG(F("Retrieve phonebook")); break;
    case MMI_retrieveMCH: DBG(F("Retrieve MCH")); break;
    case MMI_retrieveICH: DBG(F("Retrieve ICH")); break;
    case MMI_retrieveOCH: DBG(F("Retrieve OCH")); break;
    case MMI_retrieveCCH: DBG(F("Retrieve CCH")); break;
    case MMI_cancelAccessPBAP: DBG(F("Cancel access PBAP")); break;
    case MMI_OHSevent1: DBG(F("OHS event 1")); break;
    case MMI_OHSevent2: DBG(F("OHS event 2")); break;
    case MMI_OHSevent3: DBG(F("OHS event 3")); break;
    case MMI_OHSevent4: DBG(F("OHS event 4")); break;
    case MMI_SHS_SEND_USER_DATA_1: DBG(F("SHS_SEND_USER_DATA_1 (for embedded application mode)")); break;
    case MMI_SHS_SEND_USER_DATA_2: DBG(F("SHS_SEND_USER_DATA_2 (for embedded application mode)")); break;
    case MMI_SHS_SEND_USER_DATA_3: DBG(F("SHS_SEND_USER_DATA_3 (for embedded application mode)")); break;
    case MMI_SHS_SEND_USER_DATA_4: DBG(F("SHS_SEND_USER_DATA_4 (for embedded application mode)")); break;
    case MMI_SHS_SEND_USER_DATA_5: DBG(F("SHS_SEND_USER_DATA_5 (for embedded application mode)")); break;
    case MMI_currentRXnoiseReduction: DBG(F("report current RX NR status")); break;
    case MMI_currentTXnoiseReduction: DBG(F("report current TX NR status")); break;
    case MMI_forceBuzzerAlarm: DBG(F("force buzzer alarm")); break;
    case MMI_cancelAllBTpaging: DBG(F("Cancel all BT paging")); break;
    case MMI_triggerNSPKmaster: DBG(F("Trigger NSPK Master")); break;
    case MMI_triggerNSPKslav: DBG(F("Trigger NSPK Slave")); break;
    case MMI_NSPK_ONE_KEY_CONNECT_DISCONECT: DBG(F("NSPK one key connect/disconnect")); break;
    case MMI_CANCEL_NSPK_CREATION: DBG(F("Cancel NSPK creation")); break;
    case MMI_TERMINAL_NSPK_LINK: DBG(F("Terminate NSPK link")); break;
    case MMI_TERMINATE_CANCEL_NSPK_CONNECTION: DBG(F("Terminate / Cancel NSPK connection")); break;
    case MMI_NSPK_MASTER_ENTER_AUX_IN_441K_PCM_ENCODER_MODE: DBG(F("NSPK Master enter Aux-in 44.1K PCM Encoder mode")); break;
    case MMI_NSPK_MASTER_ENTER_AUX_IN_48K_PCM_ENCODER_MODE: DBG(F("NSPK Master enter Aux-in 48K PCM Encoder mode")); break;
    case MMI_NSPK_MASTER_EXIT_AUX_IN_PCM_ENCODER_MODE: DBG(F("NSPK Master exit Aux-in PCM Encoder mode")); break;
    case MMI_NSPK_MASTER_ENTER_AUX_IN_SBC_ENCODER_MDOE: DBG(F("NSPK Master enter Aux-in SBC Encoder mode")); break;
    case MMI_NSPK_MASTER_EXIT_AUX_IN_SBC_ENCODER_MODE: DBG(F("NSPK Master exit Aux-in SBC Encoder mode")); break;
    case MMI_NSPK_DYNAMIC_CREATION: DBG(F("NSPK dynamic creation")); break;
    case MMI_NSPK_SWITCH_CHANNEL: DBG(F("NSPK switch channel")); break;
    case MMI_NSPK_POWER_OFF_ALL_SPEAKERS: DBG(F("NSPK power off all speakers")); break;
    case MMI_NSPK_AFH_SFC_ENCODING_AUDIOSYNC: DBG(F("NSPK AFH SBCENCODING AUDIOSYNC")); break;
    case MMI_NSPK_MASTER_PAGE_SLAVE_FOR_NEW_SLAVE: DBG(F("NSPK MASTER PAGE SLAVE FOR NEW SLAVE")); break;
    case MMI_NSPK_SLAVE_ENABLE_PAGE_SCAN_FOR_NEW_MASTER: DBG(F("NSPK SLAVE ENABLE PAGE SCAN FOR NEW MASTER")); break;
    case MMI_NSPK_SLAVE_USE_SLOW_PAGE_SCAN: DBG(F("NSPK SLAVE USE SLOW PAGE SCAN")); break;
    case MMI_NSPK_SLAVE_USE_FAST_PAGE_SCAN: DBG(F("NSPK SLAVE USE FAST PAGE SCAN")); break;
    case MMI_NSPK_ENTER_NSPK_MODE: DBG(F("NSPK_ENTER_NSPK_MODE")); break;
    case MMI_NSPK_ENTER_BROADCAST_MODE: DBG(F("NSPK_ENTER_BROADCAST_MODE")); break;
    //#endif
  }
}

uint8_t IS2020::addRemoveScoLink(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_add_remove_SCO_link);
}

uint8_t IS2020::forceEndActiveCall(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_force_end_active_call);
}

uint8_t IS2020::acceptIncomingCall(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_accept_incoming_call);
}

uint8_t IS2020::rejectIncomingCall(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_reject_incoming_call);
}

uint8_t IS2020::endCallTransferToHeadset(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_end_call_transfer_to_headset);
}

uint8_t IS2020::toggleMicMute(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_toggle_mic_mute);
}

uint8_t IS2020::muteMic(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_mute_mic);
}

uint8_t IS2020::unmuteMic(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_unmute_mic);
}

uint8_t IS2020::voiceDial(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_voice_dial);
}

uint8_t IS2020::cancelVoiceDial(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_cancel_voice_dial);
}

uint8_t IS2020::lastNumberRedial(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_last_number_redial);
}

uint8_t IS2020::toggleActiveHoldCall(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_toggle_active_hold_call);
}

uint8_t IS2020::voiceTransfer(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_voice_transfer);
}

uint8_t IS2020::queryCallListInfo(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_query_call_list_info);
}

uint8_t IS2020::threeWayCall(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_three_way_call);
}

uint8_t IS2020::releaseWaitingOrHoldCall(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_release_waiting_or_hold_call);
}

uint8_t IS2020::endActiveCallAcceptWaitingOrHeldCall(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_end_active_call_accept_waiting_or_held_call);
}

uint8_t IS2020::initiateHfConnection(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_initiate_HF_connection);
}

uint8_t IS2020::disconnectHfLink(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_disconnect_HF_link);
}

uint8_t IS2020::increaseMicrophoneGain(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_increase_microphone_gain);
}

uint8_t IS2020::decreaseMicrophoneGain(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_decrease_microphone_gain);
}

uint8_t IS2020::switchPrimarySecondayHf(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_switch_primary_secondary_HF);
}

uint8_t IS2020::increaseSpeakerGain(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_increase_speaker_gain);
}

uint8_t IS2020::decreaseSpeakerGain(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_decrease_speaker_gain);
}

uint8_t IS2020::nextSong(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_nextSong);
}

uint8_t IS2020::previousSong(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_previousSong);
}

uint8_t IS2020::disconnectA2DPLink(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_disconnect_A2DP_link);
}

uint8_t IS2020::nextAudioEfect(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_next_audio_efect);
}

uint8_t IS2020::previousAudioEfect(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_previous_audio_efect);
}

uint8_t IS2020::enterPairingMode(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_enter_pairing_mode);
}

uint8_t IS2020::powerOnButtonPress(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_power_on_button_press);
}

uint8_t IS2020::powerOnButtonRelease(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_power_on_button_release);
}

uint8_t IS2020::eepromToDefaults(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_eeprom_to_defaults);
}

uint8_t IS2020::enterPairingModeFast(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_enter_pairing_mode_fast);
}

uint8_t IS2020::powerOff(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_power_off);
}

uint8_t IS2020::toggleBuzzer(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_toggle_buzzer);
}

uint8_t IS2020::disableBuzzer(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_disable_buzzer);
}

uint8_t IS2020::toggleBuzzer2(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_toggle_buzzer2);
}

uint8_t IS2020::changeTone(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_change_tone);
}

uint8_t IS2020::batteryStatus(uint8_t deviceID) {
  //Serial3.println("Battery_status");
  return IS2020::mmiAction(deviceID, MMI_battery_status);
}

uint8_t IS2020::exitPairingMode(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_exit_pairing_mode);
}

uint8_t IS2020::linkLastDevice(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_link_last_device);
}

uint8_t IS2020::disconnectAllLink(uint8_t deviceID) {
  return IS2020::mmiAction(deviceID, MMI_disconnect_all_link);
}
