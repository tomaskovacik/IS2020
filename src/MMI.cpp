#include "IS2020.h"
#include <Arduino.h>

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


  Note1:Query call list information(CLCC)
  action  SIZE: 1 BYTE
  Value Parameter Description
  0x0f  trigger SPK Module to query call list information


*/

void IS2020::decodeMMI(uint8_t mmiAction){
  switch(mmiAction){
    case MMI_add_remove_SCO_link:
    {
      DBG(F("Add remove SCO link."));
    }
    break;
    case MMI_force_end_active_call:
    {
      DBG(F("Force and active call."));
    }
    break;
    case MMI_accept_incoming_call:
    {
      DBG(F("Accept incoming call."));
    }
    break;
    case MMI_reject_incoming_call:
    {
      DBG(F("Reject incoming call."));
    }
    break;
    case MMI_end_call_transfer_to_headset:
    {
      DBG(F("End call transfer to headset."));
    }
    break;
    case MMI_toggle_mic_mute:
    {
      DBG(F("Toggle mic mute."));
    }
    break;
    case MMI_mute_mic:
    {
      DBG(F("Mute mic."));
    }
    break;
    case MMI_unmute_mic:
    {
      DBG(F("Unmute mic."));
    }
    break;
    case MMI_voice_dial:
    {
      DBG(F("Voice dial."));
    }
    break;
    case MMI_cancel_voice_dial:
    {
      DBG(F("Cancel voice dial."));
    }
    break;
    case MMI_last_number_redial:
    {
      DBG(F("Last number redial."));
    }
    break;
    case MMI_toggle_active_hold_call:
    {
      DBG(F("Toggle active hold call."));
    }
    break;
    case MMI_voice_transfer:
    {
      DBG(F("Voice transfer."));
    }
    break;
    case MMI_query_call_list_info:
    {
      DBG(F("Query call list info."));
    }
    break;
    case MMI_three_way_call:
    {
      DBG(F("Three way call."));
    }
    break;
    case MMI_release_waiting_or_hold_call:
    {
      DBG(F("Release waiting or hold call."));
    }
    break;
    case MMI_end_active_call_accept_waiting_or_held_call:
    {
      DBG(F("End active call accept waiting or held call."));
    }
    break;
    case MMI_initiate_HF_connection:
    {
      DBG(F("Initiate HF connection."));
    }
    break;
    case MMI_disconnect_HF_link:
    {
      DBG(F("Disconnect HF link."));
    }
    break;
    case MMI_increase_microphone_gain:
    {
      DBG(F("Increase microphone gain."));
    }
    break;
    case MMI_decrease_microphone_gain:
    {
      DBG(F("Decrease microphone gain."));
    }
    break;
    case MMI_switch_primary_secondary_HF:
    {
      DBG(F("Switch primary secondary HF"));
    }
    break;
    case MMI_increase_speaker_gain:
    {
      DBG(F("Increase speaker gain."));
    }
    break;
    case MMI_decrease_speaker_gain:
    {
      DBG(F("Decrease speaker gain."));
    }
    break;
    case MMI_next_song:
    {
      DBG(F("Next song."));
    }
    break;
    case MMI_previous_song:
    {
      DBG(F("Previous song."));
    }
    break;
    case MMI_disconnect_A2DP_link:
    {
      DBG(F("Disconnect A2DP link."));
    }
    break;
    case MMI_next_audio_efect:
    {
      DBG(F("Next audio efect."));
    }
    break;
    case MMI_previous_audio_efect:
    {
      DBG(F("Previous audio efect."));
    }
    break;
    case MMI_enter_pairing_mode:
    {
      DBG(F("Enter pairing mode."));
    }
    break;
    case MMI_power_on_button_press:
    {
      DBG(F("Power on button press."));
    }
    break;
    case MMI_power_on_button_release:
    {
      DBG(F("Power on button release."));
    }
    break;
    case MMI_eeprom_to_defaults:
    {
      DBG(F("Eeprom to defaults."));
    }
    break;
    case MMI_enter_pairing_mode_fast:
    {
      DBG(F("Enter pairing mode fast."));
    }
    break;
    case MMI_power_off:
    {
      DBG(F("Power off."));
    }
    break;
    case MMI_toggle_buzzer:
    {
      DBG(F("Toggle buzzer."));
    }
    break;
    case MMI_disable_buzzer:
    {
      DBG(F("Disable buzzer."));
    }
    break;
    case MMI_toggle_buzzer2:
    {
      DBG(F("Toggle buzzer2."));
    }
    break;
    case MMI_change_tone:
    {
      DBG(F("Change tone."));
    }
    break;
    case MMI_battery_status:
    {
      DBG(F("Battery status."));
    }
    break;
    case MMI_exit_pairing_mode:
    {
      DBG(F("Exit pairing mode."));
    }
    break;
    case MMI_link_last_device:
    {
      DBG(F("Link last device."));
    }
    break;
    case MMI_disconnect_all_link:
    {
      DBG(F("Disconnect all link."));
    }
    break;
  }
}

