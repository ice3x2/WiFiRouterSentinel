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

#define SETUP_PAGE "<html><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no'><title>WiFi Router Sentinel</title><style>body{background: #bbb;}label{width: 90px;text-align: right;display: inline-block; padding-right: 5px;}.input-ip{width: 50px; max-width: calc((100% - 100px) / 5)}input{width: 225px; max-width: calc(100% - 100px);}button{width: 200px; margin-left: 20px;}</style><body><h1>Setup</h1><br/><form action='' method='get'><input type='hidden' id='api' value='save'/><h3>WiFi</h3><div><label for='ssid'>SSID: </label><input type='text' name='ssid' value='%s'/></div><div><label for='password'>password: </label><input type='text' name='password' value='%s'/></div><div><label for='reconnectCount'>RE count: </label><input type='number' name='reconnectCount' value='%ld'/></div><div><label for='reconnectInterval'>RE interval: </label><input type='number' name='reconnectInterval' value='%ld'/></div><br/><h3>Ping</h3><div><label for='ip0'>IP Address: </label><input type='number' class='input-ip' id='ip0' value='%d'/>.<input type='number' class='input-ip' id='ip1' value='%d'/>.<input type='number' class='input-ip' id='ip2' value='%d'/>.<input type='number' class='input-ip' id='ip3' value='%d'/></div><div><label for='interval'>Interval: </label><input type='number' name='interval' value='%ld'/></div><div><label for='timeout'>Timeout: </label><input type='number' name='timeout' value='%ld'/></div><br/><h3>Callback</h3><div><label for='eventUrl'>Event url: </label><input type='text' name='eventUrl' value='%s'/></div><br/><br/> <button type='submit' >Save</button></form></body></html>"

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
bool _isSetupMode = false;
ESP8266WebServer* _webServer = NULL;

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

void releaseWebServer() {
    if(_webServer != NULL) {
        _webServer->close();
        _webServer->stop();
        delete _webServer;
        _webServer = NULL;
    }
}


void startSetupMode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("WiFiRouterSentinel", "");
  releaseWebServer();
  _webServer = new ESP8266WebServer(80);
    
  _webServer->on("/", HTTP_GET, [&]{ onHttpRequest(); });

  _webServer->begin();

  
}

void onHttpRequest() {
  String api = _webServer->arg("api");
  if(api == "save") {
    String ssid = _webServer->arg("ssid");
    Serial.println(ssid);
    String password = _webServer->arg("password");
    String reconnectCount = _webServer->arg("reconnectCount");
    String reconnectInterval = _webServer->arg("reconnectInterval");
    String ip0 = _webServer->arg("ip0");
    Serial.println(ip0);
    String ip1 = _webServer->arg("ip1");
    String ip2 = _webServer->arg("ip2");
    String ip3 = _webServer->arg("ip3");
    String interval = _webServer->arg("interval");
    String timeout = _webServer->arg("timeout");
    String eventUrl = _webServer->arg("eventUrl");
  }

  char buffer[3072];
  int ipAddress[4];
  numberToIpAddress(_option.pingAddr, ipAddress);
  sprintf(buffer, SETUP_PAGE, _option.ssid, _option.apPassword, _option.wifiReconnectCount, _option.wifiReconnectInterval, ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3],_option.pingDelay,_option.pingTimeout,_option.eventAddr);
  _webServer->send(200, "text/html", buffer); 
  
}


void loop() {
  if(!_isSetupMode) {
    startSetupMode();
    _isSetupMode =true;
  }
  if(_isSetupMode) {
    _webServer->handleClient();
  }

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

void numberToIpAddress(long addr, int* resultIP) {  
  resultIP[3] = (addr & 0xFF);
  resultIP[2] = ((addr >> 8) & 0xFF);
  resultIP[1] = ((addr >> 16) & 0xFF);
  resultIP[0] = ((addr >> 24) & 0xFF);
  
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
