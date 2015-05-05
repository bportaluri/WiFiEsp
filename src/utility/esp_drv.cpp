
#include <Arduino.h>
#include <avr/pgmspace.h>

#include "esp_drv.h"                   


#include "utility/debug.h"


#define NUMESPTAGS 7

const char* ESPTAGS[] =
{
    "\r\nOK\r\n",
    "\r\nno change\r\n",
    "\r\nready\r\n",
    "\r\nSEND OK\r\n",
    " CONNECT\r\n",
	"\r\nERROR\r\n",
	"\r\nFAIL\r\n"
};

typedef enum
{
	TAG_OK = 0,
	TAG_NOCHANGE = 1,
	TAG_READY = 2,
	TAG_SENDOK = 3,
	TAG_CONNECT = 4,
	TAG_ERROR = 5,
	TAG_FAIL = 6
} TagsEnum;






// Array of data to cache the information related to the networks discovered
//char 	_networkSsid[][WL_SSID_MAX_LENGTH] = {{"1"},{"2"},{"3"},{"4"},{"5"}};
//int32_t _networkRssi[WL_NETWORKS_LIST_MAXNUM] = { 0 };
//uint8_t _networkEncr[WL_NETWORKS_LIST_MAXNUM] = { 0 };

// Cached values of retrieved data
//char 	_ssid[] = {0};
//uint8_t	_bssid[] = {0};
//uint8_t _mac[] = {0};
//uint8_t _localIp[] = {0};
//uint8_t _subnetMask[] = {0};
//uint8_t _gatewayIp[] = {0};
// Firmware version
char    fwVersion[] = {0};





// Public Methods


EspDrv::EspDrv(Stream *espSerial, Stream *debugSerial, int resetPin)
{
	_espSerial = espSerial;
	_debugSerial = debugSerial;
	_resetPin = resetPin;
}

void EspDrv::wifiDriverInit()
{
	INFO1(F("> wifiDriverInit"));
	
	sendCmd("AT+RST", 1000);
	delay(3000);
	
	// empty dirty characters from the buffer
	while(_espSerial->available() > 0) _espSerial->read();

	// set AP mode
	sendCmd("AT+CWMODE=1", 2000);

	// set multiple connections mode
	sendCmd("AT+CIPMUX=1", 2000);
}


