/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * -----------------------------------------
 *  
 * ABOUT: https://www.mikroe.com/wifi-esp-click
 *  This sketch uses the WiFi ESP click to connect to a WiFi network and do a get
 *  HTTP request.
 *  
 * SPECIFICATIONS:
 * 
 *      Type:                     WiFi
 *  
 *      Applications:             Create smart appliances, home automation systems, 
 *                                wireless data loggers, etc
 *                            
 *      On-board modules:         ESP-WROOM-02 carries ESP8266EX highly integrated 
 *                                Wi-Fi SoC
 *                            
 *      Key Features  Protocols:  IPv4, TCP/UDP/HTTP/FTP, 802.11 b/g/n standard, 
 *                                UART interface, 3.3V power supply
 *                            
 *      Interface:                UART,GPIO
 *  
 *      Compatibility:            mikroBUS
 *  
 *      Click board size:         M (42.9 x 25.4 mm)
 *  
 *      Input Voltage:            3.3V
 *  
 *  
 * DATASHEET: https://download.mikroe.com/documents/datasheets/0c-esp-wroom-02-datasheet-en.pdf
 *  
 *  ----------------------------------------
 *  
 * Please report if you find any issue when using this code so we can
 * keep improving it
 */
 
/*
 WiFiEsp example: WebClientRepeating

 This sketch connects to a web server and makes an HTTP request
 using an Arduino ESP8266 module.
 It repeats the HTTP call each 10 seconds.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
*/

#include <ESP8266_Lib.h>
#undef max
#undef min
#include "FTTech_SAMD51Clicks.h"
#include "WiFiEsp.h"

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial2
#define serverport 80

ESP8266 wifi(&EspSerial);

char ssid[] = "Twim";            // your network SSID (name)
char pass[] = "12345678";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "arduino.cc";

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10000L; // delay between updates, in milliseconds

// Initialize the Ethernet client object
WiFiEspClient client;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void myTimerEvent(void);
void WiFiESPClick_Init(void);
void printWifiStatus(void);
void httpRequest(void);
/******************************************************************************
* Function Definitions
*******************************************************************************/

// Define EN and RST pins
#define EN 44
#define RST A4

void setup()
{
  // This will initiate the board
  FTClicks.begin(); 
  // Enable 5V
  FTClicks.turnON_5V();
  // Enable the power output of Click 2
  FTClicks.turnON(2);
  // Define Enable and Reset Pins of Click 2 as outputs
  pinMode(EN, OUTPUT);
  pinMode(RST, OUTPUT);  
  // Initialize serial for debugging and waits for console
  Serial.begin(9600);
  while(!Serial);
  // Puts a space in debug window and hard resets WiFi module
  Serial.print("\n\n\n\n\n\n\n\n\nInitiating...\n");
  WiFiESPClick_Init();
  // initialize serial for ESP module and waits
  EspSerial.begin(ESP8266_BAUD);
  while(!EspSerial);
  
  // initialize ESP module
  WiFi.init(&EspSerial);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  
  printWifiStatus();
}

void loop()
{
  // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if 10 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
}

// this method makes a HTTP connection to the server
void httpRequest()
{
  Serial.println();
    
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, serverport)) {
    Serial.println("Connecting...");
    
    // send the HTTP GET request
    client.println(F("GET /asciilogo.txt HTTP/1.1"));
    client.println(F("Host: arduino.cc"));
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void WiFiESPClick_Init(){
  Serial.print("Reseting module");
  digitalWrite(EN, HIGH);
  Serial.print(".");
  delay(500);
  digitalWrite(RST, LOW);
  Serial.print(".");
  delay(500);
  digitalWrite(RST, HIGH);
  Serial.print(".");
  delay(2000);
  Serial.println(" Reseted!\n");
}
