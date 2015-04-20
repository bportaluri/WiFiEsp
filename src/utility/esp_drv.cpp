
#include "Arduino.h"


#include "esp_drv.h"                   


#include "utility/debug.h"


#define NUMTAGSS 5

char* TAGSS[] =
{
    "\r\nOK\r\n",
    "\r\nSEND OK\r\n",
    " CONNECT\r\n",
	"\r\nERROR\r\n",
	"\r\nFAIL\r\n"
};


// Array of data to cache the information related to the networks discovered
char 	_networkSsid[][WL_SSID_MAX_LENGTH] = {{"1"},{"2"},{"3"},{"4"},{"5"}};
int32_t _networkRssi[WL_NETWORKS_LIST_MAXNUM] = { 0 };
uint8_t _networkEncr[WL_NETWORKS_LIST_MAXNUM] = { 0 };

// Cached values of retrieved data
char 	_ssid[] = {0};
uint8_t	_bssid[] = {0};
uint8_t _mac[] = {0};
uint8_t _localIp[] = {0};
uint8_t _subnetMask[] = {0};
uint8_t _gatewayIp[] = {0};
// Firmware version
char    fwVersion[] = {0};




// Public Methods


EspDrv::EspDrv(Stream *espSerial, Stream *debugSerial, int resetPin)
{
	_espSerial = espSerial;
	_debugSerial = debugSerial;
	_resetPin = resetPin;
}


// Start server TCP on port specified
void EspDrv::startServer(uint16_t port, uint8_t sock, uint8_t protMode)
{
/*
	bool ret;
	char cmd[100];
	sprintf(cmd, "AT+CIPSERVER=1,%d", _port);
	
	_wifi->espSendCmd("AT+CIPMUX=1", "OK", 5000);
	_wifi->espSendCmd(cmd, "OK", 5000);
*/
}

// Start server TCP on port specified
void EspDrv::startClientIp(uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode)
{

}

// Start server TCP on port specified
bool EspDrv::startClient(const char* host, uint16_t port, uint8_t sock, uint8_t protMode)
{
	INFO1("Entering startClient");
	
	bool ret;
	char cmd[200];

	sprintf(cmd, "AT+CIPSTART=1,\"TCP\",\"%s\",%d", host, port);  // TODO must handle connection ID
	
	int r = sendCmd(cmd, 5000);
	if (r==TAG_OK)
		return true;
	
	return false;
}

// Start server TCP on port specified
void EspDrv::stopClient(uint8_t sock)
{
	INFO1("Entering stopClient");

	bool ret;
	char cmd[100];
	
	sprintf(cmd, "AT+CIPCLOSE=%d", 1);    
	
	int r = sendCmd(cmd, 2000);
}


uint8_t EspDrv::getServerState(uint8_t sock)
{
    return 0;
}

uint8_t EspDrv::getClientState(uint8_t sock)
{
	INFO1("getClientState");

	char buf[500];
	int bufLen;
	if(!sendCmd("AT+CIPSTATUS", "STATUS:", "\r\n", buf))
		return WL_NO_SHIELD;

	// TODO: handle other statuses
	int s = atoi(buf);
	if(s==3)
		return WL_CONNECTED;

    return WL_DISCONNECTED;
}

uint16_t EspDrv::availData(uint8_t sock)
{
    //INFO1(bufPos);
	if (bufPos>0)
		return bufPos;
	
	INFO1("2");
    
    int bytes=_espSerial->available();
	if (bytes)
	{
		INFO("There are %d bytes in the serial buffer", bytes);
		if (_espSerial->find("+IPD,"))
		{
			int i = _espSerial->parseInt();
			INFO("ConnID: %d", i);
			_espSerial->read();
			bufPos = _espSerial->parseInt();
			INFO("Bytes: %d", bufPos);
			return bufPos;
		}
	}
	return 0;
}

bool EspDrv::getData(uint8_t sock, uint8_t *data, uint8_t peek)
{
    //char c = (char)_espSerial->read();
	
	// see Serial.timedRead
	int c;
	long _startMillis = millis();
	do
	{
		c = _espSerial->read();
		if (c >= 0)
		{
			*data = (char)c;
			bufPos--;
			Serial.print((char)c);
			return true;
		}
	} while(millis() - _startMillis < 500);
	
    // timed out, reset the buffer
	INFO1("TIMEOUT");
    bufPos==0;
	*data = 0;
	return false;
}

bool EspDrv::getDataBuf(uint8_t sock, uint8_t *_data, uint16_t *_dataLen)
{
    return false;
}

bool EspDrv::insertDataBuf(uint8_t sock, const uint8_t *data, uint16_t _len)
{
    return false;
}

bool EspDrv::sendUdpData(uint8_t sock)
{
    return false;
}


bool EspDrv::sendData(uint8_t sock, const uint8_t *data, uint16_t len)
{
	//INFO("Entering sendData (%d): %s", len, data);
	
	bool ret;
	char cmd[100];
	sprintf(cmd, "AT+CIPSEND=1,%d", len);  // TODO must handle connection ID

	_espSerial->println(cmd);
	
	char c = (char)_espSerial->read();
	//INFO1(c);

	_espSerial->write(data, len);
	
	_espSerial->find("SEND OK");

	//readUntil(2000);
	
	//espGetOutput("SEND OK", 5000);
	//espGetOutput("\r\nOK", 8000);
	
    return true;
}


