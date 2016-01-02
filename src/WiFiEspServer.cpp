
#include "WiFiEspServer.h"

#include "utility/EspDrv.h"
#include "utility/debug.h"



WiFiEspServer::WiFiEspServer(uint16_t port)
{
	_port = port;
}

void WiFiEspServer::begin()
{
	LOGDEBUG(F("Starting server"));
	
	_started = EspDrv::startServer(_port);
	
	if (_started)
	{
		LOGINFO1(F("Server started on port"), 80);
	}
	else
	{
		LOGERROR(F("Server failed to start"));
	}
}

WiFiEspClient WiFiEspServer::available(byte* status)
{
	// TODO the original method seems to handle automatic server restart
	
	int bytes = EspDrv::availData(0);
	if (bytes>0)
	{
		WiFiEspClient client(EspDrv::_connId);
		return client;
	}

    return WiFiEspClient(255);
}

uint8_t WiFiEspServer::status()
{
    return EspDrv::getServerState(0);
}

size_t WiFiEspServer::write(uint8_t b)
{
    return write(&b, 1);
}

size_t WiFiEspServer::write(const uint8_t *buffer, size_t size)
{
	size_t n = 0;

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        if (WiFiEspClass::_state[sock] != 0)
        {
        	WiFiEspClient client(sock);
            n += client.write(buffer, size);
        }
    }
    return n;
}