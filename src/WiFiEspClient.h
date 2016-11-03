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

#ifndef WiFiEspClient_h
#define WiFiEspClient_h


#include "Arduino.h"
#include "Print.h"
#include "Client.h"
#include "IPAddress.h"



class WiFiEspClient : public Client
{
public:
  WiFiEspClient();
  WiFiEspClient(uint8_t sock);
  
  
  // override Print.print method
  
  size_t print(const __FlashStringHelper *ifsh);
  size_t println(const __FlashStringHelper *ifsh);


  /*
  * Connect to the specified IP address and port. The return value indicates success or failure.
  * Returns true if the connection succeeds, false if not.
  */
  virtual int connect(IPAddress ip, uint16_t port);

  /*
  * Connect to the specified host and port. The return value indicates success or failure.
  * Returns true if the connection succeeds, false if not.
  */
  virtual int connect(const char *host, uint16_t port);

  /*
  * Connect to the specified IP address and port using SSL. The return value indicates success or failure.
  * Returns true if the connection succeeds, false if not.
  */
  int connectSSL(IPAddress ip, uint16_t port);
  
  /*
  * Connect to the specified host and port using SSL. The return value indicates success or failure.
  * Returns true if the connection succeeds, false if not.
  */
  int connectSSL(const char* host, uint16_t port);
  
  /*
  * Write a character to the server the client is connected to.
  * Returns the number of characters written.
  */
  virtual size_t write(uint8_t);

  /*
  * Write data to the server the client is connected to.
  * Returns the number of characters written.
  */
  virtual size_t write(const uint8_t *buf, size_t size);


  virtual int available();

  /*
  * Read the next byte received from the server the client is connected to (after the last call to read()).
  * Returns the next byte (or character), or -1 if none is available.
  */
  virtual int read();


  virtual int read(uint8_t *buf, size_t size);

  /*
  * Returns the next byte (character) of incoming serial data without removing it from the internal serial buffer.
  */
  virtual int peek();

  /*
  * Discard any bytes that have been written to the client but not yet read.
  */
  virtual void flush();

  /*
  * Disconnect from the server.
  */
  virtual void stop();

  /*
  * Whether or not the client is connected.
  * Note that a client is considered connected if the connection has been closed but there is still unread data.
  * Returns true if the client is connected, false if not.
  */
  virtual uint8_t connected();


  uint8_t status();
  
  virtual operator bool();

  
  // needed to correctly handle overriding
  // see http://stackoverflow.com/questions/888235/overriding-a-bases-overloaded-function-in-c
  using Print::write;
  using Print::print;
  using Print::println;


  /*
  * Returns the remote IP address.
  */
  IPAddress remoteIP();
  

  friend class WiFiEspServer;

private:

  uint8_t _sock;     // connection id

  int connect(const char* host, uint16_t port, uint8_t protMode);
  
  size_t printFSH(const __FlashStringHelper *ifsh, bool appendCrLf);

};

#endif