uint8_t EspDrv::checkDataSent(uint8_t sock)
{
    return 0;
}


void EspDrv::wifiDriverInit()
{
	INFO1("Entering wifiDriverInit");
	
	sendCmd("AT+RST", 1000);
	delay(3000);
	
	espEmptyBuf();

	// set AP mode
	sendCmd("AT+CWMODE=1", 2000);

	// set multiple connections mode
	sendCmd("AT+CIPMUX=1", 2000);
}

int8_t EspDrv::wifiSetNetwork(char* ssid)
{
	return 0;
}

bool EspDrv::wifiConnect(char* ssid, const char *passphrase)
{
	INFO1("Entering wifiConnect");
	
	bool ret;

	// connect to access point
	char cmd[100];
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, passphrase);

	if (sendCmd(cmd, 20000)==TAG_OK);
		return true;
	
    return false;
}


int8_t EspDrv::wifiSetKey(char* ssid, uint8_t key_idx, const void *key)
{
	return 0;
}

void EspDrv::config(uint8_t validParams, uint32_t local_ip, uint32_t gateway, uint32_t subnet)
{
}

void EspDrv::setDNS(uint8_t validParams, uint32_t dns_server1, uint32_t dns_server2)
{
}


                        
int8_t EspDrv::disconnect()
{
	INFO1("Entering disconnect");

	int ret = sendCmd("AT+CWQAP", 2000);
	if(ret==TAG_OK)
		return WL_DISCONNECTED;
		
	return WL_IDLE_STATUS;
}

uint8_t EspDrv::getConnectionStatus()
{
	INFO1("Entering getConnectionStatus");
	
	//if(!sendCmd("AT", 1000))
	//	return WL_NO_SHIELD;
	
	char buf[10];
	int bufLen;
	if(!sendCmd("AT+CIPSTATUS", "STATUS:", "\r\n", buf))
		return WL_NO_SHIELD;

	// TODO: other statuses?
	int s = atoi(buf);
	if(s==4)
		return WL_DISCONNECTED;
	else if(s==3 or s==5)         // TODO why CIPSTATUS returns 5 if connected ?
		return WL_CONNECTED;
	
	return WL_IDLE_STATUS;
}

uint8_t* EspDrv::getMacAddress()
{
	char macStr[20];
	
	//sendCmd("AT+CIPSTAMAC?",5000);
	
	if (sendCmd("AT+CIPAPMAC?", "AT+CIPAPMAC\r\r\n", "\r\n\r\nOK", macStr))
	{
		char* token;

		token = strtok(macStr, ":");
		_mac[0] = atoi(token);
		token = strtok(NULL, ":");
		_mac[1] = atoi(token);
		token = strtok(NULL, ":");
		_mac[2] = atoi(token);
		token = strtok(NULL, ":");
		_mac[3] = atoi(token);
		token = strtok(NULL, ":");
		_mac[4] = atoi(token);
		token = strtok(NULL, ":");
		_mac[5] = atoi(token);
	}
	return _mac;
}


void EspDrv::getIpAddress(IPAddress& ip)
{
	INFO1("Entering getIpAddress");
	
	char ipStr[20];

	if (sendCmd("AT+CIFSR", "AT+CIFSR\r\r\n", "\r\n\r\nOK", ipStr))
	{
		char* token;
		
		token = strtok(ipStr, ".");
		_localIp[0] = atoi(token);
		token = strtok(NULL, ".");
		_localIp[1] = atoi(token);
		token = strtok(NULL, ".");
		_localIp[2] = atoi(token);
		token = strtok(NULL, ".");
		_localIp[3] = atoi(token);

		ip = _localIp;
	}
}

 void EspDrv::getSubnetMask(IPAddress& mask)
 {
	//getNetworkData(_localIp, _subnetMask, _gatewayIp);
	//mask = _subnetMask;
 }

 void EspDrv::getGatewayIP(IPAddress& ip)
 {
	//getNetworkData(_localIp, _subnetMask, _gatewayIp);
	//ip = _gatewayIp;
 }

char* EspDrv::getCurrentSSID()
{
	INFO1("Entering getCurrentSSID");

	_ssid[0]=0;
	sendCmd("AT+CWJAP?", "+CWJAP:\"", "\"\r\n", _ssid);
	
	return _ssid;
}

uint8_t* EspDrv::getCurrentBSSID()
{

}

int32_t EspDrv::getCurrentRSSI()
{

}

uint8_t EspDrv::getCurrentEncryptionType()
{

}

int8_t EspDrv::startScanNetworks()
{
	return 0;
}


uint8_t EspDrv::getScanNetworks()
{
	return 0;
}

char* EspDrv::getSSIDNetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;

	return _networkSsid[networkItem];
}

uint8_t EspDrv::getEncTypeNetowrks(uint8_t networkItem)
{
}

