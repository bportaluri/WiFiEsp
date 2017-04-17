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
#include "WiFiEspUdp.h"

#include "utility/EspDrv.h"
#include "utility/debug.h"

/* Constructor */
WiFiEspUDP::WiFiEspUDP() : _sock(NO_SOCKET_AVAIL) {}




/* Start WiFiUDP socket, listening at local port PORT */

uint8_t WiFiEspUDP::begin(uint16_t port)
{
    uint8_t sock = WiFiEspClass::getFreeSocket();
    if (sock != NO_SOCKET_AVAIL)
    {
        EspDrv::startClient("0", port, sock, UDP_MODE);
		
        WiFiEspClass::allocateSocket(sock);  // allocating the socket for the listener
        WiFiEspClass::_server_port[sock] = port;
        _sock = sock;
        _port = port;
        return 1;
    }
    return 0;

}


/* return number of bytes available in the current packet,
   will return zero if parsePacket hasn't been called yet */
int WiFiEspUDP::available()
{
	 if (_sock != NO_SOCKET_AVAIL)
	 {
		int bytes = EspDrv::availData(_sock);
		if (bytes>0)
		{
			return bytes;
		}
	}

	return 0;
}

/* Release any resources being used by this WiFiUDP instance */
void WiFiEspUDP::stop()
{
	  if (_sock == NO_SOCKET_AVAIL)
	    return;

      // Discard data that might be in the incoming buffer
      flush();
      
      // Stop the listener and return the socket to the pool
	  EspDrv::stopClient(_sock);
      WiFiEspClass::_state[_sock] = NA_STATE;
      WiFiEspClass::_server_port[_sock] = 0;

	  _sock = NO_SOCKET_AVAIL;
}

int WiFiEspUDP::beginPacket(const char *host, uint16_t port)
{
  if (_sock == NO_SOCKET_AVAIL)
	  _sock = WiFiEspClass::getFreeSocket();
  if (_sock != NO_SOCKET_AVAIL)
  {
	  //EspDrv::startClient(host, port, _sock, UDP_MODE);
	  _remotePort = port;
	  strcpy(_remoteHost, host);
	  WiFiEspClass::allocateSocket(_sock);
	  return 1;
  }
  return 0;
}


int WiFiEspUDP::beginPacket(IPAddress ip, uint16_t port)
{
	char s[18];
	sprintf_P(s, PSTR("%d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);

	return beginPacket(s, port);
}


int WiFiEspUDP::endPacket()
{
	return 1; //ServerDrv::sendUdpData(_sock);
}

size_t WiFiEspUDP::write(uint8_t byte)
{
  return write(&byte, 1);
}

size_t WiFiEspUDP::write(const uint8_t *buffer, size_t size)
{
	bool r = EspDrv::sendDataUdp(_sock, _remoteHost, _remotePort, buffer, size);
	if (!r)
	{
		return 0;
	}

	return size;
}

int WiFiEspUDP::parsePacket()
{
	return available();
}

int WiFiEspUDP::read()
{
	uint8_t b;
	if (!available())
		return -1;

	bool connClose = false;
	
    // Read the data and handle the timeout condition
	if (! EspDrv::getData(_sock, &b, false, &connClose))
      return -1;  // Timeout occured

	return b;
}

int WiFiEspUDP::read(uint8_t* buf, size_t size)
{
	if (!available())
		return -1;
	return EspDrv::getDataBuf(_sock, buf, size);
}

int WiFiEspUDP::peek()
{
  uint8_t b;
  if (!available())
    return -1;

  return b;
}

void WiFiEspUDP::flush()
{
	  // Discard all input data
	  int count = available();
	  while (count-- > 0)
	    read();
}


IPAddress  WiFiEspUDP::remoteIP()
{
	IPAddress ret;
	EspDrv::getRemoteIpAddress(ret);
	return ret;
}

uint16_t  WiFiEspUDP::remotePort()
{
	return EspDrv::getRemotePort();
}



////////////////////////////////////////////////////////////////////////////////
// Private Methods
////////////////////////////////////////////////////////////////////////////////


