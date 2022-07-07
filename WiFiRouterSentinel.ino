#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>

#include "DictionaryMap.hpp"
#define EEPROM_SIZE 1024

#define OPTION_DATA_HEADER 202207
#define OPTION_DATA_VER 1

#define OPTION_DATA_LONG_LEN = 4;
#define OPTION_DATA_SSID_LEN = 33;
#define OPTION_DATA_AP_PASSWORD_LEN = 64;
#define OPTION_DATA_AP_EVENT_ADDR_LEN = 128;



typedef struct Option {
  String ssid = "unknown";
  String apPassword = "";
  long pingAddr;
  long pingDelay;
  long pingTimeout;
  long wifiReconnectInterval;
  long wifiReconnectCount;
  String eventAddr;

  Option() : ssid("unknown"), apPassword(""), pingAddr(134744072),pingDelay(60000),pingTimeout(15000),wifiReconnectInterval(15000),wifiReconnectCount(3), eventAddr("") {
    
  }

  void printLog() {
     Serial.print("SSID:" );
     Serial.println(ssid);
     Serial.print("apPassword:" );
     Serial.println(apPassword);
     Serial.print("pingAddr:" );
     Serial.println(pingAddr);
     Serial.print("pingDelay:" );
     Serial.println(pingDelay);
     Serial.print("pingTimeout:" );
     Serial.println(pingTimeout);
     Serial.print("wifiReconnectInterval:" );
     Serial.println(wifiReconnectInterval);
     Serial.print("wifiReconnectCount:" );
     Serial.println(wifiReconnectCount);
     Serial.print("eventAddr:" );
     Serial.println(eventAddr);  
  }
  
    
} Option;

Option _option;


void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  Serial.println("start");
  delay(3000);
  Serial.println("start");
  if(!loadConfig()) {
    saveConfig();
  } else {
    _option.printLog();
  }
  

  
}

void loop() {
  delay(1000);
  Serial.println("lopo");
  // put your main code here, to run repeatedly:

}

void saveConfig() {
  int address = 0;
  address = writeLongToEEPROM(address, OPTION_DATA_HEADER);
  address = writeLongToEEPROM(address, OPTION_DATA_VER);
  address = writeStringToEEPROM(address, &_option.ssid);
  address = writeStringToEEPROM(address, &_option.apPassword);
  address = writeLongToEEPROM(address, _option.pingAddr);
  address = writeLongToEEPROM(address, _option.pingDelay);
  address = writeLongToEEPROM(address, _option.pingTimeout);
  address = writeLongToEEPROM(address, _option.wifiReconnectInterval);
  address = writeLongToEEPROM(address, _option.wifiReconnectCount);
  writeStringToEEPROM(address, &_option.eventAddr);

  EEPROM.commit();
}

boolean loadConfig() {
  long value = 0;
  int address = 0;
  address = readLongFromEEPROM(address, &value);
    Serial.println(value);
  if(value != OPTION_DATA_HEADER) return false;
  address = readLongFromEEPROM(address, &value);
  if(value != OPTION_DATA_VER) return false;

  address = readStringFromEEPROM(address, &_option.ssid);
  address = readStringFromEEPROM(address, &_option.apPassword);
  address = readLongFromEEPROM(address, &_option.pingAddr);
  address = readLongFromEEPROM(address, &_option.pingDelay);
  address = readLongFromEEPROM(address, &_option.pingTimeout);
  address = readLongFromEEPROM(address, &_option.wifiReconnectInterval);
  address = readLongFromEEPROM(address, &_option.wifiReconnectCount);
  readStringFromEEPROM(address, &_option.eventAddr);

  return true;
}

int readLongFromEEPROM(long address, long* value) {

  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);
  
  *value = ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
  return address + 4;
}

int writeLongToEEPROM(int address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
  return address + 4;
}

int writeStringToEEPROM(int address, String* value) {
  long len = value->length();
  address = writeLongToEEPROM(address, len);
  for(int i = 0; i < len; ++i) {
    EEPROM.write(address, (*value)[i]);
    ++address;
  }
  return address;
}

int readStringFromEEPROM(int address,String* value) {
  long len;
  String tmp("");
  address = readLongFromEEPROM(address, &len);
  for(int i = 0; i < len; ++i) {
    tmp.concat((char)EEPROM.read(address));
    ++address;
  }
  *value = tmp; 
  return address;
}
