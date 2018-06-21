#include <Arduino.h>
//#include <MyBase64.h>
//// Requires install of Base64 lib from https://github.com/agdl/Base64
#include <Base64.h>
#include "SMTPClient.h"




// Construction, destruction and initialisation
SMTPClient::SMTPClient()
{
	m_strFromEmail = NULL;
	m_strToEmail = NULL;
	m_strUsername = NULL;
	m_strPassword = NULL;
	m_strSubject = NULL;
	m_strContent = NULL;
	m_strDateTime = NULL;
	m_strIPAddress = NULL;
	m_nTime = 0;
	m_nLine = 0;
	memset(m_strStatus, 0, m_nMaxStatus);
}

SMTPClient::~SMTPClient()
{
}

// Interface
bool SMTPClient::connect(IPAddress ip)
{
	bool bResult = WiFiEspClient::connect(ip, 25) > 0;
	
	if (bResult)
	{
		Serial.print(F("Successfully connected to '"));
	}
	else
	{
		Serial.print(F("Failed to connect to '"));
	}
	Serial.print(ip[0]);
	Serial.print(F("."));
	Serial.print(ip[1]);
	Serial.print(F("."));
	Serial.print(ip[2]);
	Serial.print(F("."));
	Serial.print(ip[3]);

	Serial.println(F("'!"));
	
	return bResult;
}

bool SMTPClient::connect(const char *strHost)
{
	bool bResult = WiFiEspClient::connect(strHost, 25) > 0;
	
	if (bResult)
	{
		Serial.print(F("Successfully connected to '"));
	}
	else
	{
		Serial.print(F("Failed to connect to '"));
	}
	Serial.print(strHost);
	Serial.println(F("'!"));
	
	return bResult;
}

void SMTPClient::printErrorMsg()
{
	Serial.print(F("ERROR: "));
	Serial.print(F("SMTPClient.cpp"));
	Serial.print(F(", line "));
	Serial.print(m_nLine);
	Serial.print(F(", '"));
	Serial.print(m_strStatus);
	Serial.println(F("'!"));
}

