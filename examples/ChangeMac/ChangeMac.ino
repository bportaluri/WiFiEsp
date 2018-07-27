#include "WiFiEsp.h"
#include "SoftwareSerial.h"
int status = WL_IDLE_STATUS;     // the Wifi radio's status

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module "for boards that like uno with only one serial interface, use software serial"
  Serial1.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial1);
  delay(1000);
  WiFi.setSTAmacAddress("18:fe:35:98:d3:7a",true); 
  printWifiData();
  WiFi.setSTAmacAddress("18:fe:35:98:d3:7b",true); 
  printWifiData();
  WiFi.setSTAmacAddress("18:fe:35:98:d3:7c",true); 
  printWifiData();
  WiFi.setSTAmacAddress("18:fe:35:98:d3:7d",true); 
  printWifiData();
  WiFi.setSTAmacAddress("18:fe:35:98:d3:7e",true); 
  printWifiData();
  WiFi.setSTAmacAddress("18:fe:35:98:d3:7f",true); 
  printWifiData();
  WiFi.setSTAmacAddress("18:fe:35:98:d3:70",true); 
  printWifiData();
  }

void loop() {}

void printWifiData()
{
  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);
}

