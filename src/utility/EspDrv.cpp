
#include <Arduino.h>
#include <avr/pgmspace.h>

#include "utility/EspDrv.h"
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
char 	EspDrv::_ssid[] = {0};
//uint8_t	_bssid[] = {0};
uint8_t EspDrv::_mac[] = {0};
uint8_t EspDrv::_localIp[] = {0};
//uint8_t _subnetMask[] = {0};
//uint8_t _gatewayIp[] = {0};
char EspDrv::fwVersion[] = {0};

long EspDrv::_bufPos=0;
uint8_t EspDrv::_connId=0;


char EspDrv::cmdBuf[] = {0};

char EspDrv::ringBuf[] = {0};
unsigned int EspDrv::ringBufPos = 0;


#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial EspDrv::Serial1(6, 7); // RX, TX
#endif



void EspDrv::wifiDriverInit(unsigned long baud)
{
	INFO2(F("> wifiDriverInit"), baud);
	
	Serial1.begin(baud);

	sendCmd(F("AT+RST"), 1000);
	delay(3000);
	espEmptyBuf(false);  // empty dirty characters from the buffer
	
	getFwVersion();
		
	if (strcmp(fwVersion, "0018000902-AI03")!=0)
	{
		Serial.print(F("Firmware version "));
		Serial.print(fwVersion);
		Serial.println(F(" is not tested"));
		delay(2000);
	}

	// set AP mode
	sendCmd(F("AT+CWMODE=1"));

	// set multiple connections mode
	sendCmd(F("AT+CIPMUX=1"));
}


// Start server TCP on port specified
void EspDrv::startServer(uint16_t port)
{
	INFO1(F("> startServer"));

	sprintf(cmdBuf, "AT+CIPSERVER=1,%d", port);
	
	sendCmd(cmdBuf);
}



bool EspDrv::startClient(const char* host, uint16_t port, uint8_t sock, uint8_t protMode)
{
	INFO1(F("> startClient"));
	
	bool ret;

	sprintf(cmdBuf, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", sock, host, port);
	
	int r = sendCmd(cmdBuf, 5000);
	if (r==TAG_OK)
		return true;
	
	return false;
}



// Start server TCP on port specified
void EspDrv::stopClient(uint8_t sock)
{
	INFO1(F("> stopClient"));

	bool ret;
	
	sprintf(cmdBuf, "AT+CIPCLOSE=%d", sock);    
	
	int r = sendCmd(cmdBuf, 4000);
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
	
    
    int bytes = Serial1.available();

	if (bytes)
	{
		//INFO2(F("Bytes in the serial buffer: "), bytes);
		if (Serial1.find("+IPD,"))
		{
			// format is : +IPD,<id>,<len>:<data>
			
			_connId = Serial1.parseInt();
			INFO2(F("New incoming connection - connId:"), _connId);
			
			Serial1.read();  // read the ',' character
			
			// ESP sends 21 more characters to acknowledge send request: \r\nOK\r\n\r\nOK\r\nUnlink\r\n

			//Serial.print((char)Serial1.read());
			//Serial.print((char)Serial1.read());
			//Serial.print((char)Serial1.read());
			//Serial.println((char)Serial1.read());
			//_bufPos = 10;
			
			_bufPos = Serial1.parseInt();
			INFO2(F("Bytes:"), _bufPos);
			
			Serial1.read();  // read the ':' character
			
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
		if (Serial1.available())
		{
			 *data = (char)Serial1.read();
			_bufPos--;
			//Serial.print((char)*data);
			
			// empty buffer if finished receiving data
			// based on the request the ESP is returning "\r\nOK\r\n" or "\r\nOK\r\n\r\nOK\r\nUnlink\r\n"
			if (_bufPos==0)
			{
				Serial.println();
				Serial.println(F(">>> Trailing data >>>"));
				int c;
				while( (c = timedRead()) > 0)
					Serial.print((char)c);
				Serial.println("<<<<<<<<<<<<<<<<<<<<<");
			}
			return true;
		}
	} while(millis() - _startMillis < 2000);
	
    // timed out, reset the buffer
	INFO2(F("TIMEOUT:"), _bufPos);
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
	//INFO("> sendData (%d, %d): %s", sock, len, data);
	INFO3(F("> sendData:"), sock, len);
	
	bool ret;
	sprintf(cmdBuf, "AT+CIPSEND=%d,%d", sock, len);

	Serial1.println(cmdBuf);

	if(!Serial1.find(">"))
	{
		INFO1("FAILED 1 !!!");
		return false;
	}

	Serial1.write(data, len);

	ret = Serial1.find("SEND OK\r\n");
	
	if(!ret)
	{
		INFO1("FAILED 2 !!!");
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
	sprintf(cmdBuf, "AT+CWJAP=\"%s\",\"%s\"", ssid, passphrase);

	if (sendCmd(cmdBuf, 20000)==TAG_OK);
	{
		return true;
	}
	
    return false;
}


int8_t EspDrv::wifiSetKey(char* ssid, uint8_t key_idx, const void *key)
{
	return 0;
}


int8_t EspDrv::disconnect()
{
	INFO1(F("> disconnect"));

	int ret = sendCmd(F("AT+CWQAP"));
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

	int s = atoi(buf);
	if(s==4)
		return WL_DISCONNECTED;
	else if(s==2 or s==3 or s==5)   // TODO why CIPSTATUS returns 5 if connected ?
		return WL_CONNECTED;
	
	return WL_IDLE_STATUS;
}


uint8_t* EspDrv::getMacAddress()
{
	memset(_mac, 0, WL_MAC_ADDR_LENGTH);
	
	if (sendCmd("AT+CIPAPMAC?", "AT+CIPAPMAC\r\r\n", "\r\n", cmdBuf, sizeof(cmdBuf)))
	{
		char* token;

		token = strtok(cmdBuf, ":");
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
	INFO1(F("> getIpAddress"));
	
	if (sendCmd("AT+CIFSR", "AT+CIFSR\r\r\n", "\r\n", cmdBuf, sizeof(cmdBuf)))
	{
		char* token;
		
		token = strtok(cmdBuf, ".");
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
	INFO1(F("> getCurrentSSID"));

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
	INFO2(F(">>"), cmd);

	Serial1.println(cmd);
	
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
int EspDrv::sendCmd(const __FlashStringHelper* cmd, int timeout)
{
    espEmptyBuf();

	INFO1(F("----------------------------------------------"));
	INFO2(F(">>"), cmd);

	Serial1.println(cmd);
	
	int i = readUntil(timeout);
	
	INFO1(F("----------------------------------------------"));
	INFO1();
	
    return i;
}


/*
* Sends the AT command and returns the id of the TAG.
* Return -1 if no tag is found.
*/
int EspDrv::sendCmd(const char* cmd, int timeout)
{
    espEmptyBuf();

	INFO1(F("----------------------------------------------"));
	INFO2(F(">>"), cmd);

	Serial1.println(cmd);
	
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
        if(Serial1.available())
		{
            c = (char)Serial1.read();
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



void EspDrv::espEmptyBuf(bool warn) 
{
    int i=0;
	while(Serial1.available() > 0)
    {
		Serial1.read();
		i++;
	}
	if (i>0 and warn==true)
		INFO2(F("Dirty characters in the serial buffer!!!! >"), i);
}


// copied from Serial::timedRead
int EspDrv::timedRead()
{
  int _timeout = 500;
  int c;
  long _startMillis = millis();
  do
  {
    c = Serial1.read();
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

EspDrv espDrv;
