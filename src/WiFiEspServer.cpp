
#include <string.h>
#include "utility/esp_drv.h"
#include "utility/debug.h"

#include "WiFiEsp.h"
#include "WiFiEspClient.h"
#include "WiFiEspServer.h"


WiFiEspServer::WiFiEspServer(WiFiEsp *esp, uint16_t port)
{
	_esp = esp;
	_port = port;
}

void WiFiEspServer::begin()
{
    INFO1("WiFiEspServer::begin");

	_esp->espDrv->startServer(_port);
	_started = true;
}

WiFiEspClient WiFiEspServer::available(byte* status)
{
	// the original method seems to handle automatic server restart
	
	int bytes = _esp->espDrv->availData(0);
	if (bytes>0)
	{
		INFO("returning client %d", _esp->espDrv->_connId);
		
		WiFiEspClient client(_esp, _esp->espDrv->_connId);
		return client;
	}

    return WiFiEspClient(_esp, 255);
}

uint8_t WiFiEspServer::status()
{
    return _esp->espDrv->getServerState(0);
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
        if (_esp->_state[sock] != 0)
        {
        	WiFiEspClient client(_esp, sock);
            n += client.write(buffer, size);
        }
    }
    return n;
}