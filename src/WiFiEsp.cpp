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


int16_t 	WiFiEspClass::_state[MAX_SOCK_NUM] = { NA_STATE, NA_STATE, NA_STATE, NA_STATE };
uint16_t 	WiFiEspClass::_server_port[MAX_SOCK_NUM] = { 0, 0, 0, 0 };


uint8_t WiFiEspClass::espMode = 0;


WiFiEspClass::WiFiEspClass()
{

}

void WiFiEspClass::init(Stream *espSerial)
{
    LOGINFO(F("Initializing ESP module"));
	EspDrv::wifiDriverInit(espSerial);
}



char* WiFiEspClass::firmwareVersion()
{
	return EspDrv::getFwVersion();
}


int WiFiEspClass::begin(char* ssid, const char *passphrase)
{
    espMode = 1;
	if (EspDrv::wifiConnect(ssid, passphrase))
		return WL_CONNECTED;

	return WL_CONNECT_FAILED;
}


int WiFiEspClass::beginAP(char* ssid, const char* pwd, char channel, uint8_t enc)
{
	espMode = 2;
    if (EspDrv::wifiStartAP(ssid, pwd, channel, enc))
		return WL_CONNECTED;

	return WL_CONNECT_FAILED;
}



void WiFiEspClass::config(IPAddress local_ip)
{
	LOGERROR(F("Not implemented"));
	//EspDrv::config(1, (uint32_t)local_ip, 0, 0);
}


int WiFiEspClass::disconnect()
{
    return EspDrv::disconnect();
}

uint8_t* WiFiEspClass::macAddress(uint8_t* mac)
{
	// TODO we don't need _mac variable
	uint8_t* _mac = EspDrv::getMacAddress();
	memcpy(mac, _mac, WL_MAC_ADDR_LENGTH);
    return mac;
}

IPAddress WiFiEspClass::localIP()
{
	IPAddress ret;
	if(espMode==1)
		EspDrv::getIpAddress(ret);
	else
		EspDrv::getIpAddressAP(ret);
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


int8_t WiFiEspClass::scanNetworks()
{
	return EspDrv::getScanNetworks();
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



////////////////////////////////////////////////////////////////////////////
// Non standard methods
////////////////////////////////////////////////////////////////////////////

void WiFiEspClass::reset(void)
{
	EspDrv::reset();
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
*/


bool WiFiEspClass::ping(const char *host)
{
	return EspDrv::ping(host);
}


WiFiEspClass WiFi;