int32_t EspDrv::getRSSINetoworks(uint8_t networkItem)
{
}

uint8_t EspDrv::reqHostByName(const char* aHostname)
{
}

int EspDrv::getHostByName(IPAddress& aResult)
{
}

int EspDrv::getHostByName(const char* aHostname, IPAddress& aResult)
{
/*
	uint8_t retry = 10;
	if (reqHostByName(aHostname))
	{
		while(!getHostByName(aResult) && --retry > 0)
		{
			delay(1000);
		}
	}else{
		return 0;
	}
	return (retry>0);
*/
}

char*EspDrv:: EspDrv::getFwVersion()
{
	INFO1("Entering getFwVersion");
	
	int _dataLen = 0;
	sendCmd("AT+GMR", "AT+GMR\r\r\n", "\r\n\r\nOK", fwVersion);
	
    return fwVersion;
}




//------------------------------------------------------------------------------



/*
* Sends the AT command and stops if any of the TAGS is found.
* Extract the string enclosed in the passed tags and returns it in the outStr buffer.
* Returns true if the string is extracted, false if tags are not found of timed out.
*/
bool EspDrv::sendCmd(char* cmd, char* startTag, char* endTag, char* outStr)
{
	String data_tmp;
	data_tmp = sendCmdRet(cmd, 6000);

	//INFO("len=%d", data_tmp.length());

    if (data_tmp.length() == 0)
		return false;

	int32_t index1 = data_tmp.indexOf(startTag);
	if (index1 != -1)
	{
		int32_t index2 = data_tmp.indexOf(endTag, index1+1);
		if (index2 != -1)
		{
			index1 += strlen(startTag);
			String ret = data_tmp.substring(index1, index2);
			ret.toCharArray(outStr, 50);

			INFO("Return: %s", outStr);
			INFO1("----------------------------------------------");
			INFO1();
			return true;
		}
	}	

	WARN("Error: TAGSS not found");
	INFO1("----------------------------------------------");
	INFO1();
	
	return false;
}


/*
* Sends the AT command and returns the id of the TAG.
* Return -1 if no tag is found.
*/
int EspDrv::sendCmd(char* cmd, int timeout)
{
    espEmptyBuf();

	INFO1("----------------------------------------------");
	INFO(">> %s", cmd);

	_espSerial->println(cmd);
	delay(100);

	// get result
    String data;
	char c;
    unsigned long start = millis();
	int ret = -1;
    	
	while ((millis() - start < timeout) and ret<0)
	{
        while(_espSerial->available())
		{
            c = (char)_espSerial->read();
			#ifdef _DEBUG_
			Serial.print(c);
			#endif
			data += c;
        }
		for(int i=0; i<NUMTAGSS; i++)
		{
			if (data==NULL)
				break;
			if (data.indexOf(TAGSS[i]) != -1)
			{
				ret = i;
				break;
			}
		}
    }
	
	INFO1();

	if (millis() - start > timeout)
		INFO1("TIMEOUT");
	
	INFO("Return: %d", ret);
	INFO1("----------------------------------------------");
	INFO1();

    return ret;
}


String EspDrv::sendCmdRet(char* cmd, int timeout)
{
    espEmptyBuf();

	INFO1("----------------------------------------------");
	INFO(">> %s", cmd);

	_espSerial->println(cmd);
	delay(100);

	// get result
    String data;
	char c;
    unsigned long start = millis();
	int ret = -1;
	
	while (millis() - start < timeout and ret<0)
	{
		while(_espSerial->available())
		{
            c = (char)_espSerial->read();
			#ifdef _DEBUG_
			Serial.print(c);
			#endif
			data += c;
        }
		for(int i=0; i<NUMTAGSS; i++)
		{
			if (data==NULL)
				break;
			if (data.indexOf(TAGSS[i]) != -1)
			{
				ret = i;
				break;
			}
		}
    }

	INFO1();
	
	if (millis() - start > timeout)
	{
		INFO1("TIMEOUT");
		return "";
	}
	
	INFO1("----------------------------------------------");
	INFO1();

    return data;
}



/*
* 
*/
int EspDrv::readUntil(int timeout)
{
    String data;
	char c;
    unsigned long start = millis();
	int ret = -1;
    	
	while ((millis() - start < timeout) and ret<0)
	{
        while(_espSerial->available())
		{
            c = (char)_espSerial->read();
			#ifdef _DEBUG_
			Serial.print(c);
			#endif
			data += c;
        }
		for(int i=0; i<NUMTAGSS; i++)
		{
			if (data==NULL)
				break;
			if (data.indexOf(TAGSS[i]) != -1)
			{
				ret = i;
				break;
			}
		}
    }
	
	INFO1();

	if (millis() - start > timeout)
		INFO1("TIMEOUT");
	
	INFO("Return: %d", ret);
	INFO1("----------------------------------------------");
	INFO1();

    return ret;
}




void EspDrv::espEmptyBuf() 
{
    int i=0;
	while(_espSerial->available() > 0)
    {
		_espSerial->read();
		i++;
	}
	if (i>0)
		INFO("xxxxxx %d", i);
}


