#include <Arduino.h>
#include <avr/pgmspace.h>

#include "utility/EspDrv.h"
#include "utility/debug.h"


#define NUMESPTAGS 5

const char* ESPTAGS[] =
{
    "\r\nOK\r\n",
	"\r\nERROR\r\n",
	"\r\nFAIL\r\n",
    "\r\nSEND OK\r\n",
    " CONNECT\r\n"
};

typedef enum
{
	TAG_OK,
	TAG_ERROR,
	TAG_FAIL,
	TAG_SENDOK,
	TAG_CONNECT
} TagsEnum;


Stream *EspDrv::espSerial;

RingBuffer EspDrv::ringBuf(30);


// Cached values of retrieved data
char EspDrv::_ssid[] = {0};
uint8_t EspDrv::_bssid[] = {0};
uint8_t EspDrv::_mac[] = {0};
uint8_t EspDrv::_localIp[] = {0};
char EspDrv::fwVersion[] = {0};

long EspDrv::_bufPos=0;
uint8_t EspDrv::_connId=0;


char EspDrv::cmdBuf[] = {0};



void EspDrv::wifiDriverInit(Stream *espSerial)
{
	LOGDEBUG(F("> wifiDriverInit"));
	
	//espSerial->begin(baud);
	EspDrv::espSerial = espSerial;

	reset();
}


void EspDrv::reset()
{
	LOGDEBUG(F("> reset"));

	sendCmd(F("AT+RST"), 1000);
	delay(3000);
	espEmptyBuf(false);  // empty dirty characters from the buffer
	
	// disable echo of commands
	sendCmd(F("ATE0"), 1000);
	
	// check firmware version
	getFwVersion();
	LOGDEBUG1(F("Firmware version"), fwVersion);
	
	// prints a warning message if the firmware is not 1.X
	if (fwVersion[0] != '1' or
		fwVersion[1] != '.')
	{
		LOGWARN1(F("Warning: Unsupported firmware"), fwVersion);
		delay(5000);
	}

	// set station mode
	sendCmd(F("AT+CWMODE=1"));

	// set multiple connections mode
	sendCmd(F("AT+CIPMUX=1"));
	
	// enable DHCP
	sendCmd(F("AT+CWDHCP=1,1"));
}



bool EspDrv::wifiConnect(char* ssid, const char *passphrase)
{
	LOGDEBUG(F("> wifiConnect"));

	// TODO
	// Escape character syntax is needed if "SSID" or "password" contains 
	// any special characters (',', '"' and '/')

	// connect to access point
	sprintf(cmdBuf, "AT+CWJAP_CUR=\"%s\",\"%s\"", ssid, passphrase);

	if (sendCmd(cmdBuf, 20000)==TAG_OK)
	{
		LOGINFO1(F("Connected to"), ssid);
		return true;
	}
	
	LOGWARN1(F("Failed to connected to"), ssid);
	
	// clean additional messages logged after the FAIL tag
	delay(1000);
	espEmptyBuf(false);
	
	return false;
}


bool EspDrv::wifiStartAP(char* ssid, const char* pwd, uint8_t channel, uint8_t enc)
{
	LOGDEBUG(F("> wifiStartAP"));

	// TODO
	// Escape character syntax is needed if "SSID" or "password" contains 
	// any special characters (',', '"' and '/')

	// set AP mode, use CUR mode to avoid starting the AP at reboot
	if (sendCmd(F("AT+CWMODE_CUR=2"))!=TAG_OK)
	{
		LOGWARN1(F("Failed to set AP mode"), ssid);
		return false;
	}
	
	// start access point
	sprintf(cmdBuf, "AT+CWSAP_CUR=\"%s\",\"%s\",%d,%d", ssid, pwd, channel, enc);

	if (sendCmd(cmdBuf, 10000)==TAG_OK)
	{
		LOGINFO1(F("Access point started"), ssid);
		return true;
	}
	
	LOGWARN1(F("Failed to start AP"), ssid);
	return false;
}


int8_t EspDrv::disconnect()
{
	LOGDEBUG(F("> disconnect"));

	if(sendCmd(F("AT+CWQAP"))==TAG_OK)
		return WL_DISCONNECTED;
	
	// wait and clear any additional message
	delay(2000);
	espEmptyBuf(false);
	
	return WL_IDLE_STATUS;
}

