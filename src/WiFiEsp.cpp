
#include "WiFiEsp.h"

#include "utility/EspDrv.h"


int16_t 	WiFiEspClass::_state[MAX_SOCK_NUM] = { NA_STATE, NA_STATE, NA_STATE, NA_STATE };
//uint16_t 	WiFiEspClass::_server_port[MAX_SOCK_NUM] = { 0, 0, 0, 0 };



WiFiEspClass::WiFiEspClass()
{

}

void WiFiEspClass::init(unsigned long baud)
{
    EspDrv::wifiDriverInit(baud);
}



char* WiFiEspClass::firmwareVersion()
{
	return EspDrv::getFwVersion();
}



int WiFiEspClass::begin(char* ssid)
{
	return WL_CONNECT_FAILED;
}

int WiFiEspClass::begin(char* ssid, uint8_t key_idx, const char *key)
{
	return WL_CONNECT_FAILED;
}

int WiFiEspClass::begin(char* ssid, const char *passphrase)
{
    if (EspDrv::wifiConnect(ssid, passphrase))
		return WL_CONNECTED;
	//{
	//	uint8_t status = EspDrv::getConnectionStatus();
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





void WiFiEspClass::config(IPAddress local_ip)
{
	//EspDrv::config(1, (uint32_t)local_ip, 0, 0);
}

void WiFiEspClass::config(IPAddress local_ip, IPAddress dns_server)
{
	//EspDrv::config(1, (uint32_t)local_ip, 0, 0);
	//EspDrv::setDNS(1, (uint32_t)dns_server, 0);
}

void WiFiEspClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
	//EspDrv::config(2, (uint32_t)local_ip, (uint32_t)gateway, 0);
	//EspDrv::setDNS(1, (uint32_t)dns_server, 0);
}

void WiFiEspClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
	//EspDrv::config(3, (uint32_t)local_ip, (uint32_t)gateway, (uint32_t)subnet);
	//EspDrv::setDNS(1, (uint32_t)dns_server, 0);
}

void WiFiEspClass::setDNS(IPAddress dns_server1)
{
	//EspDrv::setDNS(1, (uint32_t)dns_server1, 0);
}

void WiFiEspClass::setDNS(IPAddress dns_server1, IPAddress dns_server2)
{
	//EspDrv::setDNS(2, (uint32_t)dns_server1, (uint32_t)dns_server2);
}

int WiFiEspClass::disconnect()
{
    return EspDrv::disconnect();
}

uint8_t* WiFiEspClass::macAddress(uint8_t* mac)
{
	uint8_t* _mac = EspDrv::getMacAddress();
	memcpy(mac, _mac, WL_MAC_ADDR_LENGTH);
    return mac;
}
   
IPAddress WiFiEspClass::localIP()
{
	IPAddress ret;
	EspDrv::getIpAddress(ret);
	return ret;
}

IPAddress WiFiEspClass::subnetMask()
{
	IPAddress ret;
	EspDrv::getSubnetMask(ret);
	return ret;
}

IPAddress WiFiEspClass::gatewayIP()
{
	IPAddress ret;
	EspDrv::getGatewayIP(ret);
	return ret;
}

char* WiFiEspClass::SSID()
{
    return EspDrv::getCurrentSSID();
}


uint8_t* WiFiEspClass::BSSID(uint8_t* bssid)
{
	//uint8_t* _bssid = EspDrv::getCurrentBSSID();
	//memcpy(bssid, _bssid, WL_MAC_ADDR_LENGTH);
    return bssid;
}

int32_t WiFiEspClass::RSSI()
{
    return 0;
    //return EspDrv::getCurrentRSSI();
}


uint8_t WiFiEspClass::encryptionType()
{
    //return EspDrv::getCurrentEncryptionType();
	return 0;
}


int8_t WiFiEspClass::scanNetworks()
{
	uint8_t attempts = 10;
	uint8_t numOfNetworks = 0;

	if (EspDrv::startScanNetworks() == WL_FAILURE)
		return WL_FAILURE;
 	do
 	{
 		delay(2000);
 		numOfNetworks = EspDrv::getScanNetworks();
 	}
	while (( numOfNetworks == 0)&&(--attempts>0));
	return numOfNetworks;
}

char* WiFiEspClass::SSID(uint8_t networkItem)
{
	return EspDrv::getSSIDNetoworks(networkItem);
}

int32_t WiFiEspClass::RSSI(uint8_t networkItem)
{
	return EspDrv::getRSSINetoworks(networkItem);
}

uint8_t WiFiEspClass::encryptionType(uint8_t networkItem)
{
    return EspDrv::getEncTypeNetowrks(networkItem);
}

uint8_t WiFiEspClass::status()
{
	return EspDrv::getConnectionStatus();
}

int WiFiEspClass::hostByName(const char* aHostname, IPAddress& aResult)
{
	return EspDrv::getHostByName(aHostname, aResult);
}



WiFiEspClass WiFi;
