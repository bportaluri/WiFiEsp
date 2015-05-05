/*
 Web Server

 A simple web server that shows the value of the analog input pins using an ESP8266 module.

 This example is written for a network using WPA encryption.
 For WEP or WPA, change the Wifi.begin() call accordingly.
*/


#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>

#if not (defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__))
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7);
#endif

char ssid[] = "Twim";    // network SSID (name)
char pass[] = "12345678";  // network password

int status = WL_IDLE_STATUS;

WiFiEsp WiFi(&Serial1, &Serial);

WiFiEspServer server(&WiFi, 80);


void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial1.begin(9600);

  WiFi.init();

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }


  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    delay(5000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}


void loop()
{
  // listen for incoming clients
  WiFiEspClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK"
                         "Content-Type: text/html"
                         "Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<h1>Welcome to the WiFiEsp server example</h1>");
          // output the value of each analog input pin
          char buf[100];
          for (int analogChannel = 0; analogChannel < 6; analogChannel++)
          {
            int sensorReading = analogRead(analogChannel);
            sprintf(buf, "Analog input %d is %d<br />", analogChannel, sensorReading);
            client.println(buf);
          }
          client.println("</html>");
          break;
        }
        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

