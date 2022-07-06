#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

#include "DictionaryMap.hpp"

String _ssid = "unknown";
String _apPassword = "";


void setup() {
  LittleFS.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void saveConfig() {
  
}


void loadConfig() {
  Serial.printf("Reading file: %s\r\n", path);
  File file = LittleFS.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return;
  }
  int len = file.size();
  char* buff = new char[len];
  for(int i = 0; i < len && file.available(); ++i) {
    buff[i] = file.read();
  }
  DictionaryMap configMap;
  configMap.parseFromQueryString(buff);
  char* ssid = configMap.get("ssid");
  char* passwd = configMap.get("passwd");
  char* pingAddr = configMap.get("pingAddr");
  char* pingInterval = configMap.get("pingDelay");
  char* pingTimeout = configMap.get("pingTimeout");
  char* wifiReconnectInterval = configMap.get("wifiReInterval");
  char* wifiReconnectCount = configMap.get("wifiReCount");

  if(ssid != unllptr) _ssid = String(ssid);
  if(passwd != unllptr) _ssid = String(ssid);
  
}
