#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>   // Include the WebServer library

#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>               // include SPI library

#include <Adafruit_GFX.h>      // include adafruit graphics library
#include <Adafruit_PCD8544.h>  // include adafruit PCD8544 (Nokia 5110) library
 
// Nokia 5110 LCD module connections (CLK, DIN, D/C, CS, RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(D6, D1, D2);

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleLED();
void handleNotFound();

const int oneWireBus = D4;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

const int buttonPin = D3;
const int backlightPin = D0;
volatile int displayStatus = 0;
volatile int updateDisplay = 0;
volatile int requestTemp = 0;
volatile float temperatureC = 0;
volatile float temperatureF = 0;


void setup(void){

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
  display.setCursor(0,0);
  display.display();
  pinMode(buttonPin, INPUT);
  pinMode(backlightPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), displayButtonPushedISR, CHANGE);


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

  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
////////////////////////////////////////////////
  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/temp", HTTP_GET, handleTEMP);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}

///////////////////////////////////////
//Listen for HTTP requests from clients
///////////////////////////////////////
void loop(void){
  server.handleClient();
  if (requestTemp){
    temperatureC = sensors.getTempCByIndex(0);
    temperatureF = sensors.getTempFByIndex(0);
    sensors.requestTemperatures();
    requestTemp = 0;
    updateDisplay = 1;
  }                 

  if(updateDisplay){
    updateDisplay = 0;
    display.clearDisplay();

    if(displayStatus){
      display.print("Temp: ");
      display.print(temperatureC);
      display.println(" C");
    }
    display.display();
  }
}

/////TEMP UPDATE EVERY SECOND ISR///////////
ICACHE_RAM_ATTR void updateTempISR(){
  requestTemp = 1;
  timer1_write(1200000);  
}

/////Button Pushed for display ISR/////////
ICACHE_RAM_ATTR void displayButtonPushedISR(){
  updateDisplay = 1;
  if(digitalRead(buttonPin)==LOW){
    displayStatus = 1;
    digitalWrite(backlightPin, HIGH);
  }
  else{
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
  server.sendHeader("Location","/");
  char c[200];
  sprintf(c, "<form action=\"/temp\" method=\"GET\"><input type=\"submit\" value=\"Update Temp\"></form> <p>%.2f F</p> <p>%.2f C</p>",temperatureF,temperatureC);// Add a header to respond with a new location for the browser to go to the home page again
  server.send(200, "text/html", c);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