// Start server TCP on port specified
void EspDrv::startServer(uint16_t port)
{
	INFO1(F("> startServer"));

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
	INFO1(F("> startClient"));
	
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
	INFO1(F("> stopClient"));

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
	INFO("> sendData (%d, %d): %s", sock, len, data);
	
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



int8_t EspDrv::wifiSetNetwork(char* ssid)
{
	return 0;
}

bool EspDrv::wifiConnect(char* ssid, const char *passphrase)
{
	INFO1(F("> wifiConnect"));
	
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
	INFO1(F("> disconnect"));

	int ret = sendCmd("AT+CWQAP", 2000);
	if(ret==TAG_OK)
		return WL_DISCONNECTED;
		
	return WL_IDLE_STATUS;
}

uint8_t EspDrv::getConnectionStatus()
{
	INFO1(F("> getConnectionStatus"));
	
	char buf[10];
	if(!sendCmd("AT+CIPSTATUS", "STATUS:", "\r\n", buf, sizeof(buf)))
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
	char buf[20];
	
	memset(_mac, 0, WL_MAC_ADDR_LENGTH);
	
	if (sendCmd("AT+CIPAPMAC?", "AT+CIPAPMAC\r\r\n", "\r\n\r\nOK", buf, sizeof(buf)))
	{
		char* token;

		token = strtok(buf, ":");
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
	INFO1("> getIpAddress");
	
	char buf[20];

	if (sendCmd("AT+CIFSR", "AT+CIFSR\r\r\n", "\r\n\r\nOK", buf, sizeof(buf)))  // Old firmware
	//if (sendCmd("AT+CIFSR", "+CIFSR:STAIP,\"", "\"\r\n", buf))  // New firmware
	{
		char* token;
		
		token = strtok(buf, ".");
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
	INFO1("> getCurrentSSID");

	_ssid[0]=0;
	sendCmd("AT+CWJAP?", "+CWJAP:\"", "\"\r\n", _ssid, sizeof(_ssid));
	
	return _ssid;
}


char* EspDrv::getFwVersion()
{
	INFO1(F("> getFwVersion"));

	fwVersion[0]=0;
	
	sendCmd("AT+GMR", "AT+GMR\r\r\n", "\r\n", fwVersion, sizeof(fwVersion));
	
    return fwVersion;
}




//------------------------------------------------------------------------------



/*
* Sends the AT command and stops if any of the TAGS is found.
* Extract the string enclosed in the passed tags and returns it in the outStr buffer.
* Returns true if the string is extracted, false if tags are not found of timed out.
*/
bool EspDrv::sendCmd(const char* cmd, const char* startTag, const char* endTag, char* outStr, int outStrLen)
{
    int idx;
	bool ret = false;
	
	outStr[0] = 0;
	
	espEmptyBuf();

	INFO1(F("----------------------------------------------"));
	INFO(">> %s", cmd);

	_espSerial->println(cmd);
	
	idx = readUntil(1000, startTag);
	//INFO("---------- 1: %d", idx);
	
	if(idx==NUMESPTAGS)
	{
		// start tag found
		idx = readUntil(1000, endTag);
		if(idx==NUMESPTAGS)
		{
			// end tag found
			//INFO("---------- 2: %d", idx);
			//INFO("ringBuf: (%d) '%s'", ringBufPos, ringBuf);
			
			ringBuf[ringBufPos-strlen(endTag)] = 0;
			strncpy(outStr, ringBuf, outStrLen);

			//INFO("outStr: '%s'", outStr);
			readUntil(1000, NULL);
			//INFO("outStr: '%s'", outStr);
			ret = true;
		}
		else
		{
			WARN(F("Error: TAG not found"));
		}
	}
	else if(idx>=0 and idx<NUMESPTAGS)
	{
		// the command has returned but no start tag is found
		WARN(F("Error: TAG not found"));
	}
	else
	{
		// the command has returned but no tag is found
		WARN(F("Error"));
	}

	INFO1(F("----------------------------------------------"));
	INFO1();
	
	//INFO("outStr: '%s'", outStr);
	
	return ret;
}


/*
* Sends the AT command and returns the id of the TAG.
* Return -1 if no tag is found.
*/
int EspDrv::sendCmd(const char* cmd, int timeout)
{
    espEmptyBuf();

	INFO1(F("----------------------------------------------"));
	INFO(">> %s", cmd);

	_espSerial->println(cmd);
	
	int i = readUntil(timeout);
	
	INFO1(F("----------------------------------------------"));
	INFO1();
	
    return i;
}


// Read from serial until one of the tags is found
int EspDrv::readUntil(int timeout, const char* findStr)
{
	ringBufInit();
	
	char c;
    unsigned long start = millis();
	int ret = -1;
    	
	while ((millis() - start < timeout) and ret<0)
	{
        if(_espSerial->available())
		{
            c = (char)_espSerial->read();
			#ifdef _DEBUG_
			Serial.print(c);
			#endif
			ringBufPutChar(c);
        
			if (findStr!=NULL)
			{
				if (ringBufFind(findStr))
				{
					ret = NUMESPTAGS;
					//INFO1("xxx");
				}
			}
			for(int i=0; i<NUMESPTAGS; i++)
			{
				if (ringBufFind(ESPTAGS[i]))
				{
					ret = i;
					//INFO1("yyy");
					break;
				}
			}
		}
    }


	if (millis() - start >= timeout)
	{
		INFO1(F("TIMEOUT >>>"));
	}
	
	ringBuf[ringBufPos] = 0;
	//INFO("Return: %d > '%s'", ret, ringBuf);
	
	
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
  do
  {
    c = _espSerial->read();
    if (c >= 0) return c;
  } while(millis() - _startMillis < _timeout);
  
  return -1; // -1 indicates timeout
}


//--------------------------------------------------------

void EspDrv::ringBufInit()
{
  ringBufPos = 0;
  memset(ringBuf, 0, RINGBUFLEN);
}

void EspDrv::ringBufPutChar(char c)
{
  ringBuf[ringBufPos%RINGBUFLEN] = c;
  ringBufPos++;
}


bool EspDrv::ringBufFind(const char* findStr)
{
  int findStrLen = strlen(findStr);
  
  if(ringBufPos < findStrLen)
	return false;

  unsigned int j = ringBufPos-findStrLen;
      
  for(unsigned int i=0; i<findStrLen; i++)
  {
	if(findStr[i] != ringBuf[(j+i)%RINGBUFLEN])
      return false;
  }
  
  return true;
}
