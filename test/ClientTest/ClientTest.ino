/*
 WiFiEsp test: ClientTest
 
 Test client functions.
*/

#include "WiFiEsp.h"

// Emulate Serial1 on pins 7/6 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif


char ssid[] = "Twim";     // your network SSID (name)
char pwd[] = "12345678";  // your network password


// Initialize the Wifi client library
WiFiEspClient client;

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600);
  WiFi.init(&Serial1);
}

void loop()
{
  bool f;
  int c;

  
  assertEquals("Check status WL_DISCONNECTED", WiFi.status(), WL_DISCONNECTED);
  
  assertEquals("Connect", WiFi.begin(ssid, pwd), WL_CONNECTED);
  
  assertEquals("Check status WL_CONNECTED", WiFi.status(), WL_CONNECTED);
  
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  assertEquals("Ping", WiFi.ping("www.google.com"), true);
  
  assertEquals("Not connected", client.connected(), false);
  assertEquals("Connect to server", client.connect("www.brainjar.com", 80), 1);
  assertEquals("Connected", client.connected(), true);


  //--------------------------------------------------------------
  // HTTP request without 'Connection: close' command
  
  client.println("GET /java/host/test.html HTTP/1.1");
  client.println("Host: www.brainjar.com");
  client.println();

  // wait for the response
  long _startMillis = millis();
  while (!client.available() and (millis()-_startMillis < 2000))
  {
  }
  
  assertEquals("Response received", (millis()-_startMillis < 2000), true);
  
  f = client.find("<html>");
  assertEquals("Response check", f, true);
  if (f)
  {
    while( (c = client.read()) > 0)
      Serial.print((char)c);
  }

  assertEquals("Connected", client.connected(), true);


  //--------------------------------------------------------------
  
  delay(5000);
  
  assertEquals("Check status WL_CONNECTED", WiFi.status(), WL_CONNECTED);

  assertEquals("Connected", client.connected(), true);


  //--------------------------------------------------------------
  // HTTP request without 'Connection: close' command
  
  client.println("GET /java/host/test.html HTTP/1.1");
  client.println("Host: www.brainjar.com");
  client.println("Connection: close");
  client.println();


  // wait for the response
  _startMillis = millis();
  while (!client.available() and (millis()-_startMillis < 2000))
  {
  }
  
  assertEquals("Response received", (millis()-_startMillis < 2000), true);
  
  f = client.find("<html>");
  assertEquals("Response check", f, true);
  if (f)
  {
    while( (c = client.read()) > 0)
      Serial.print((char)c);
  }

  //--------------------------------------------------------------
  
  assertEquals("Check status WL_CONNECTED", WiFi.status(), WL_CONNECTED);
  assertEquals("Not connected", client.connected(), false);

  assertEquals("Ping", WiFi.ping("www.google.com"), true);

  assertEquals("Connect", WiFi.disconnect(), WL_DISCONNECTED);
  assertEquals("Check status WL_DISCONNECTED", WiFi.status(), WL_DISCONNECTED);

  Serial.println("END OF TESTS");
  delay(30000);
}


////////////////////////////////////////////////////////////////////////////////////


void assertEquals(const char* test, int actual, int expected)
{
  if(actual==expected)
    pass(test);
  else
    fail(test, actual, expected);
}

void assertEquals(const char* test, char* actual, char* expected)
{
  if(strcmp(actual, expected) == 0)
    pass(test);
  else
    fail(test, actual, expected);
}


void pass(const char* test)
{
  Serial.print(F("********************************************** "));
  Serial.print(test);
  Serial.println(" > PASSED");
  Serial.println();
}

void fail(const char* test, char* actual, char* expected)
{
  Serial.print(F("********************************************** "));
  Serial.print(test);
  Serial.print(" > FAILED");
  Serial.print(" (actual=\"");
  Serial.print(actual);
  Serial.print("\", expected=\"");
  Serial.print(expected);
  Serial.println("\")");
  Serial.println();
  delay(10000);
}

void fail(const char* test, int actual, int expected)
{
  Serial.print(F("********************************************** "));
  Serial.print(test);
  Serial.print(" > FAILED");
  Serial.print(" (actual=");
  Serial.print(actual);
  Serial.print(", expected=");
  Serial.print(expected);
  Serial.println(")");
  Serial.println();
  delay(10000);
}


