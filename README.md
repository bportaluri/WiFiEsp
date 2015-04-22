# WiFiEsp

With an ESP8266 board, WiFiEsp library allows an Arduino board to connect to the internet.
It can serve as either a server accepting incoming connections or a client making outgoing ones.
The WiFiEsp library is very similar to the Arduino [WiFi](http://www.arduino.cc/en/Reference/WiFi) and [Ethernet](http://www.arduino.cc/en/Reference/Ethernet) libraries, and many of the function calls are the same. 

It is still an alpha version but works fine already.


##Features

- APIs compatible with standard Arduino WiFi library.
- Use the standard ESP firmware.
- Support hardware and software serial ports
- Debugging features


##ToDo

- Clean the source code
- Add specific methods for ESP
- Add support for AP mode
- Align to the new ESP firmware
- Implement UDP
- Test all public methods
- Publish documentation and samples
- Avoid use of constructor?


##Supported APIs

###WiFiEsp class

- begin() - Not all authentication types
- disconnect() - Yes
- config()
- setDNS() - Not supported by AT commands
- SSID() - NO
- BSSID()
- RSSI()
- encryptionType()
- scanNetworks()
- getSocket()
- macAddress() - TODO

###WiFiEspServer class

The Server class creates servers which can send data to and receive data from connected clients (programs running on other computers or devices).

- WiFiServer() - YES
- begin() - YES
- available() - YES
- write() - YES
- print() - YES
- println() - YES


###Client class

The client class creates clients that can connect to servers and send and receive data.

- WiFiClient() - YES
- connected()
- connect() - YES
- write() - YES
- print() - YES
- println()
- available() - YES
- read() - YES
- flush()
- stop() - YES


###UDP class

The UDP class enables UDP message to be sent and received.

TODO


##Contributing

If you discover a bug or would like to propose a new feature, please open a new [issue](https://github.com/bportaluri/WiFiEsp/issues).
