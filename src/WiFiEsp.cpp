
#include "utility/esp_drv.h"
#include "WiFiEsp.h"



WiFiEsp::WiFiEsp(Stream *espSerial, Stream *debugSerial, int resetPin)
{
	espDrv = new EspDrv(espSerial, debugSerial, resetPin);
}

void WiFiEsp::init()
{
    espDrv->wifiDriverInit();
}



char* WiFiEsp::firmwareVersion()
{
	return espDrv->getFwVersion();
}



int WiFiEsp::begin(char* ssid)
{
/*
	uint8_t status = WL_IDLE_STATUS;
	uint8_t attempts = WL_MAX_ATTEMPT_CONNECTION;

   if (espDrv->wifiSetNetwork(ssid, strlen(ssid)) != WL_FAILURE)
   {
	   do
	   {
		   delay(WL_DELAY_START_CONNECTION);
		   status = espDrv->getConnectionStatus();
	   }
	   while ((( status == WL_IDLE_STATUS)||(status == WL_SCAN_COMPLETED))&&(--attempts>0));
   }else
   {
	   status = WL_CONNECT_FAILED;
   }
   return status;
*/
	return WL_CONNECT_FAILED;
}

int WiFiEsp::begin(char* ssid, uint8_t key_idx, const char *key)
{
/*
	uint8_t status = WL_IDLE_STATUS;
	uint8_t attempts = WL_MAX_ATTEMPT_CONNECTION;

	// set encryption key
   if (espDrv->wifiSetKey(ssid, strlen(ssid), key_idx, key, strlen(key)) != WL_FAILURE)
   {
	   do
	   {
		   status = espDrv->getConnectionStatus();
	   } while ((( status == WL_IDLE_STATUS)||(status == WL_SCAN_COMPLETED))&&(--attempts>0));
   }else{
	   status = WL_CONNECT_FAILED;
   }
   return status;
*/
	return WL_CONNECT_FAILED;
}

int WiFiEsp::begin(char* ssid, const char *passphrase)
{
    if (espDrv->wifiConnect(ssid, passphrase))
		return WL_CONNECTED;
	//{
	//	uint8_t status = espDrv->getConnectionStatus();
	//	return status;
    //}
	
    return WL_CONNECT_FAILED;
}



/*

void ESP8266::hardReset(void)
{
connected = false;
strcpy(ip, "");
digitalWrite(ESP8266_RST, LOW);
delay(ESP8266_HARD_RESET_DURACTION);
digitalWrite(ESP8266_RST, HIGH);
delay(ESP8266_HARD_RESET_DURACTION);
}
void ESP8266::softReset(void)
{
connected = false;
strcpy(ip, "");
state = STATE_RESETING;
_wifiSerial.println(F("AT+RST"));
setResponseTrueKeywords(KEYWORD_READY);
setResponseFalseKeywords();
readResponse(15000, PostSoftReset);
}

*/





void WiFiEsp::config(IPAddress local_ip)
{
	//espDrv->config(1, (uint32_t)local_ip, 0, 0);
}

void WiFiEsp::config(IPAddress local_ip, IPAddress dns_server)
{
	//espDrv->config(1, (uint32_t)local_ip, 0, 0);
	//espDrv->setDNS(1, (uint32_t)dns_server, 0);
}

void WiFiEsp::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
	//espDrv->config(2, (uint32_t)local_ip, (uint32_t)gateway, 0);
	//espDrv->setDNS(1, (uint32_t)dns_server, 0);
}

void WiFiEsp::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
	//espDrv->config(3, (uint32_t)local_ip, (uint32_t)gateway, (uint32_t)subnet);
	//espDrv->setDNS(1, (uint32_t)dns_server, 0);
}

void WiFiEsp::setDNS(IPAddress dns_server1)
{
	//espDrv->setDNS(1, (uint32_t)dns_server1, 0);
}

void WiFiEsp::setDNS(IPAddress dns_server1, IPAddress dns_server2)
{
	//espDrv->setDNS(2, (uint32_t)dns_server1, (uint32_t)dns_server2);
}

int WiFiEsp::disconnect()
{
    return espDrv->disconnect();
}

uint8_t* WiFiEsp::macAddress(uint8_t* mac)
{
	uint8_t* _mac = espDrv->getMacAddress();
	memcpy(mac, _mac, WL_MAC_ADDR_LENGTH);
    return mac;
}
   
IPAddress WiFiEsp::localIP()
{
	IPAddress ret;
	espDrv->getIpAddress(ret);
	return ret;
}

IPAddress WiFiEsp::subnetMask()
{
	IPAddress ret;
	espDrv->getSubnetMask(ret);
	return ret;
}

IPAddress WiFiEsp::gatewayIP()
{
	IPAddress ret;
	espDrv->getGatewayIP(ret);
	return ret;
}

char* WiFiEsp::SSID()
{
    return espDrv->getCurrentSSID();
}

uint8_t* WiFiEsp::BSSID(uint8_t* bssid)
{
	uint8_t* _bssid = espDrv->getCurrentBSSID();
	memcpy(bssid, _bssid, WL_MAC_ADDR_LENGTH);
    return bssid;
}

int32_t WiFiEsp::RSSI()
{
    return espDrv->getCurrentRSSI();
}

uint8_t WiFiEsp::encryptionType()
{
    return espDrv->getCurrentEncryptionType();
}


int8_t WiFiEsp::scanNetworks()
{
	uint8_t attempts = 10;
	uint8_t numOfNetworks = 0;

	if (espDrv->startScanNetworks() == WL_FAILURE)
		return WL_FAILURE;
 	do
 	{
 		delay(2000);
 		numOfNetworks = espDrv->getScanNetworks();
 	}
	while (( numOfNetworks == 0)&&(--attempts>0));
	return numOfNetworks;
}

char* WiFiEsp::SSID(uint8_t networkItem)
{
	return espDrv->getSSIDNetoworks(networkItem);
}

int32_t WiFiEsp::RSSI(uint8_t networkItem)
{
	return espDrv->getRSSINetoworks(networkItem);
}

uint8_t WiFiEsp::encryptionType(uint8_t networkItem)
{
    return espDrv->getEncTypeNetowrks(networkItem);
}

uint8_t WiFiEsp::status()
{
    return espDrv->getConnectionStatus();
}

int WiFiEsp::hostByName(const char* aHostname, IPAddress& aResult)
{
	return espDrv->getHostByName(aHostname, aResult);
}



//WiFiEspClass WiFi;
