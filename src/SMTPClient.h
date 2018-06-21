#ifndef __SMTP_CLIENT_H
#define __SMTP_CLIENT_H




//#include <Time.h>
#include <time.h>
#include "WiFiEspClient.h"




class SMTPClient: public WiFiEspClient
{
	public:

		// Construction, destruction and initialisation
		SMTPClient();
		~SMTPClient();


		// Interface
		bool connect(IPAddress ip);
		bool connect(const char *strHost);

		void setIPAddr(const char* str)
		{
			m_strIPAddress = (char*)str;
		};
		void setFrom(const char* str)
		{
			m_strFromEmail = (char*)str;
		};
		void setTo(const char* str)
		{
			m_strToEmail = (char*)str;
		};
		void setUsername(const char* str)
		{
			m_strUsername = (char*)str;
		};
		void setPassword(const char* str)
		{
			m_strPassword = (char*)str;
		};
		void setSubject(const char* str)
		{
			m_strSubject = (char*)str;
		};
		void setContent(const char* str)
		{
			m_strContent = (char*)str;
		};
		void setDateTime(const char* str)
		{
			m_strDateTime = (char*)str;
		};
		void setTime(time_t nTime)
		{
			m_nTime = nTime;
		};
		bool sendEmail();
		const char* getStatus()
		{
			return m_strStatus;
		};
		void printErrorMsg();

	protected:

		// Data
		static const uint32_t m_nTimeout = 300000;
		static const uint8_t m_nMaxStatus = 151;

		char *m_strFromEmail, *m_strToEmail, *m_strDateTime, *m_strUsername, *m_strPassword, *m_strSubject, *m_strContent, *m_strIPAddress,
			 m_strStatus[m_nMaxStatus];
		uint16_t m_nLine;
		time_t m_nTime;
		
		// Helper functions
		bool waitAck();
		void setServerMessage();

		// Debugging
		void printTime();
		void printAvailable();
		void printStatus();
		
#ifndef __SAM3X8E__
		size_t write(const __FlashStringHelper* str);
		size_t writeln(const __FlashStringHelper* str);
#endif
		size_t write(const char* str);
		size_t writeln(const char* str);
};




#endif
