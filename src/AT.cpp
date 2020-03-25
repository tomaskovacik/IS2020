#include "IS2020.h"
#include <Arduino.h>
#include "AT.h"

/*
   send using this function:
   vendorAtCommand(uint8_t deviceId, uint16_t dataSize, char* data)
*/


/*
Select Phonebook Memory Storage +CPBS
Description:
This command selects phonebook memory storage. The available phonebooks are:
Values:
“SM”: ADN (SIM phonebook)
“FD”: FDN (SIM Fix Dialing, restricted phonebook)
“ON”: MSISDN (SIM own numbers)
“EN”: EN (SIM emergency number)
“LD”: LND (combined ME and SIM last dialing phonebook)
“MC”: MSD (ME missed calls list)
“ME”: ME (ME phonebook)
“MT”: MT (combined ME and SIM phonebook)
“RC”: LIC (ME received calls list)
“SN”: SDN (Services dialing phonebook)

AT+CPBS=”SM”
Note: Select ADN phonebook

AT+CPBS=?
Note: Possible values

AT+CPBS?
Note: Status
*/

// ATemergencyPB "EN" //SIM emergency number
// ATfixedDiallPB "FD" //SIM Fix Dialing, restricted phonebook, ??emergency numbers??
// ATlastDialledList "DC" //
// ATlastDialledCombinedList "LD" //combined ME and SIM last dialing phonebook
// ATmissedCallesList "MC" //ME missed calls list
// ATphoneBook "ME"  //ME phonebook
// ATcombinedBook "MT" //combined ME and SIM phonebook
// ATownnNumber "ON" //SIM own numbers
// ATreceivedCallsList "RC" //ME received calls list
// ATsimBook "SM" //SIM phonebook
// ATserviceDialBook "SN" //Services dialing phonebook

// ATSelectPhonebookMemoryStorage "+CPBS"

uint8_t IS2020::selectEmergencyPB(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATemergencyPB);
}

uint8_t IS2020::selectfixedDiallPB(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATfixedDiallPB);
}

uint8_t IS2020::selectlastDialledList(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATlastDialledList);
}

uint8_t IS2020::selectlastDialledCombinedList(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATlastDialledCombinedList);
}

uint8_t IS2020::selectmissedCallesList(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATmissedCallesList);
}

uint8_t IS2020::selectphoneBook(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATphoneBook);
}

uint8_t IS2020::selectcombinedBook(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATcombinedBook);
}

uint8_t IS2020::selectownnNumber(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATownnNumber);
}

uint8_t IS2020::selectreceivedCallsList(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATreceivedCallsList);
}

uint8_t IS2020::selectsimBook(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATsimBook);
}

uint8_t IS2020::selectserviceDialBook(uint8_t deviceId) {
  IS2020::setPhonebook(deviceId, ATserviceDialBook);
}

uint8_t IS2020::setPhonebook(uint8_t deviceId, char pb[2]) {
  /*
   * #define ATSelectPhonebookMemoryStorage "+CPBS"
   * AT+CPBS=”SM”
   * Note: Select ADN phonebooks
   */
  char tmp[4];
  strcpy(tmp,ATSelectPhonebookMemoryStorage);
  strcpy(tmp,"=");
  strcpy(tmp,pb);
  IS2020::sendATCPB(deviceId,tmp);
}

uint8_t IS2020::getAvailablePhonebooks(uint8_t deviceId) {
  /*
   * #define ATSelectPhonebookMemoryStorage "+CPBS"
   * AT+CPBS=?
   * Note: Possible values
   */
  char tmp[3];
  strcpy(tmp,ATSelectPhonebookMemoryStorage);
  strcat(tmp,"=?");
  IS2020::sendATCPB(deviceId,tmp);
}

uint8_t IS2020::getSelectedPhonebook(uint8_t deviceId) {
  /*
   * #define ATSelectPhonebookMemoryStorage "+CPBS"
   * AT+CPBS?
   * Note: Status
   */
  char data[6] = ATCommandPB;
  strcat(data,ATSelectPhonebookMemoryStorage);
  data[5] = '?';
  Serial.println(data);
  IS2020::vendorAtCommand(deviceId, data);
}

const char * IS2020::decodePB(PhoneBook pb) {
  switch (pb) {
    case EN: return "EN - SIM emergency number";
    case FD: return "FD - SIM Fix Dialing, restricted phonebook, ??emergency numbers??";
    case DC: return "DC - Last dialled list";
    case LD: return "LD - combined ME and SIM last dialing phonebook";
    case MC: return "MC - Missed calls list";
    case ME: return "ME - Phone phonebook";
    case MT: return "MT - combined ME and SIM phonebook";
    case ON: return "ON - SIM own numbers";
    case RC: return "RC - received calls list";
    case SM: return "SM - SIM phonebook";
    case SN: return "SN - Services dialing phonebook";
  }
  return false;
}

void IS2020::printSupportedPB(){
  for (uint8_t i=1;i<12;i++){
    if (bitRead(supportedPBs,i))
      Serial.println(decodePB((PhoneBook)i));
  }
}

void IS2020::printSelectedPB(){
  if (!selectedPB)
      Serial.println(decodePB((PhoneBook)selectedPB));
  else
      Serial.println("No selected PB!");
}