void EspDrv::config(uint32_t local_ip)
{
	LOGDEBUG(F("> config"));
	
	// TODO Not tested yet
	
	// disable DHCP
	// disable DHCP
	sendCmd(F("AT+CWDHCP_DEF=0,0"));
	
	sprintf(cmdBuf, "AT+CIPSTA_CUR=\"%s\"", local_ip);

	if (sendCmd(cmdBuf, 5000)==TAG_OK)
	{
		LOGINFO1(F("IP address successfully set"), local_ip);
	}

}

uint8_t EspDrv::getConnectionStatus()
{
	LOGDEBUG(F("> getConnectionStatus"));
	
/*
	AT+CIPSTATUS

	Response

		STATUS:<stat> 
		+CIPSTATUS:<link ID>,<type>,<remote_IP>,<remote_port>,<local_port>,<tetype>

	Parameters

		<stat> 
			2: Got IP 
			3: Connected 
			4: Disconnected 
		<link ID> ID of the connection (0~4), for multi-connect 
		<type> string, "TCP" or "UDP" 
		<remote_IP> string, remote IP address. 
		<remote_port> remote port number 
		<local_port> ESP8266 local port number 
		<tetype> 
			0: ESP8266 runs as client 
			1: ESP8266 runs as server 
*/

	char buf[10];
	if(!sendCmd(F("AT+CIPSTATUS"), "STATUS:", "\r\n", buf, sizeof(buf)))
		return WL_NO_SHIELD;

	int s = atoi(buf);
	if(s>=4)
		return WL_DISCONNECTED;
	else if(s==2 or s==3)
		return WL_CONNECTED;

	return WL_IDLE_STATUS;
}

uint8_t EspDrv::getClientState(uint8_t sock)
{
	LOGDEBUG1(F("> getClientState"), sock);

	char buf[10];
	char buf2[20];

	sprintf(buf2, "+CIPSTATUS:%d,", sock);

	if (sendCmd(F("AT+CIPSTATUS"), buf2, ",", buf, sizeof(buf)))
	{
		LOGDEBUG(F("Connected"));
		return true;
	}

	LOGDEBUG(F("Not connected"));
	return false;
}

uint8_t* EspDrv::getMacAddress()
{
	LOGDEBUG(F("> getMacAddress"));

	memset(_mac, 0, WL_MAC_ADDR_LENGTH);
	
	char buf[20];
	if (sendCmd(F("AT+CIFSR"), ":STAMAC,\"", "\"", buf, sizeof(buf)))
	{
		char* token;

		token = strtok(buf, ":");
		_mac[5] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_mac[4] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_mac[3] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_mac[2] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_mac[1] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_mac[0] = (byte)strtol(token, NULL, 16);
	}
	return _mac;
}