bool SMTPClient::sendEmail()
{
	const uint16_t nBuffSize = 513;
	char strTemp[nBuffSize];
	
	if (!m_strIPAddress)
	{
		strcpy_P(m_strStatus, (PGM_P)F("ERROR in SMTPClient::sendEmail(): m_strIPAddress not set!"));
	}
	else if (!m_strToEmail)
	{
		strcpy_P(m_strStatus, (PGM_P)F("ERROR in SMTPClient::sendEmail(): m_strToEmail not set!"));
	}
	else if (!m_strUsername)
	{
		strcpy_P(m_strStatus, (PGM_P)F("ERROR in SMTPClient::sendEmail(): m_strUsername not set!"));
	}
	else if (!m_strPassword)
	{
		strcpy_P(m_strStatus, (PGM_P)F("ERROR in SMTPClient::sendEmail(): m_strPassword not set!"));
	}
	else if (!m_strSubject)
	{
		strcpy_P(m_strStatus, (PGM_P)F("ERROR in SMTPClient::sendEmail(): m_strSubject not set!"));
	}
	else
	{
		memset(strTemp, 0, nBuffSize);
		strcpy_P(strTemp, (PGM_P)F("EHLO "));
		strcat(strTemp, m_strIPAddress);
		Serial.print(strTemp);
		writeln(strTemp);
		if (!waitAck())
		{
			m_nLine = __LINE__;
			return false;
		}
		Serial.print(F("auth login plain"));
		writeln(F("auth login"));
		if (!waitAck())
		{
			m_nLine = __LINE__;
			return false;
		}		
		memset(strTemp, 0, nBuffSize);
		//base64_encode(strTemp, m_strUsername, strlen(m_strUsername));		
		Base64.encode(strTemp, m_strUsername, strlen(m_strUsername));		
		Serial.print(strTemp);
		Serial.print(F(", "));
		Serial.print(m_strUsername);
		writeln(strTemp);

		if (!waitAck())
		{
			m_nLine = __LINE__;
			return false;
		}
		memset(strTemp, 0, nBuffSize);
		//base64_encode(strTemp, m_strPassword, strlen(m_strPassword));		
		Base64.encode(strTemp, m_strPassword, strlen(m_strPassword));		
		Serial.print(strTemp);
		Serial.print(F(", "));
		Serial.print(m_strPassword);
		writeln(strTemp);
		if (!waitAck())
		{
			m_nLine = __LINE__;
			return false;
		}
		memset(strTemp, 0, nBuffSize);
		strcpy_P(strTemp, (PGM_P)F("MAIL From: <"));
		if (strlen(m_strFromEmail) > 0)
			strcat(strTemp, m_strFromEmail);
		strcat_P(strTemp, (PGM_P)F(">"));				
		Serial.print(strTemp);
		writeln(strTemp);
		if (!waitAck())
		{
			m_nLine = __LINE__;
			return false;
		}
		memset(strTemp, 0, nBuffSize);
		strcpy_P(strTemp, (PGM_P)F("RCPT To: <"));
		strcat(strTemp, m_strToEmail);
		strcat_P(strTemp, (PGM_P)F(">"));
		Serial.print(strTemp);
		writeln(strTemp);
		if (!waitAck())
		{
			m_nLine = __LINE__;
			return false;
		}
		Serial.print(F("DATA"));
		writeln(F("DATA"));
		if (!waitAck())
		{
			m_nLine = __LINE__;
			return false;
		}
		memset(strTemp, 0, nBuffSize);
		strcpy_P(strTemp, (PGM_P)F("To: <"));
		strcat(strTemp, m_strToEmail);
		strcat_P(strTemp, (PGM_P)F(">\r\n"));

		strcat_P(strTemp, (PGM_P)F("From: Irrigation controller <"));
		strcat(strTemp, m_strFromEmail);
		strcat_P(strTemp, (PGM_P)F(">\r\n"));

		strcat_P(strTemp, (PGM_P)F("Sent: "));
		strcat(strTemp, m_strDateTime);
		strcat_P(strTemp, (PGM_P)F("\r\n"));

		strcat_P(strTemp, (PGM_P)F("X-Accept-Language: en-us, en\r\n"));
		strcat_P(strTemp, (PGM_P)F("MIME-Version: 1.0\r\n"));
		strcat_P(strTemp, (PGM_P)F("Content-Type: text/plain; charset=us-ascii; format=flowed\r\n"));
		strcat_P(strTemp, (PGM_P)F("Content-Transfer-Encoding: 7bit\r\n"));

		strcat_P(strTemp, (PGM_P)F("Message-ID: <"));
		char strNum[7];
		memset(strNum, 0, 7);
		ultoa(m_nTime, strNum, 36);
		strcat(strTemp, strNum);
		strcat_P(strTemp, (PGM_P)F("."));
/*
	Message-ID: <4129F3CA.2020509@dc.edu>
	
	Append "<".

	Get the current (wall-clock) time in the highest resolution to which you have access (most systems can give it to you in milliseconds, but seconds will do);

	Generate 64 bits of randomness from a good, well-seeded random number generator;

	Convert these two numbers to base 36 (0-9 and A-Z) and append the first number, a ".", the second number, and an "@". This makes the left hand side of the message ID be only about 21 characters long.

	Append the FQDN of the local host, or the host name in the user's return address.

	Append ">". 
*/
		randomSeed(analogRead(0));
		uint32_t nRandom = random(0, 2147483647);
		memset(strNum, 0, 7);
		ultoa(nRandom, strNum, 36);
		strcat(strTemp, strNum);						
		strcat_P(strTemp, (PGM_P)F("@gmail.com>\r\n"));

		strcat_P(strTemp, (PGM_P)F("Subject: "));
		strcat(strTemp, m_strSubject);													
		strcat_P(strTemp, (PGM_P)F("\r\n"));

		strcat(strTemp, m_strContent);
		strcat_P(strTemp, (PGM_P)F("\r\n"));

		strcat_P(strTemp, (PGM_P)F("."));
		Serial.print(strTemp);
		writeln(strTemp);
		if (!waitAck())
		{
			m_nLine = __LINE__;
			return false;
		}
		writeln(F("QUIT"));
		Serial.print(F("QUIT"));
		if (waitAck())
		{
			m_nLine = __LINE__;
			strcpy_P(m_strStatus, (PGM_P)F("SMTPClient::sendEmail(): email sent!"));
			stop();
			return true;
		}
		else
		{
			m_nLine = __LINE__;
			flush();
			return false;
		}
	}
	return false;
}

