#ifndef WiFiEspServer_h
#define WiFiEspServer_h

#include <Server.h>

#include "WiFiEsp.h"


class WiFiEspClient;

class WiFiEspServer : public Server
{

public:
	WiFiEspServer(uint16_t port);


	/*
	* Gets a client that is connected to the server and has data available for reading.
	* The connection persists when the returned client object goes out of scope; you can close it by calling client.stop().
	* Returns a Client object; if no Client has data available for reading, this object will evaluate to false in an if-statement.
	*/
	WiFiEspClient available(uint8_t* status = NULL);

	/*
	* Start the TCP server
	*/
	void begin();

	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *buf, size_t size);

	uint8_t status();

	using Print::write;


private:
	uint16_t _port;
	bool _started;
	
};

#endif
