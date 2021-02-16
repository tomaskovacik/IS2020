#ifndef AT_h
#define AT_h

#include "IS2020.h"
#include <Arduino.h>

#ifdef PHONEBOOKSUPPORT

typedef enum {
  EN = 1,
  FD = 2,
  DC = 3,
  LD = 4,
  MC = 5,
  ME = 6,
  MT = 7,
  ON = 8,
  RC = 9,
  SM = 10,
  SN = 11
} PhoneBook;

#define ATemergencyPB "EN" //SIM emergency number
#define ATfixedDiallPB "FD" //SIM Fix Dialing, restricted phonebook, ??emergency numbers??
#define ATlastDialledList "DC" //
#define ATlastDialledCombinedList "LD" //combined ME and SIM last dialing phonebook
#define ATmissedCallesList "MC" //ME missed calls list
#define ATphoneBook "ME"  //ME phonebook
#define ATcombinedBook "MT" //combined ME and SIM phonebook
#define ATownnNumber "ON" //SIM own numbers
#define ATreceivedCallsList "RC" //ME received calls list
#define ATsimBook "SM" //SIM phonebook
#define ATserviceDialBook "SN" //Services dialing phonebook

#define ATCommandPB "+CPB"
#define ATSelectPhonebookMemoryStorage "S"
#define ATFindPhonebookEntries "F"
#define ATReadPhonebookEntries "R"//should be followed with number of entry or range divided by comma,returns +CPBR: PhoneBookEntry,phoneNumber,NumberType(see next four defines),Text(name)
#define ATnationalNumberType1 129 //return
#define ATnationalNumberType2 161
#define ATinternationalNumberType 145
#define ATnetworkSpecificNumer 177
#define ATWrite Phonebook Entry "W" //should be followed 3 entries "name",numberType(one of four type defined previously),text(name)
#define ATdeletePhoneBookEntry "W" //should be followed with entry number
#define ATPhonebookPhoneSearch "P"
#define ATMoveActionInPB "N="

#define ATCPIN "+CPIN"

#define ATCGM "+CGM"
#define ATManufacturerIdentification "+CGMI"
#define ATRequestModelIdentification "+CGMM"
#define ATRequestRevisionIdentification "+CGMR"
#define ATSerialNumber "+CGSN"
#define ATcharacterSet "+CSCS" //? give back supported character sets
#define ATPhonebookCharacterSet "+WPCS"
#define ATRequestIMSI "+CIMI"
#define ATCardIdentification "+CCID"
#define ATCapabilitiesList "+GCAP"
#define ATRepeatLastCommand "A/"
#define ATPowerOff "+CPOF"
#define ATSetPhoneFunctionality "+CFUN"
#define ATPhoneActivityStatus "+CPAS"
#define ATPhoneStatusReady 0
#define ATPhoneStatusUnavailable 1
#define ATPhoneStatusUnknown 2
#define ATPhoneStatusRinging 3
#define ATPhoneStatusCall 4
#define ATPhoneStatusAsleep 5
#define ATReportMobileEquipmentErrors "+CMEE"
#define ATKeypadControl "+CKPD"
#define ATClockManagement "+CCLK"
#define ATAlarmManagement "+CALA"
#define ATDialCommand "D"
#define ATHangUp command "H"
#define ATAnswer a Call "A"
#define ATExtendedErrorReport "+CEER"
#define ATRedialLastTelephoneNumber "DL"
#define ATAutomaticAnswer "S0"
#define ATMicrophoneMuteControl "+CMUT"
#define ATSpeakerMicrophoneSelection "+SPEAKER"
#define ATSubscriberNumber "+CNUM"
#define ATNewMessageAcknowledgement "+CNMA"
#define ATPreferredMessageStorage "+CPMS"
#define ATPreferredMessageFormat "+CMGF"
#define ATNewMessageIndication "+CNMI"
#define ATReadMessage "+CMGR"
#define ATListMessage "+CMGL"
#define ATSendMessage "+CMGS"
#define ATWriteMessagetoMemory "+CMGW"
#define ATSendMessagefromStorage "+CMSS"
#define ATSetTextModeParameters "+CSMP"
#define ATDeleteMessage "+CMGD"
#define ATReadOperatorName "+WOPN"
#define ATPINRemainingAttemptNumber "+CPINC"

#endif
#endif
