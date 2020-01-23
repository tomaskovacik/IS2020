#ifndef IS2020_h
#define IS2020_h

#include <Arduino.h>
#include "MMI.h"
#include "Music.h"
#include "commands.h"
#include "events.h"
#include  "AVRCP.h"


#define STARTBYTE 0xAA

#define DUMMYBYTE 0x00
#define DEBUG 1
#define DEBUG_AVRCP 1

#define DEVICENAME_LENGHT_SUPPORT 24

//class SoftwareSerial;

class IS2020
{
  public:
    IS2020(HardwareSerial *ser);
    void begin(uint8_t _reset,uint32_t baudrate=115200);
    ~IS2020();


    uint8_t  makeCall(uint8_t deviceId, char phoneNumber[19]); //return event from BT will be handled elseware??? event call status
    uint8_t  makeExtensionCall(uint8_t deviceId, char phoneNumber[10]); ;
    uint8_t  mmiAction(uint8_t deviceId, uint8_t action);
    uint8_t  eventMaskSetting();
    uint8_t  musicControl(uint8_t deviceId, uint8_t action);
    uint8_t  changeDeviceName(String name);
    uint8_t  changePinCode();
    uint8_t  btmParameterSetting();
    uint8_t  readBtmVersion();
    uint8_t  getPbByAtCmd(uint8_t deviceId);
    uint8_t  vendorAtCommand();
    uint8_t  avrcpSpecificCmd();
    uint8_t  avrcpGroupNavigation();
    uint8_t  readLinkStatus();
    uint8_t  readPairedDeviceRecord();
    uint8_t  readLocalBtAddress();
    uint8_t  readLocalDeviceName();
    uint8_t  setAccessPbMethod();
    uint8_t  sendSppIapData();
    uint8_t  btmUtilityFunction();
    uint8_t  eventAck(uint8_t cmd);
    uint8_t  additionalProfilesLinkSetup();
    uint8_t  readLinkedDeviceInformation(uint8_t deviceId, uint8_t query);
    uint8_t  profileLinkBack(uint8_t type, uint8_t deviceId, uint8_t profile);
    uint8_t  connectLastDevice(); //alias profileLinkBack
    uint8_t  disconnect(uint8_t flag = 0x0F); //B0000 1111 = > 0x0F bit 3,2,1,0 set 
    uint8_t  mcuStatusIndication();
    uint8_t  userConfirmSppReqReply();
    uint8_t  setHfGainLevel();
    uint8_t  eqModeSetting();
    uint8_t  dspNrCtrl();
    uint8_t  gpioControl();
    uint8_t  mcuUartRxBufferSize();
    uint8_t  voicePromptCmd();
    uint8_t  mapRequest();
    uint8_t  securityBondingReq();
    uint8_t  setOverallGain();
    int      serialAvailable();
    int      serialRead();

    uint8_t checkResponce(uint8_t eventID);
    uint8_t getNextEventFromBt();
    uint8_t btmStatusChanged = 0; // set to 1 in EVT_BTM_Status, when this event happen main program shoud read array BTMStatus[]
    uint8_t callStatusChanged = 0; //set to 1 in case we are making/receiving call then check callStatus[]
    uint8_t btmState;
    uint8_t btmLinkInfo;
    uint8_t callStatus[2] = {0, 0}; //data_base_index, call_status
    uint8_t callerId[2][32];
    uint8_t smsStatus[2] = {0, 0}; //[0] - link 0 status, [1] link 1 status
    uint8_t missedCallStatus[2] = {0, 0};
    uint8_t maxBatteryLevel[2] = {0, 0};
    uint8_t currentBatteryLevel[2] = {0, 0};
    uint8_t roamingStatus[2] = {0, 0};
    uint8_t maxSignalLevel[2] = {0, 0};
    uint8_t currentSignalLevel[2] = {0, 0};
    uint8_t serviceStatus[2] = {0, 0};
    uint8_t btmBatteryStatus[2] = {0, 0};
    uint8_t btmChargingStatus = 0;
    uint8_t hfGainLevel[2] = {0, 0}; // this can be only 0x0F max, so we can bitbang device zero and one to one byte if we got to low mem state ...
    uint8_t eqMode = 0;
    uint8_t pbapAccessFinish = 0;
    String deviceName[2];
    uint8_t deviceInBandRingtone[2] = {0, 0}; //remake this for some bit masked variable .... this is just 0 or 1 for each device ..
    uint8_t deviceIsIap[2] = {0, 0};
    uint8_t deviceSupportAvrcpV13[2] = {0, 0} ; //reply if remote device support AVRCP v1.3
    uint8_t deviceHfAndA2dpGain[2] = {0, 0};
    uint8_t deviceLineInGain[2] = {0, 0};
    int     btmUartVersion = 0;
    int     btmFwVersion = 0;
    uint8_t linkStatus[7] = {0, 0, 0, 0, 0, 0, 0};

