
#include "Arduino.h"


#include "esp_drv.h"                   

#include "utility/debug.h"


#define NUMTAGSS 6

char* TAGSS[] =
{
    "\r\nOK\r\n",
    "\r\nno change\r\n",
    "\r\nSEND OK\r\n",
    " CONNECT\r\n",
	"\r\nERROR\r\n",
	"\r\nFAIL\r\n"
};

typedef enum
{
	TAG_OK = 0,
	TAG_NOCHANGE = 1,
	TAG_SENDOK = 2,
	TAG_CONNECT = 3,
	TAG_ERROR = 4,
	TAG_FAIL = 5
} TagsEnum;


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
void EspDrv::startServer(uint16_t port)
{
	INFO ("Entering startServer (%d)", port);

	char cmd[100];
	sprintf(cmd, "AT+CIPSERVER=1,%d", port);
	
	sendCmd(cmd, 2000);
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
	char cmd[250];

	sprintf(cmd, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", sock, host, port);
	
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
	char cmd[20];
	
	sprintf(cmd, "AT+CIPCLOSE=%d", sock);    
	
	int r = sendCmd(cmd, 4000);
}


uint8_t EspDrv::getServerState(uint8_t sock)
{
    return 0;
}



uint16_t EspDrv::availData(uint8_t connId)
{
    //INFO1(bufPos);
	
	// if there is data in the buffer
	if (_bufPos>0)
	{
		if (_connId==connId)
			return _bufPos;
		else if (_connId==0)
			return _bufPos;
	}
	
    
    int bytes = _espSerial->available();

	if (bytes)
	{
		INFO("There are %d bytes in the serial buffer", bytes);
		if (_espSerial->find("+IPD,"))
		{
			// format is : +IPD,<id>,<len>:<data>
			
			_connId = _espSerial->parseInt();
			INFO("ConnID: %d", _connId);
			
			_espSerial->read();  // read the ',' character
			
			// ESP sends 21 more characters to acknowledge send request: \r\nOK\r\n\r\nOK\r\nUnlink\r\n

			//Serial.print((char)_espSerial->read());
			//Serial.print((char)_espSerial->read());
			//Serial.print((char)_espSerial->read());
			//Serial.println((char)_espSerial->read());
			//_bufPos = 10;
			
			_bufPos = _espSerial->parseInt();
			INFO("Bytes: %d", _bufPos);
			
			_espSerial->read();  // read the ':' character
			
			if(_connId==connId || connId==0)
				return _bufPos;
		}
	}
	return 0;
}


bool EspDrv::getData(uint8_t connId, uint8_t *data, uint8_t peek)
{
	if (connId!=_connId)
		return false;
	
	
	// see Serial.timedRead
	
	long _startMillis = millis();
	do
	{
		if (_espSerial->available())
		{
			 *data = (char)_espSerial->read();
			_bufPos--;
			//Serial.print((char)*data);
			
			// empty buffer
			// based on the request the ESP is returning "\r\nOK\r\n" or "\r\nOK\r\n\r\nOK\r\nUnlink\r\n"
			if (_bufPos==0)
			{
				Serial.println();
				Serial.println("--");
				int c;
				while( (c = timedRead()) > 0)
					Serial.print((char)c);
				Serial.println("--");
			}
			return true;
		}
	} while(millis() - _startMillis < 2000);
	
    // timed out, reset the buffer
	INFO("TIMEOUT (%d)", _bufPos);
    _bufPos = 0;
	_connId = 0;
	*data = 0;
	return false;
}


bool EspDrv::getDataBuf(uint8_t sock, uint8_t *_data, uint16_t *_dataLen)
{
    return false;
}

/*
bool EspDrv::insertDataBuf(uint8_t sock, const uint8_t *data, uint16_t _len)
{
    return false;
}

bool EspDrv::sendUdpData(uint8_t sock)
{
    return false;
}
*/

bool EspDrv::sendData(uint8_t sock, const uint8_t *data, uint16_t len)
{
	INFO("Entering sendData (%d, %d): %s", sock, len, data);
	
	bool ret;
	char cmd[100];
	sprintf(cmd, "AT+CIPSEND=%d,%d", sock, len);

	_espSerial->println(cmd);

	if(!_espSerial->find(">"))
	{
		//INFO1("FAILED 1 !!!");
		return false;
	}

	_espSerial->write(data, len);

	ret = _espSerial->find("SEND OK\r\n");
	
	if(!ret)
	{
		//INFO1("FAILED 2 !!!");
		return false;
	}

    return ret;
}


void EspDrv::wifiDriverInit()
{
	INFO1("Entering wifiDriverInit");
	
	sendCmd("AT+RST", 1000);
	delay(3000);
	
	// empty dirty characters from the buffer
	while(_espSerial->available() > 0) _espSerial->read();

	// set AP mode
	sendCmd("AT+CWMODE=1", 2000);
	//sendCmd("AT+CWMODE=2", 2000);

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

	if (sendCmd("AT+CIFSR", "AT+CIFSR\r\r\n", "\r\n\r\nOK", ipStr))  // Old firmware
	//if (sendCmd("AT+CIFSR", "+CIFSR:STAIP,\"", "\"\r\n", ipStr))  // New firmware
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



char* EspDrv::getCurrentSSID()
{
	INFO1("Entering getCurrentSSID");

	_ssid[0]=0;
	sendCmd("AT+CWJAP?", "+CWJAP:\"", "\"\r\n", _ssid);
	
	return _ssid;
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
	//delay(100);

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
	//delay(100);

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
		INFO("Dirty characters in the serial buffer!!!! > %d", i);
}


// copied from Serial::timedRead
int EspDrv::timedRead()
{
  int _timeout = 500;
  int c;
  long _startMillis = millis();
  do {
    c = _espSerial->read();
    if (c >= 0) return c;
  } while(millis() - _startMillis < _timeout);
  return -1; // -1 indicates timeout
}