
#include <string.h>
#include "utility/esp_drv.h"


#include "WiFiEsp.h"
#include "WiFiEspClient.h"
#include "WiFiEspServer.h"

WiFiEspServer::WiFiEspServer(uint16_t port)
{
	_port = port;
}

void WiFiEspServer::begin()
{
    uint8_t _sock = _esp->getSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
        _esp->espDrv->startServer(_port, _sock);
        _esp->_server_port[_sock] = _port;
        _esp->_state[_sock] = _sock;
    }
}

WiFiEspClient WiFiEspServer::available(byte* status)
{
	static int cycle_server_down = 0;
	const int TH_SERVER_DOWN = 50;

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        if (_esp->_server_port[sock] == _port)
        {
        	WiFiEspClient client(_esp, sock);
            uint8_t _status = client.status();
            uint8_t _ser_status = this->status();

            if (status != NULL)
            	*status = _status;

            //server not in listen state, restart it
            if ((_ser_status == 0)&&(cycle_server_down++ > TH_SERVER_DOWN))
            {
            	_esp->espDrv->startServer(_port, sock);
            	cycle_server_down = 0;
            }

            if (_status == ESTABLISHED)
            {                
                return client;  //TODO 
            }
        }
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
        if (_esp->_server_port[sock] != 0)
        {
        	WiFiEspClient client(_esp, sock);

            if (_esp->_server_port[sock] == _port &&
                client.status() == ESTABLISHED)
            {                
                n+=client.write(buffer, size);
            }
        }
    }
    return n;
}