    uint8_t btAddress[8][6]={ //based on 
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0}}; //byte0 = link priority, 1-6 BT addr
    uint8_t numOfPairedDevices();
    String pairedDevicesRecords();

    uint8_t moduleBtAddress[6];
    void removeInfoAboutDevice(uint8_t deviceId);
    uint8_t musicState[2]={0,0};
    uint8_t pairingFailReason=0;

    //MMI specific functions
    uint8_t addRemoveScoLink(uint8_t deviceID);
    uint8_t forceEndActiveCall(uint8_t deviceID);
    uint8_t acceptIncomingCall(uint8_t deviceID);
    uint8_t rejectIncomingCall(uint8_t deviceID);
    uint8_t endCallTransferToHeadset(uint8_t deviceID);
    uint8_t toggleMicMute(uint8_t deviceID);
    uint8_t muteMic(uint8_t deviceID);
    uint8_t unmuteMic(uint8_t deviceID);
    uint8_t voiceDial(uint8_t deviceID);
    uint8_t cancelVoiceDial(uint8_t deviceID);
    uint8_t lastNumberRedial(uint8_t deviceID);
    uint8_t toggleActiveHoldCall(uint8_t deviceID);
    uint8_t voiceTransfer(uint8_t deviceID);
    uint8_t queryCallListInfo(uint8_t deviceID);
    uint8_t threeWayCall(uint8_t deviceID);
    uint8_t releaseWaitingOrHoldCall(uint8_t deviceID);
    uint8_t endActiveCallAcceptWaitingOrHeldCall(uint8_t deviceID);
    uint8_t initiateHfConnection(uint8_t deviceID);
    uint8_t disconnectHfLink(uint8_t deviceID);
    uint8_t increaseMicrophoneGain(uint8_t deviceID);
    uint8_t decreaseMicrophoneGain(uint8_t deviceID);
    uint8_t switchPrimarySecondayHf(uint8_t deviceID);
    uint8_t increaseSpeakerGain(uint8_t deviceID);
    uint8_t decreaseSpeakerGain(uint8_t deviceID);
    uint8_t nextSong(uint8_t deviceID);
    uint8_t previousSong(uint8_t deviceID);
    uint8_t disconnectA2DPLink(uint8_t deviceID);
    uint8_t nextAudioEfect(uint8_t deviceID);
    uint8_t previousAudioEfect(uint8_t deviceID);
    uint8_t enterPairingMode(uint8_t deviceID);
    uint8_t powerOnButtonPress(uint8_t deviceID);
    uint8_t powerOnButtonRelease(uint8_t deviceID);
    uint8_t eepromToDefaults(uint8_t deviceID);
    uint8_t enterPairingModeFast(uint8_t deviceID);
    uint8_t powerOff(uint8_t deviceID);
    uint8_t toggleBuzzer(uint8_t deviceID);
    uint8_t disableBuzzer(uint8_t deviceID);
    uint8_t toggleBuzzer2(uint8_t deviceID);
    uint8_t changeTone(uint8_t deviceID);
    uint8_t batteryStatus(uint8_t deviceID);
    uint8_t exitPairingMode(uint8_t deviceID);
    uint8_t linkLastDevice(uint8_t deviceID=0);
    uint8_t disconnectAllLink(uint8_t deviceID);
    uint8_t trigerToQueryCallListInfo(uint8_t deviceID);

    //Mucic control
    uint8_t stopFfwRwd(uint8_t deviceId);
    uint8_t ffw(uint8_t deviceId);
    uint8_t repFfw(uint8_t deviceId);
    uint8_t rwd(uint8_t deviceId);
    uint8_t repRwd(uint8_t deviceId);
    uint8_t play(uint8_t deviceId);
    uint8_t pause(uint8_t deviceId);
    uint8_t togglePlayPause(uint8_t deviceId);
    uint8_t stop(uint8_t deviceId);
    uint8_t stopFfwRwd();
    uint8_t ffw();
    uint8_t repFfw();
    uint8_t rwd();
    uint8_t repRwd();
    uint8_t play();
    uint8_t pause();
    uint8_t togglePlayPause();
    uint8_t stop();

    String moduleState();
    String btStatus();
    String musicStatus(uint8_t deviceID);
    String connectionStatus(uint8_t deviceId);
    String streamStatus(uint8_t deviceId);
    uint8_t batteryLevel(uint8_t deviceId);

    uint32_t EventMask = 0xFC0F0200;
    void setEventMask(uint32_t mask);
    void enableAllSettingEvent();

    String localDeviceName;

    //AVRCP
    void decodeAvrcpPdu(uint8_t pdu);
    void decodeAvrcpEvent(uint8_t Event);
    void decodeAvrcpPlayerAtributes(uint8_t attribute);
    void registerAllEvents(uint8_t deviceId);
    uint8_t  avrcpGetElementAttributesCommand(uint8_t deviceId);
    uint8_t  avrcpRegistrationForNotificationOfEvent(uint8_t deviceId, uint8_t event, uint8_t param1=0x00, uint8_t param2=0x00, uint8_t param3=0x00, uint8_t param4=0x00);
    uint8_t  avrcpListApplicationSettingsAttributesCommand(uint8_t deviceId);
    uint8_t  avrcpGetCapabilities(uint8_t deviceId, uint8_t capId = CAP_EVENTS_SUPPORTED);
    uint8_t  avrcpListPlayerAttributes(uint8_t deviceId);
    uint8_t  avrcpListPlayerValues(uint8_t deviceId, uint8_t attribute);
    uint8_t  avrcpGetCurrentPlayerValue(uint8_t deviceId, uint8_t attribute);
    uint8_t  avrcpSetPlayerValue(uint8_t deviceId, uint8_t attribute, uint8_t value);
    uint8_t  avrcpGetPlayerAttributeText(uint8_t deviceId,uint8_t attribute);
    uint8_t  avrcpGetPlayerValueText(uint8_t deviceId, uint8_t attribute, uint8_t setting);
    uint8_t  avrcpDisplayableCharset(uint8_t deviceId);
    uint8_t  avrcpGetElementAttributes(uint8_t deviceId);
    uint8_t  avrcpGetElementAttributesAll(uint8_t deviceId);
    uint8_t  avrcpGetPlayStatus(uint8_t deviceId);
    uint8_t  avrcpRequestContinuing(uint8_t deviceId, uint8_t pdu);
    uint8_t  avrcpAbortContinuing(uint8_t deviceId, uint8_t pdu);
    uint8_t  avrcpSetAbsoluteVolume(uint8_t deviceId, uint8_t volume);
    uint8_t  avrcpSetAddressedPlayer(uint8_t deviceId, uint16_t player);
    uint8_t  avrcpSetBrowsedPlayer(uint8_t deviceId, uint16_t player);
    uint8_t  avrcpGetFolderItems(uint8_t deviceId, uint8_t scope,uint32_t start, uint8_t end);
    uint8_t  avrcpChangePath(uint8_t deviceId, uint8_t direction=0x01, uint64_t folderUID=5);

    //reset module
    void resetModule();
    uint8_t queryDeviceName(uint8_t deviceId);
    uint8_t queryLineInGain(uint8_t deviceId);
    uint8_t queryHfA2dpGain(uint8_t deviceId);
    uint8_t queryIfRemoteDeviceSupportAvrcpV13(uint8_t deviceId);
    uint8_t queryIfRemoteDeviceIsIapDevice(uint8_t deviceId);
    uint8_t queryInBandRingtoneStatus(uint8_t deviceId);

  private:
    void decodeEvent(uint8_t Event);
    void decodeCommand(uint8_t cmd);
    void decodeMMI(uint8_t mmiAction);
    uint8_t _reset;
    void resetLow();
    void resetHigh();
    void sendPacketArrayInt(uint16_t packetSize, uint8_t cmd, uint8_t deviceId, uint8_t data[]);
    void sendPacketInt(uint8_t cmd, uint8_t data);
    void sendPacketString(uint8_t cmd, String str);
    void sendPacketArrayChar(uint16_t packetSize, uint8_t cmd, uint8_t deviceId, char data[]);
    uint8_t checkCkeckSum(int size, uint8_t data[]);
    void DBG(String text);
    void DBG_AVRCP(String text);
    HardwareSerial *btSerial;
};


#endif
