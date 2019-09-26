#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>               // include SPI library
#include <FS.h>   // Include the SPIFFS library
#include <WebSocketsServer.h>

#include <Adafruit_GFX.h>      // include adafruit graphics library
#include <Adafruit_PCD8544.h>  // include adafruit PCD8544 (Nokia 5110) library


// Nokia 5110 LCD module connections (CLK, DIN, D/C, CS, RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(D6, D1, D2);

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket(81);    // create a websocket server on port 81


void handleRoot();              // function prototypes for HTTP handlers
void handleLED();
void handleNotFound();

const int oneWireBus = D4;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

const int buttonPin = D3;
const int backlightPin = D0;
const int tempLightPin = D1;
volatile int displayStatus = 0;
volatile int updateDisplay = 0;
volatile int requestTemp = 0;
volatile float temperatureC = 0;
volatile float temperatureF = 0;

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)

void setup(void) {

  /////THERMOSTAT SETUP//////////////
  sensors.begin();
  sensors.setResolution(10);
  sensors.requestTemperatures();

  timer1_attachInterrupt(updateTempISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(1200000);


  /////////////DISPLAY SETUP//////////
  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.display();
  pinMode(buttonPin, INPUT);
  pinMode(backlightPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), displayButtonPushedISR, CHANGE);


  pinMode(tempLightPin, OUTPUT);    // the pins with LEDs connected are outputs
  //////////////////////////////////
  //SERVER SETUP////////////////////
  //////////////////////////////////
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  wifiMulti.addAP("Samsung Galaxy S10+_7243", "tdem7012");   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("Cat House", "Hammock2!");
  //wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  SPIFFS.begin();                           // Start the SPI Flash Files System
  startWebSocket();            // Start a WebSocket server
  ////////////////////////////////////////////////
  //I dont think we need these for anything anymore, right?
  //As our data sending isnt going to be through post requests 
  //but through the websocket sending data every X seconds
  //server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  //server.on("/temp", HTTP_GET, handleTEMP);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  //server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}


void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
}


///////////////////////////////////////
//Listen for HTTP requests from clients
///////////////////////////////////////
void loop(void) {
  webSocket.loop();
  server.handleClient();
  if (requestTemp) {
    temperatureC = sensors.getTempCByIndex(0);
    temperatureF = sensors.getTempFByIndex(0);
    sensors.requestTemperatures();
    requestTemp = 0;
    updateDisplay = 1;
  }

  if (updateDisplay) {
    updateDisplay = 0;
    display.clearDisplay();

    if (displayStatus) {
      display.print("Temp: ");
      display.print(temperatureC);
      display.println(" C");
    }
    display.display();
  }


}

/////TEMP UPDATE EVERY SECOND ISR///////////
ICACHE_RAM_ATTR void updateTempISR() {
  requestTemp = 1;
  timer1_write(1200000);
}

/////Button Pushed for display ISR/////////
ICACHE_RAM_ATTR void displayButtonPushedISR() {
  updateDisplay = 1;
  if (digitalRead(buttonPin) == LOW) {
    displayStatus = 1;
    digitalWrite(backlightPin, HIGH);
  }
  else {
    displayStatus = 0;
    digitalWrite(backlightPin, LOW);
  }
}

///////////////////////////////////////////////////////////////////////////
// When URI / is requested, send a web page with a button to toggle the LED
///////////////////////////////////////////////////////////////////////////
void handleRoot() {
  server.send(200, "text/html", "<form action=\"/temp\" method=\"GET\"><input type=\"submit\" value=\"Update Temp\"></form>");
}

void handleTEMP() {                          // If a POST request is made to URI /LED
  
  server.sendHeader("Location", "/");
  char c[200];
  sprintf(c, "<form action=\"/temp\" method=\"GET\"><input type=\"submit\" value=\"Update Temp\"></form> <p>%.2f F</p> <p>%.2f C</p>", temperatureF, temperatureC); // Add a header to respond with a new location for the browser to go to the home page again
  server.send(200, "text/html", c);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect

}

void updateData(String data,String timestamp){
  //My idea is make a csv or a json file and for each interval
  //timestamp it with either the time or just an integer.
  //and then call that integer stored temp from a csv on the board.
  webSocket.broadcastTXT(data);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed version
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false;                                          // If the file doesn't exist, return false
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received
  String fakeData = "Data";
  String fakeTimestamp = "Timestamp";
 
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:                     // if new text data is received
      Serial.printf("[%u] get Text: %s\n", num, payload);
      updateData(fakeData,fakeTimestamp);
      break;
    
  }
}