uint8_t IS2020::addRemoveScoLink(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_add_remove_SCO_link);
}

uint8_t IS2020::forceEndActiveCall(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_force_end_active_call);
}

uint8_t IS2020::acceptIncomingCall(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_accept_incoming_call);
}

uint8_t IS2020::rejectIncomingCall(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_reject_incoming_call);
}

uint8_t IS2020::endCallTransferToHeadset(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_end_call_transfer_to_headset);
}

uint8_t IS2020::toggleMicMute(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_toggle_mic_mute);
}

uint8_t IS2020::muteMic(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_mute_mic);
}

uint8_t IS2020::unmuteMic(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_unmute_mic);
}

uint8_t IS2020::voiceDial(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_voice_dial);
}

uint8_t IS2020::cancelVoiceDial(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_cancel_voice_dial);
}

uint8_t IS2020::lastNumberRedial(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_last_number_redial);
}

uint8_t IS2020::toggleActiveHoldCall(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_toggle_active_hold_call);
}

uint8_t IS2020::voiceTransfer(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_voice_transfer);
}

uint8_t IS2020::queryCallListInfo(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_query_call_list_info);
}

uint8_t IS2020::threeWayCall(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_three_way_call);
}

uint8_t IS2020::releaseWaitingOrHoldCall(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_release_waiting_or_hold_call);
}

uint8_t IS2020::endActiveCallAcceptWaitingOrHeldCall(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_end_active_call_accept_waiting_or_held_call);
}

uint8_t IS2020::initiateHfConnection(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_initiate_HF_connection);
}

uint8_t IS2020::disconnectHfLink(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_disconnect_HF_link);
}

uint8_t IS2020::increaseMicrophoneGain(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_increase_microphone_gain);
}

uint8_t IS2020::decreaseMicrophoneGain(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_decrease_microphone_gain);
}

uint8_t IS2020::switchPrimarySecondayHf(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_switch_primary_secondary_HF);
}

uint8_t IS2020::increaseSpeakerGain(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_increase_speaker_gain);
}

uint8_t IS2020::decreaseSpeakerGain(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_decrease_speaker_gain);
}

uint8_t IS2020::nextSong(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_next_song);
}

uint8_t IS2020::previousSong(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_previous_song);
}

uint8_t IS2020::disconnectA2DPLink(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_disconnect_A2DP_link);
}

uint8_t IS2020::nextAudioEfect(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_next_audio_efect);
}

uint8_t IS2020::previousAudioEfect(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_previous_audio_efect);
}

uint8_t IS2020::enterPairingMode(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_enter_pairing_mode);
}

uint8_t IS2020::powerOnButtonPress(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_power_on_button_press);
}

uint8_t IS2020::powerOnButtonRelease(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_power_on_button_release);
}

uint8_t IS2020::eepromToDefaults(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_eeprom_to_defaults);
}

uint8_t IS2020::enterPairingModeFast(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_enter_pairing_mode_fast);
}

uint8_t IS2020::powerOff(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_power_off);
}

uint8_t IS2020::toggleBuzzer(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_toggle_buzzer);
}

uint8_t IS2020::disableBuzzer(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_disable_buzzer);
}

uint8_t IS2020::toggleBuzzer2(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_toggle_buzzer2);
}

uint8_t IS2020::changeTone(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_change_tone);
}

uint8_t IS2020::batteryStatus(uint8_t deviceID){
  //Serial3.println("Battery_status");
      return IS2020::mmiAction(deviceID,MMI_battery_status);
}

uint8_t IS2020::exitPairingMode(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_exit_pairing_mode);
}

uint8_t IS2020::linkLastDevice(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_link_last_device);
}

uint8_t IS2020::disconnectAllLink(uint8_t deviceID){
      return IS2020::mmiAction(deviceID,MMI_disconnect_all_link);
}
