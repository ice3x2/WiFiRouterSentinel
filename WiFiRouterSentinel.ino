#include <ESP8266WebServer.h>
#include <WiFi.h>
#include <EEPROM.h>

#include "DictionaryMap.hpp"
#define EEPROM_SIZE 1024
#define OPTION_DATA_HEADER 202207
#define OPTION_DATA_VER 1
#define OPTION_DATA_SSID_LEN = 33;
#define OPTION_DATA_AP_PASSWORD_LEN = 64;
#define OPTION_DATA_AP_PING_ADDR_LEN = 128;
#define OPTION_DATA_AP_EVENT_ADDR_LEN = 512;



typedef struct Option {
  String ssid = "unknown";
  String apPassword = "";
  String pingAddr = ;
  String eventAddr = ;
  long pingDelay;
  long pingTimeout;
  long wifiReconnectInterval;
  long wifiReconnectCount;
  
    
} Option;

Option _option;


void setup() {
  LittleFS.begin();
  EEPROM.begin(EEPROM_SIZE);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void saveConfig() {
  int address = 0;
  char* ssid = _option.ssid.c_str();
  char* apPassword =  _option.apPassword.c_str();
  char* pingAddr = _option.pingAddr.c_str();
  char* eventAddr =  _option.eventAddr.c_str();
  
  writeLongToEEPROM(address, OPTION_DATA_HEADER);
  address += 4;
  
  writeLongToEEPROM(address, OPTION_DATA_VER);
  address += 4;
  
  writeBufferToEEPROM(address, ssid, );
  address += 4;

  
  writeLongToEEPROM(address, DATA_VER);
  
}



long readLongFromEEPROM(long address) {
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);
  
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void writeLongToEEPROM(long address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}


void readBufferFromEEPROM(long address, char* buffer, int len) {
  for(int i = 0; i < len; ++i) {
    buffer[i] = EEPROM.read(address + i);
  }
}

void writeBufferToEEPROM(long address, char* buffer, int len) {
  for(int i = 0; i < len; ++i) {
    EEPROM.write(address + i, buffer[i]);
  }
}