// ATFindPhonebookEntries "+CPBF"
uint8_t IS2020::findItemInPhonebook(uint8_t deviceId, char * text) {
  char tmp[8+strlen(text)];
  strcpy(tmp,ATCommandPB);
  strcat(tmp,ATFindPhonebookEntries);
  strcat(tmp,"=\"");
  strcat(tmp,text);
  strcat(tmp,"\"");
  IS2020::vendorAtCommand(deviceId, tmp);
}

uint8_t IS2020::readPhonebook(uint8_t deviceId, char * text) {
/*
 * ATReadPhonebookEntries "+CPBR" 
 * should be followed with number of entry or range divided by comma
 * returns +CPBR: PhoneBookEntry:
 * 
 * phoneNumber
 * NumberType(see next four defines)
 * ATnationalNumberType1 129 //return
 * ATnationalNumberType2 161
 * ATinternationalNumberType 145
 * ATnetworkSpecificNumer 177
 * Text(name)
 */
  char tmp[8+strlen(text)];
  strcpy(tmp,ATCommandPB);
  strcat(tmp,ATFindPhonebookEntries);
  strcat(tmp,"=\"");
  strcat(tmp,text);
  strcat(tmp,"\"");
  IS2020::vendorAtCommand(deviceId, tmp);
}


// ATWrite Phonebook Entry "+CPBW" //should be followed 3 entries "name",numberType(one of four type defined previously),text(name)
// ATdeletePhoneBookEntry "+CPBW" //should be followed with entry number
// ATPhonebookPhoneSearch "+CPBP"

/*
  Move Action in Phonebook
  Possible responses
  +CPBP: 15,”+331290101”,145,”Eric”
  OK
  Note: Display the entry corresponding to the specified phone number
  +CPBP: 15,”01290101”,129,”Eric”
  OK
  Note: Display the entry corresponding to the specified phone number
  +CPBP: 15,”+331290202”,145,”David”
  OK
  Note: Display the entry corresponding to the specified phone number
  +CPBP: 15,”+331290101”,145,”8045682344FFFF” (UCS2 format)
  OK
  Note: Display the entry corresponding to the specified phone number
  +CME ERROR: 22
  Note: Entry not found
  +CPBN
  Description:
  This specific command instructs the product to make a forward or backward move in the phonebook (in
  alphabetical order). This command is not allowed for the “EN” phonebook - which does not contain
  alphanumeric fields.
  Values:
  <mode>
  0: First item
  1: Last item
  2: Next valid item in alphabetical order
  3: Previous valid item in alphabetical order
  4: Last item read (usable only if a read operation has been performed on the current phonebook since the end of initialization (+WIND: 4))
  5: Last item written (usable only if a write operation has been performed on the current phonebook since the end of initialization (+WIND: 4))
  Syntax: AT+CPBN=<mode>
*/
// ATMoveActionInPB "+CPBN"

uint8_t IS2020::nextItemInPhonebook(uint8_t deviceId, char mode) {
  char tmp[8];
  strcpy(tmp,ATCommandPB);
  strcat(tmp,ATMoveActionInPB);
  strcat(tmp,"=");
  tmp[8]=mode;
  IS2020::vendorAtCommand(deviceId, tmp);
}

// ATManufacturerIdentification "+CGMI"
// ATRequestModelIdentification "+CGMM"
// ATRequestRevisionIdentification "+CGMR"
// ATSerialNumber "+CGSN"
// ATcharacterSet "+CSCS" //? give back supported character sets
// ATPhonebookCharacterSet "+WPCS"
// ATRequestIMSI "+CIMI"
// ATCardIdentification "+CCID"
// ATCapabilitiesList "+GCAP"
// ATRepeatLastCommand "A/"
// ATPowerOff "+CPOF"
// ATSetPhoneFunctionality "+CFUN"
// ATPhoneActivityStatus "+CPAS"
// ATPhoneStatusReady 0
// ATPhoneStatusUnavailable 1
// ATPhoneStatusUnknown 2
// ATPhoneStatusRinging 3
// ATPhoneStatusCall 4
// ATPhoneStatusAsleep 5
// ATReportMobileEquipmentErrors "+CMEE"
// ATKeypadControl "+CKPD"
// ATClockManagement "+CCLK"
// ATAlarmManagement "+CALA"
// ATDialCommand "D"
// ATHang-Up command "H"
// ATAnswer a Call "A"
// ATExtendedErrorReport "+CEER"
// ATRedialLastTelephoneNumber "DL"
// ATAutomaticAnswer "S0"
// ATMicrophoneMuteControl "+CMUT"
// ATSpeakerMicrophoneSelection "+SPEAKER"
// ATSubscriberNumber "+CNUM"
// ATNewMessageAcknowledgement "+CNMA"
// ATPreferredMessageStorage "+CPMS"
// ATPreferredMessageFormat "+CMGF"
// ATNewMessageIndication "+CNMI"
// ATReadMessage "+CMGR"
// ATListMessage "+CMGL"
// ATSendMessage "+CMGS"
// ATWriteMessagetoMemory "+CMGW"
// ATSendMessagefromStorage "+CMSS"
// ATSetTextModeParameters "+CSMP"
// ATDeleteMessage "+CMGD"
// ATReadOperatorName "+WOPN"
// ATPINRemainingAttemptNumber "+CPINC"
