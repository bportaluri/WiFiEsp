# WiFiEsp

With an ESP8266 board, WiFiEsp library allows an Arduino board to connect to the internet.
It can serve as either a server accepting incoming connections or a client making outgoing ones.
The WiFiEsp library is very similar to the Arduino [WiFi](http://www.arduino.cc/en/Reference/WiFi) and [Ethernet](http://www.arduino.cc/en/Reference/Ethernet) libraries, and many of the function calls are the same. 

Supports ESP SDK version 1.1.1 and above (AT version 0.25 and above).


##Features

- APIs compatible with standard Arduino WiFi library.
- Use AT commands of standard ESP firmware (no need to flash a custom firmware).
- Support hardware and software serial ports.
- Configurable tracing level.

##Wiring

The WiFiEsp library has been designed to work with the [ESP WiFi shield] (http://www.instructables.com/id/Cheap-Arduino-WiFi-Shield-With-ESP8266/).
It is a cheap version of the Arduino WiFi shield that uses an ESP-01 module to provide networking capabilities to Arduino boards.


##Supported APIs

Most of the standard Arduino WiFi library methods are available. Refer to the [WiFi library page](http://www.arduino.cc/en/Reference/WiFi) for more details.

###WiFiEsp class

- begin() - Not all authentication types
- disconnect() - YES
- config()
- setDNS() - NO (no AT command available)
- SSID() - YES
- BSSID() - YES
- RSSI() - YES
- encryptionType() - NO (no AT command available)
- scanNetworks() - NO
- getSocket()
- macAddress() - YES


###WiFiEspServer class

The WiFiEspServer class creates servers which can send data to and receive data from connected clients (programs running on other computers or devices).

- WiFiEspServer() - YES
- begin() - YES
- available() - YES
- write() - YES
- print() - YES
- println() - YES


###Client class

The WiFiEspClient class creates clients that can connect to servers and send and receive data.

- WiFiEspClient() - YES
- connected() - YES
- connect() - YES
- write() - YES
- print() - YES
- println() - YES
- available() - YES
- read() - YES
- flush() - YES
- stop() - YES


###UDP class

The UDP class enables UDP message to be sent and received.

TODO


##Contributing

If you discover a bug or would like to propose a new feature, please open a new [issue](https://github.com/bportaluri/WiFiEsp/issues).