// Helper functions
#ifndef __SAM3X8E__
	size_t SMTPClient::write(const __FlashStringHelper* str)
	{
		static const uint8_t nSize = 33;
		static char strTemp[nSize];
		
		memset(strTemp, 0, nSize);
		strcpy_P(strTemp, (PGM_P)str);
		return WiFiEspClient::print(strTemp);
	}

	size_t SMTPClient::writeln(const __FlashStringHelper* str)
	{
		static const uint8_t nSize = 33;
		static char strTemp[nSize];
		
		memset(strTemp, 0, nSize);
		strcpy_P(strTemp, (PGM_P)str);
		return WiFiEspClient::println(strTemp);
	}
#endif

size_t SMTPClient::write(const char* str)
{
	return WiFiEspClient::print(str);
}

size_t SMTPClient::writeln(const char* str)
{
	return WiFiEspClient::println(str);
}

void SMTPClient::printTime()
{
	Serial.print("@@@@@@ ");
	Serial.print(millis());
	Serial.println(" @@@@@@");
}

void SMTPClient::printAvailable()
{
	Serial.print("##### ");
	Serial.print(available());
	Serial.print(" #####");
}

void SMTPClient::printStatus()
{
	Serial.print("$$$$$ ");
	Serial.print(m_strStatus);
	Serial.print(" $$$$$");
}

bool SMTPClient::waitAck()
{
	bool bResult = true;
	uint8_t nResponse;
	int16_t nCount = 0, nI = 0;

	memset(m_strStatus, 0, m_nMaxStatus);
	while (!available())
	{
      delay(1);
	  nCount++;
 
	  // If nothing received for 10 seconds, timeout
	  if (nCount > m_nTimeout) 
	  {
		//stop();
		break;
	  }
	}
	while (available())
	{  
		nResponse = read();
		if ((nResponse != 10) && (nResponse != 13))
			m_strStatus[nI++] = nResponse;
	} 
	if ((strstr_P(m_strStatus, (PGM_P)F("250")) == m_strStatus) || (strstr_P(m_strStatus, (PGM_P)F("235")) == m_strStatus) || (strstr_P(m_strStatus, (PGM_P)F("354")) == m_strStatus) ||
		(strstr_P(m_strStatus, (PGM_P)F("220")) == m_strStatus) || (strstr_P(m_strStatus, (PGM_P)F("334")) == m_strStatus) || (strstr_P(m_strStatus, (PGM_P)F("221")) == m_strStatus))
	{
		Serial.println(F("...success"));
		Serial.println(m_strStatus);
		Serial.println();
	}
	else
	{
		Serial.println(F("...failure"));
		bResult = false; 
	}
/*
Serial.println(F("XXXXXXXXXXX"));
Serial.println(strstr_P(m_strStatus, (PGM_P)F("500")));	
Serial.println(m_strStatus);
Serial.println(bResult);
Serial.println(F("XXXXXXXXXXX"));
*/
	return bResult;
}
