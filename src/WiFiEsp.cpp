/*--------------------------------------------------------------------
This file is part of the Arduino WiFiEsp library.

The Arduino WiFiEsp library is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

The Arduino WiFiEsp library is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Arduino WiFiEsp library.  If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------*/

#include "WiFiEsp.h"

#include "utility/EspDrv.h"
#include "utility/debug.h"


int16_t 	WiFiEspClass::_state[MAX_SOCK_NUM] = { NA_STATE, NA_STATE, NA_STATE, NA_STATE };
//uint16_t 	WiFiEspClass::_server_port[MAX_SOCK_NUM] = { 0, 0, 0, 0 };



WiFiEspClass::WiFiEspClass()
{

}

void WiFiEspClass::init(unsigned long baud)
{
    LOGINFO(F("Initializing ESP module"));
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
	LOGERROR(F("Not implemented"));
	//EspDrv::config(1, (uint32_t)local_ip, 0, 0);
}

void WiFiEspClass::config(IPAddress local_ip, IPAddress dns_server)
{
	LOGERROR(F("Not implemented"));
	//EspDrv::config(1, (uint32_t)local_ip, 0, 0);
	//EspDrv::setDNS(1, (uint32_t)dns_server, 0);
}

void WiFiEspClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
	LOGERROR(F("Not implemented"));
	//EspDrv::config(2, (uint32_t)local_ip, (uint32_t)gateway, 0);
	//EspDrv::setDNS(1, (uint32_t)dns_server, 0);
}

void WiFiEspClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
	LOGERROR(F("Not implemented"));
	//EspDrv::config(3, (uint32_t)local_ip, (uint32_t)gateway, (uint32_t)subnet);
	//EspDrv::setDNS(1, (uint32_t)dns_server, 0);
}

void WiFiEspClass::setDNS(IPAddress dns_server1)
{
	LOGERROR(F("Not implemented"));
	//EspDrv::setDNS(1, (uint32_t)dns_server1, 0);
}

void WiFiEspClass::setDNS(IPAddress dns_server1, IPAddress dns_server2)
{
	LOGERROR(F("Not implemented"));
	//EspDrv::setDNS(2, (uint32_t)dns_server1, (uint32_t)dns_server2);
}

int WiFiEspClass::disconnect()
{
    return EspDrv::disconnect();
}

uint8_t* WiFiEspClass::macAddress(uint8_t* mac)
{
	// TODO we don't need _mac
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
	// TODO we don't need _bssid
	uint8_t* _bssid = EspDrv::getCurrentBSSID();
	memcpy(bssid, _bssid, WL_MAC_ADDR_LENGTH);
    return bssid;
}

int32_t WiFiEspClass::RSSI()
{
    return EspDrv::getCurrentRSSI();
}


uint8_t WiFiEspClass::encryptionType()
{
    LOGERROR(F("Not implemented"));
	return 0;
}


int8_t WiFiEspClass::scanNetworks()
{
	LOGERROR(F("Not implemented"));
	return 0;
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


////////////////////////////////////////////////////////////////////////////
// Non standard methods
////////////////////////////////////////////////////////////////////////////

bool WiFiEspClass::ping(const char *host)
{
	return EspDrv::ping(host);
}


WiFiEspClass WiFi;
