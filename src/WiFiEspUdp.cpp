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
    uint8_t sock = getFirstSocket();
    if (sock != NO_SOCKET_AVAIL)
    {
        //EspDrv::startClient(host, port, _sock)
		//ServerDrv::startServer(port, sock, UDP_MODE);
        WiFiEspClass::_server_port[sock] = port;
        _sock = sock;
        _port = port;
        return 1;
    }
    return 0;

}


/* return number of bytes available in the current packet,
   will return zero if parsePacket hasn't been called yet */
int WiFiEspUDP::available() {
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

	  //ServerDrv::stopClient(_sock);

	  _sock = NO_SOCKET_AVAIL;
}

int WiFiEspUDP::beginPacket(const char *host, uint16_t port)
{
  if (_sock == NO_SOCKET_AVAIL)
	  _sock = getFirstSocket();
  if (_sock != NO_SOCKET_AVAIL)
  {
	  EspDrv::startClient(host, port, _sock, UDP_MODE);
	  WiFiEspClass::_state[_sock] = _sock;
	  return 1;
  }
  return 0;
}


int WiFiEspUDP::beginPacket(IPAddress ip, uint16_t port)
{
	char s[18];
	sprintf(s, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

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
	bool r = EspDrv::sendData(_sock, buffer, size);
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
	EspDrv::getData(_sock, &b, false, &connClose);

	return b;
}

int WiFiEspUDP::read(unsigned char* buffer, size_t len)
{
	int bytes = available();

	if(bytes==-1)
		return -1;

	if (bytes!=len)
		LOGWARN1(bytes, len);


	uint16_t size = 0;
	if (!EspDrv::getDataBuf(_sock, buffer, &size))
		return -1;
	return size;
}

int WiFiEspUDP::peek()
{
  uint8_t b;
  if (!available())
    return -1;

  //ServerDrv::getData(_sock, &b, 1);
  return b;
}

void WiFiEspUDP::flush()
{
  // TODO: a real check to ensure transmission has been completed
}

IPAddress  WiFiEspUDP::remoteIP()
{
	uint8_t _remoteIp[4] = {0};
	uint8_t _remotePort[2] = {0};

	//WiFiDrv::getRemoteData(_sock, _remoteIp, _remotePort);
	IPAddress ip(_remoteIp);
	return ip;
}

uint16_t  WiFiEspUDP::remotePort()
{
	uint8_t _remoteIp[4] = {0};
	uint8_t _remotePort[2] = {0};

	// 	WiFiDrv::getRemoteData(_sock, _remoteIp, _remotePort);
	uint16_t port = (_remotePort[0]<<8)+_remotePort[1];
	return port;
}


////////////////////////////////////////////////////////////////////////////////
// Private Methods
////////////////////////////////////////////////////////////////////////////////

// TODO remove duplication with WiFiEspClient::getFirstSocket()

uint8_t WiFiEspUDP::getFirstSocket()
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

