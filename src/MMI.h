#ifndef MMI_h
#define MMI_h

#include "IS2020.h"
#include <Arduino.h>

#define MMI_add_remove_SCO_link 0x01
#define MMI_force_end_active_call 0x02
#define MMI_accept_incoming_call 0x04
#define MMI_reject_incoming_call 0x05
#define MMI_end_call_transfer_to_headset 0x06 //1. End call if SCO exist. OR 2. Voice transfer to headset if SCO not exist."
#define MMI_toggle_mic_mute 0x07 //1. Mute microphone if microphone is not mute OR 2. Active microphone if microphone is mute"
#define MMI_mute_mic 0x08	//Mute microphone
#define MMI_unmute_mic 0x09	//Active microphone
#define MMI_voice_dial 0x0A
#define MMI_cancel_voice_dial 0x0Bl
#define MMI_last_number_redial 0x0C
#define MMI_toggle_active_hold_call 0x0D //Set the active call on hold and active the hold call
#define MMI_voice_transfer 0x0E
#define MMI_query_call_list_info 0x0F	//Query call list information(CLCC)
#define MMI_three_way_call 0x10
#define MMI_release_waiting_or_hold_call 0x11	//release the waiting call or on hold call
#define MMI_end_active_call_accept_waiting_or_held_call 0x12	//accept the waiting call or active the on hold call and release the active call
#define MMI_initiate_HF_connection 0x16
#define MMI_disconnect_HF_link 0x17	//disconnect HF link
//#ifdef BM62
#define MMI_enableRXnoiseReduction 0x18 //Enable RX noise reduction
#define MMI_disableRXnoiseReduction 0x19 //Disable RX noise reduction
#define MMI_switchRXnoiseReduction 0x1A //Switch RX noise reduction
#define MMI_enableTXnoiseReduction 0x1B //Enable TX noise reduction
#define MMI_disableTXnoiseReduction 0x1C //Disable TX noise reduction
#define MMI_switchTXnoiseReduction 0x1D //Switch TX noise reduction
#define MMI_enableAEC 0x1E //Enable AEC
#define MMI_disableAEC 0x1F //Disalbe AEC
#define MMI_switchAEC 0x20 //Switch AEC enable/disable
#define MMI_enableAECRXnoiseReduction 0x21 //Enable AEC RX noise reduction
#define MMI_disableAECRXnoiseReduction 0x22 //Disable AEC RX noise reduction
#define MMI_switchAECRXnoiseReduction 0x23 //Switch AEC RX noise reduction
//#endif
#define MMI_increase_microphone_gain 0x24
#define MMI_decrease_microphone_gain 0x25
#define MMI_switch_primary_secondary_HF 0x26	//switch primary HF device and secondary HF device role
#define MMI_increase_speaker_gain 0x30
#define MMI_decrease_speaker_gain 0x31
//#ifdef BM62
#define MMI_togglePlayPause 0x32 // Play/Pause
#define MMI_stop 0x33  //Stop
//#endif
#define MMI_nextSong 0x34
#define MMI_previousSong 0x35
//#ifdef BM62
#define MMI_FastForward 0x36 //Faseforward
#define MMI_Rewind 0x37 //Rewind
#define MMI_EQmodeUp 0x38 //EQ mode up
#define MMI_EQmodeDown 0x39 //EQ mode down
#define MMI_LockButton 0x3A //Lock button
//#endif
#define MMI_disconnect_A2DP_link 0x3B
#define MMI_next_audio_efect 0x3C
#define MMI_previous_audio_efect 0x3D
//#ifdef BM62
#define MMI_toggle3Deffect 0x3E  //Toggle 3D effect
#define MMI_currentEQmode 0x3F  //report current EQ mode
#define MMI_currentAudioEffectStatus 0x40  //report current audio effect status
//#endif
#define MMI_enter_pairing_mode 0x50	//enter pairing mode (from power off state)
#define MMI_power_on_button_press 0x51 //power on button press
#define MMI_power_on_button_release 0x52 //power on button release
//#ifdef BM62
#define MMI_powerOffButtonPress 0x53 //power off button press
#define MMI_powerOffButtonRelease 0x54  //power off button release
#define MMI_reversePanel 0x55 //Reverse panel
//#endif
#define MMI_eeprom_to_defaults 0x56	//Reset some eeprom setting to default setting
//#ifdef BM62
#define MMI_forceSpeakerGainToggle 0x57 //Force speaker gain toggle
#define MMI_toggleButtonIndication 0x58 //Toggle button indicatoin
#define MMI_combineFunction0 0x59 //Combine function 0
#define MMI_combineFunction1 0x5A //Combine function 1
#define MMI_combineFunction2 0x5B //Combine function 2
#define MMI_combineFunction3 0x5C //Combine function 3
//#endif
#define MMI_enter_pairing_mode_fast 0x5D	//fast enter pairing mode (from non-off mode)
#define MMI_power_off 0x5E	//switch power off: to execute the power_off process directly, actually, the combine command set, power off button press and release, could be replace by this command.
#define MMI_toggle_buzzer 0x60 //Enable buzzer if buzzer is OFF Disable buzzer if buzzer is ON
#define MMI_disable_buzzer 0x61
#define MMI_toggle_buzzer2 0x62	//"Enable buzzer Disable buzzer if buzzer is ON"
#define MMI_change_tone 0x63	//Change tone set (SPK module support two sets of tone)
//#ifdef BM62
#define MMI_retrievePhonebook 0x64 //Retrieve phonebook
#define MMI_retrieveMCH 0x65 //Retrieve MCH
#define MMI_retrieveICH 0x66 //Retrieve ICH
#define MMI_retrieveOCH 0x67 //Retrieve OCH
#define MMI_retrieveCCH 0x68 //Retrieve CCH
#define MMI_cancelAccessPBAP 0x69 //Cancel access PBAP
//#endif
#define MMI_battery_status 0x6A	//Indicate battery status
#define MMI_exit_pairing_mode 0x6B	//Exit pairing mode
#define MMI_link_last_device 0x6C	//link last device
#define MMI_disconnect_all_link 0x6D	//disconnect all link
//#ifder BM62
#define MMI_OHSevent1 0x6E //OHS event 1
#define MMI_OHSevent2 0x6F //OHS event 2
#define MMI_OHSevent3 0x70 //OHS event 3
#define MMI_OHSevent4 0x71 //OHS event 4
#define MMI_SHS_SEND_USER_DATA_1 0x72 //SHS_SEND_USER_DATA_1 (for embedded application mode)
#define MMI_SHS_SEND_USER_DATA_2 0x73 //SHS_SEND_USER_DATA_2 (for embedded application mode)
#define MMI_SHS_SEND_USER_DATA_3 0x74 //SHS_SEND_USER_DATA_3 (for embedded application mode)
#define MMI_SHS_SEND_USER_DATA_4 0x75 //SHS_SEND_USER_DATA_4 (for embedded application mode)
#define MMI_SHS_SEND_USER_DATA_5 0x76 //SHS_SEND_USER_DATA_5 (for embedded application mode)
#define MMI_currentRXnoiseReduction 0x77 //report current RX NR status
#define MMI_currentTXnoiseReduction 0x78 //report current TX NR status
#define MMI_forceBuzzerAlarm 0x79 //force buzzer alarm
#define MMI_cancelAllBTpaging 0x7A //Cancel all BT paging
#define MMI_triggerNSPKmaster 0xE0 //Trigger NSPK Master
#define MMI_triggerNSPKslav 0xE1 //Trigger NSPK Slave
#define MMI_NSPK_ONE_KEY_CONNECT_DISCONECT 0xE2 //NSPK one key connect/disconnect
#define MMI_CANCEL_NSPK_CREATION 0xE3 //Cancel NSPK creation
#define MMI_TERMINAL_NSPK_LINK 0xE4 //Terminate NSPK link
#define MMI_TERMINATE_CANCEL_NSPK_CONNECTION 0xE5 //Terminate / Cancel NSPK connection
#define MMI_NSPK_MASTER_ENTER_AUX_IN_441K_PCM_ENCODER_MODE 0xE6 //NSPK Master enter Aux-in 44.1K PCM Encoder mode
#define MMI_NSPK_MASTER_ENTER_AUX_IN_48K_PCM_ENCODER_MODE 0xE7 //NSPK Master enter Aux-in 48K PCM Encoder mode
#define MMI_NSPK_MASTER_EXIT_AUX_IN_PCM_ENCODER_MODE 0xE8 //NSPK Master exit Aux-in PCM Encoder mode
#define MMI_NSPK_MASTER_ENTER_AUX_IN_SBC_ENCODER_MDOE 0xE9 //NSPK Master enter Aux-in SBC Encoder mode
#define MMI_NSPK_MASTER_EXIT_AUX_IN_SBC_ENCODER_MODE 0xEA //NSPK Master exit Aux-in SBC Encoder mode
#define MMI_NSPK_DYNAMIC_CREATION 0xEB //NSPK dynamic creation
#define MMI_NSPK_SWITCH_CHANNEL 0xEC //NSPK switch channel
#define MMI_NSPK_POWER_OFF_ALL_SPEAKERS 0xED //NSPK power off all speakers
#define MMI_NSPK_AFH_SFC_ENCODING_AUDIOSYNC 0xEE //NSPK AFH SBCENCODING AUDIOSYNC
#define MMI_NSPK_MASTER_PAGE_SLAVE_FOR_NEW_SLAVE 0xF0 //NSPK MASTER PAGE SLAVE FOR NEW SLAVE
#define MMI_NSPK_SLAVE_ENABLE_PAGE_SCAN_FOR_NEW_MASTER 0xF1 //NSPK SLAVE ENABLE PAGE SCAN FOR NEW MASTER
#define MMI_NSPK_SLAVE_USE_SLOW_PAGE_SCAN 0xF2 //NSPK SLAVE USE SLOW PAGE SCAN
#define MMI_NSPK_SLAVE_USE_FAST_PAGE_SCAN 0xF3 //NSPK SLAVE USE FAST PAGE SCAN
#define MMI_NSPK_ENTER_NSPK_MODE 0xF4 //NSPK_ENTER_NSPK_MODE
#define MMI_NSPK_ENTER_BROADCAST_MODE 0xF5 //NSPK_ENTER_BROADCAST_MODE
//#endif

/*

Ex:
[+CLCC:<id1>,<dir>,<stat>,<mode>,<mpty>[,<number>,<type>[,<alpha>]][<CR><LF>+CLCC:<id2>,<dir>,<stat>,<mode>,<mpty>[,<number>,<type>[,<alpha>]][...]]]

idx : Call identification number
dir :
0:Mobile originated (MO) call
1:Mobile originated (MT) call
state: State of the call
0:ACTIVE
1:HELD
2: Dialing (MO)
3: Alerting (MO)
4: Incoming (MT)
5: Waiting (MT)
mode:
0: Voice
1: Data
2: Fax
3: Voice followed by data, voice mode
4: Alternating voice/data, voice mode
5: Alternating voice/fax, voice mode
6: Voice followed by data, data mode
7: Alternating voice/data, data mode
8: Alternating voice/fax, fax mode
9: Unknown
empty:
0:Call is not one of multiparty (conference) call parties
1:Call is one of multiparty (conference) call parties
*/

#endif