void EspDrv::getIpAddress(IPAddress& ip)
{
	LOGDEBUG(F("> getIpAddress"));
	
	// AT+CIFSR or AT+CIPSTA?
	char buf[20];
	if (sendCmd(F("AT+CIFSR"), ":STAIP,\"", "\"", buf, sizeof(buf)))
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

void EspDrv::getIpAddressAP(IPAddress& ip)
{
	LOGDEBUG(F("> getIpAddressAP"));
	
	char buf[20];
	if (sendCmd(F("AT+CIPAP?"), "+CIPAP:ip:\"", "\"", buf, sizeof(buf)))
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
	LOGDEBUG(F("> getCurrentSSID"));

	_ssid[0] = 0;
	sendCmd(F("AT+CWJAP?"), "+CWJAP:\"", "\"", _ssid, sizeof(_ssid));
	
	return _ssid;
}

uint8_t* EspDrv::getCurrentBSSID()
{
	LOGDEBUG(F("> getCurrentBSSID"));

	memset(_bssid, 0, WL_MAC_ADDR_LENGTH);

	char buf[20];
	if (sendCmd(F("AT+CWJAP?"), ",\"", "\",", buf, sizeof(buf)))
	{
		char* token;

		token = strtok(buf, ":");
		_bssid[5] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_bssid[4] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_bssid[3] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_bssid[2] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_bssid[1] = (byte)strtol(token, NULL, 16);
		token = strtok(NULL, ":");
		_bssid[0] = (byte)strtol(token, NULL, 16);
	}
	return _bssid;

}

int32_t EspDrv::getCurrentRSSI()
{
	LOGDEBUG(F("> getCurrentRSSI"));

    int ret=0;
	char buf[10];
	sendCmd(F("AT+CWJAP?"), ",-", "\r\n", buf, sizeof(buf));
	
	if (isDigit(buf[0])) {
      ret = -atoi(buf);
    }

    return ret;
}


char* EspDrv::getFwVersion()
{
	LOGDEBUG(F("> getFwVersion"));

	fwVersion[0] = 0;
	
	sendCmd(F("AT+GMR"), "SDK version:", "\r\n", fwVersion, sizeof(fwVersion));

    return fwVersion;
}



bool EspDrv::ping(const char *host)
{
	LOGDEBUG(F("> ping"));
	
	sprintf(cmdBuf, "AT+PING=\"%s\"", host);

	if (sendCmd(cmdBuf, 2000)==TAG_OK);
	{
		return true;
	}
	
    return false;
}



// Start server TCP on port specified
void EspDrv::startServer(uint16_t port)
{
	LOGDEBUG1(F("> startServer"), port);
	
	sprintf(cmdBuf, "AT+CIPSERVER=1,%d", port);
	
	sendCmd(cmdBuf);
}


bool EspDrv::startClient(const char* host, uint16_t port, uint8_t sock, uint8_t protMode)
{
	LOGDEBUG2(F("> startClient"), host, port);
	
	bool ret;

	sprintf(cmdBuf, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", sock, host, port);
	
	if (sendCmd(cmdBuf, 5000)==TAG_OK)
		return true;
	
	return false;
}


// Start server TCP on port specified
void EspDrv::stopClient(uint8_t sock)
{
	LOGDEBUG1(F("> stopClient"), sock);

	bool ret;
	
	sprintf(cmdBuf, "AT+CIPCLOSE=%d", sock);    
	
	int r = sendCmd(cmdBuf, 4000);
}


uint8_t EspDrv::getServerState(uint8_t sock)
{
    return 0;
}



////////////////////////////////////////////////////////////////////////////
// TCP/IP functions
////////////////////////////////////////////////////////////////////////////



uint16_t EspDrv::availData(uint8_t connId)
{
    //LOGDEBUG(bufPos);
	
	// if there is data in the buffer
	if (_bufPos>0)
	{
		if (_connId==connId)
			return _bufPos;
		else if (_connId==0)
			return _bufPos;
	}
	
    
    int bytes = espSerial->available();

	if (bytes)
	{
		//LOGDEBUG1(F("Bytes in the serial buffer: "), bytes);
		if (espSerial->find((char *)"+IPD,"))
		{
			// format is : +IPD,<id>,<len>:<data>
			
			_connId = espSerial->parseInt();
			espSerial->read();  // read the ',' character
			_bufPos = espSerial->parseInt();
			espSerial->read();  // read the ':' character
			
			LOGDEBUG2(F("Data packet"), _connId, _bufPos);
			
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
		if (espSerial->available())
		{
			 *data = (char)espSerial->read();
			_bufPos--;
			//Serial.print((char)*data);

/*
			// empty buffer if finished receiving data
			// based on the request the ESP is returning "\r\nOK\r\n" or "\r\nOK\r\n\r\nOK\r\nUnlink\r\n"
			if (_bufPos==0)
			{
				LOGDEBUG();
				LOGDEBUG(F(">>> Trailing data >>>"));
				int c;
				while( (c = timedRead()) > 0)
					LOGDEBUG0((char)c);
				LOGDEBUG("<<<<<<<<<<<<<<<<<<<<<");
			}
*/
			return true;
		}
	} while(millis() - _startMillis < 2000);
	
    // timed out, reset the buffer
	LOGDEBUG1(F("TIMEOUT:"), _bufPos);
	
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
	LOGDEBUG2(F("> sendData:"), sock, len);
	
	sprintf(cmdBuf, "AT+CIPSEND=%d,%d", sock, len);

	espSerial->println(cmdBuf);

	//if(!espSerial->find((char *)">"))
	int idx = readUntil(1000, (char *)">", false);
	if(idx!=NUMESPTAGS)
	{
		LOGERROR(F("Data packet send error (1)"));
		return false;
	}

	//LOGDEBUG(F("Sending data"));
	espSerial->write(data, len);

//if(!espSerial->find((char *)"\r\nSEND OK\r\n"))

	idx = readUntil(2000);
	if(idx!=TAG_SENDOK)
	{
		LOGERROR(F("Data packet send error (2)"));
		return false;
	}

    return true;
}


////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////



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

	LOGDEBUG(F("----------------------------------------------"));
	LOGDEBUG1(F(">>"), cmd);

	// send AT command to ESP
	espSerial->println(cmd);
	
	// read result until the startTag is found
	idx = readUntil(1000, startTag);
	
	if(idx==NUMESPTAGS)
	{
		// start tag found, serch the endTag
		idx = readUntil(500, endTag);
		
		if(idx==NUMESPTAGS)
		{
			// end tag found
			
			// copy the result to the output buffer
			//ringBuf[ringBufPos-strlen(endTag)] = 0;
			//strncpy(outStr, ringBuf, outStrLen);
			
			ringBuf.getStr(outStr, ringBuf.getPos()-strlen(endTag));
			
			//LOGINFO1("outStr: '", outStr);
			
			// lead the remaining part of the response
			readUntil(2000);
			//LOGINFO("outStr: '%s'", outStr);
			
			ret = true;
		}
		else
		{
			LOGWARN(F("End tag not found"));
		}
	}
	else if(idx>=0 and idx<NUMESPTAGS)
	{
		// the command has returned but no start tag is found
		LOGDEBUG1(F("No start tag found:"), idx);
	}
	else
	{
		// the command has returned but no tag is found
		LOGWARN(F("No tag found"));
	}

	LOGDEBUG1(F("---------------------------------------------- >"), outStr);
	LOGDEBUG();
	
	//INFO("outStr: '%s'", outStr);
	
	return ret;
}

bool EspDrv::sendCmd(const __FlashStringHelper* cmd, const char* startTag, const char* endTag, char* outStr, int outStrLen)
{
	strcpy_P(cmdBuf, (char*)cmd);
	return sendCmd(cmdBuf, startTag, endTag, outStr, outStrLen);
}



/*
* Sends the AT command and returns the id of the TAG.
* Return -1 if no tag is found.
*/
int EspDrv::sendCmd(const char* cmd, int timeout)
{
    espEmptyBuf();

	LOGDEBUG(F("----------------------------------------------"));
	LOGDEBUG1(F(">>"), cmd);

	espSerial->println(cmd);
	
	int idx = readUntil(timeout);
	
	LOGDEBUG1(F("---------------------------------------------- >"), idx);
	LOGDEBUG();
	
    return idx;
}

int EspDrv::sendCmd(const __FlashStringHelper* cmd, int timeout)
{
	strcpy_P(cmdBuf, (char*)cmd);
	return sendCmd(cmdBuf, timeout);
}


// Read from serial until one of the tags is found
// Returns:
//   the index of the tag found in the ESPTAGS array
//   -1 if no tag was found (timeout)
int EspDrv::readUntil(int timeout, const char* tag, bool findTags)
{
	ringBuf.init();
	
	char c;
    unsigned long start = millis();
	int ret = -1;
    	
	while ((millis() - start < timeout) and ret<0)
	{
        if(espSerial->available())
		{
            c = (char)espSerial->read();
			LOGDEBUG0(c);
			ringBuf.push(c);
        
			if (tag!=NULL)
			{
				if (ringBuf.endsWith(tag))
				{
					ret = NUMESPTAGS;
					//LOGDEBUG1("xxx");
				}
			}
			if(findTags)
			{
				for(int i=0; i<NUMESPTAGS; i++)
				{
					if (ringBuf.endsWith(ESPTAGS[i]))
					{
						ret = i;
						break;
					}
				}
			}
		}
    }


	if (millis() - start >= timeout)
	{
		LOGWARN(F(">>> TIMEOUT >>>"));
	}
	
	//ringBuf.init();
	//INFO("Return: %d > '%s'", ret, ringBuf);
	
    return ret;
}


void EspDrv::espEmptyBuf(bool warn) 
{
    char c;
	int i=0;
	while(espSerial->available() > 0)
    {
		c = espSerial->read();
		if (i>0 and warn==true)
			LOGDEBUG0(c);
		i++;
	}
	if (i>0 and warn==true)
    {
		LOGDEBUG(F(""));
		LOGDEBUG1(F("Dirty characters in the serial buffer! >"), i);
	}
}


// copied from Serial::timedRead
int EspDrv::timedRead()
{
  int _timeout = 500;
  int c;
  long _startMillis = millis();
  do
  {
    c = espSerial->read();
    if (c >= 0) return c;
  } while(millis() - _startMillis < _timeout);
  
  return -1; // -1 indicates timeout
}



EspDrv espDrv;
