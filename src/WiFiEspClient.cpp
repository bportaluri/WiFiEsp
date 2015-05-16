
#include "utility/debug.h"
#include <inttypes.h>


#include "WiFiEsp.h"
#include "WiFiEspClient.h"
#include "WiFiEspServer.h"

#include "utility/EspDrv.h"


WiFiEspClient::WiFiEspClient() : _sock(255)
{

}

WiFiEspClient::WiFiEspClient(uint8_t sock) : _sock(sock)
{

}

int WiFiEspClient::connect(const char* host, uint16_t port)
{	
	_sock = getFirstSocket();

    if (_sock != NO_SOCKET_AVAIL)
    {
    	if (!EspDrv::startClient(host, port, _sock))
			return 0;

    	WiFiEspClass::_state[_sock] = _sock;
    }
	else
	{
    	Serial.println(F("No socket available"));
    	return 0;
    }
    return 1;
}

int WiFiEspClient::connect(IPAddress ip, uint16_t port)
{
	char s[18];  
	sprintf(s, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	return connect(s, port);
}



size_t WiFiEspClient::write(uint8_t b)
{
	  return write(&b, 1);
}

size_t WiFiEspClient::write(const uint8_t *buf, size_t size)
{
    //INFO("Entering WiFiEspClient::write (%d, %d)", _sock, size);

	if (_sock >= MAX_SOCK_NUM)
	{
		setWriteError();
		return 0;
	}
	if (size==0)
	{
		setWriteError();
		return 0;
	}
	
	if (!EspDrv::sendData(_sock, buf, size))
	{
		setWriteError();
		INFO1(F("Failed to write, disconnecting"));
		delay(2000);
		stop();
		return 0;
	}
/*
	if (!EspDrv::checkDataSent(_sock))
	{
		setWriteError();
		return 0;
	}
*/
	return size;
}


int WiFiEspClient::available()
{
	if (_sock != 255)
	{
		int bytes = EspDrv::availData(_sock);
		if (bytes>0)
		{
			return bytes;
		}
	}

	return 0;
}

int WiFiEspClient::read()
{
  uint8_t b;
  if (!available())
    return -1;

  EspDrv::getData(_sock, &b);
  
  //Serial.print((char)b);
  
  return b;
}

int WiFiEspClient::read(uint8_t* buf, size_t size)
{
  uint16_t _size = size;
  if (!EspDrv::getDataBuf(_sock, buf, &_size))
      return -1;
  return 0;
}

int WiFiEspClient::peek()
{
	  uint8_t b;
	  if (!available())
	    return -1;

	  EspDrv::getData(_sock, &b, 1);
	  return b;
}

void WiFiEspClient::flush()
{
  while (available())
    read();
}


void WiFiEspClient::stop()
{
	//INFO1("Entering WiFiEspClient::stop");
	
	if (_sock == 255)
		return;

	EspDrv::stopClient(_sock);
	
	WiFiEspClass::_state[_sock] = NA_STATE;
	_sock = 255;
}

uint8_t WiFiEspClient::connected()
{
	if (_sock == 255)
	{
		return 0;
	}
	
	uint8_t s = status();
	
	return (s != CLOSED);
}


uint8_t WiFiEspClient::status()
{
	if (_sock == 255)
	{
		return CLOSED;
	}
	
	if (EspDrv::availData(_sock))
	{
		return ESTABLISHED;
	}
	
	return LISTEN;
}

WiFiEspClient::operator bool()
{
  return _sock != 255;
}



////////////////////////////////////////////////////////////////////////////////
// Private Methods
////////////////////////////////////////////////////////////////////////////////

uint8_t WiFiEspClient::getFirstSocket()
{
    for (int i = 0; i < MAX_SOCK_NUM; i++)
	{
      if (WiFiEspClass::_state[i] == NA_STATE)
      {
          return i;
      }
    }
    return SOCK_NOT_AVAIL;
}
