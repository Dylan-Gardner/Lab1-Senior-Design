#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <WebSocketsServer.h>
#include <SPI.h>               // include SPI library
#include <FS.h>   // Include the SPIFFS library
#include <ArduinoJson.h>
#include <ESP8266Ping.h>

//#include <Adafruit_GFX.h>      // include adafruit graphics library
//#include <Adafruit_PCD8544.h>  // include adafruit PCD8544 (Nokia 5110) library
//#include <OneWire.h>
//#include <DallasTemperature.h>
//#include <ESP8266mDNS.h>

// username ZXNwODI2NnVzZXI=
// password ZXNwODI2NnBhc3M=
char serverName[] = "mail.smtp2go.com";   // The SMTP Server

// Nokia 5110 LCD module connections (CLK, DIN, D/C, CS, RST)
//Adafruit_PCD8544 display = Adafruit_PCD8544(D6, D1, D2);

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket(81);    // create a websocket server on port 81
WiFiClient espClient;
IPAddress ip(192,168,43,179);

bool softwareButtonPressed = false;
bool firePing = false;
double minTemp = 0;
double maxTemp = 26;
bool sentBelowText = false;
bool sentAboveText = false;
char* phoneNumber = "8478269269";
char* phoneEnding = "@txt.att.net";
void getTempFromClient();
//void handleRoot();              // function prototypes for HTTP handlers
//void handleLED();
void handleNotFound();

//const int oneWireBus = D4;
//OneWire oneWire(oneWireBus);
//DallasTemperature sensors(&oneWire);

/*
const int buttonPin = D3;
const int backlightPin = D0;
const int tempLightPin = D1;
volatile int displayStatus = 0;
volatile int updateDisplay = 0;
volatile int requestTemp = 0;
volatile float temperatureC = 0;
volatile float temperatureF = 0;
*/

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)

void setup(void) {
  
  //noInterrupts();
  //timer1_isr_init();
  timer1_attachInterrupt(checkIfOnISR);
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);
  timer1_write(1562500);
  //interrupts();

/*

  /////THERMOSTAT SETUP//////////////
  sensors.begin();
  sensors.setResolution(10);
  sensors.requestTemperatures();

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
  */
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
  server.on("/softwareButton", HTTP_POST, getTempFromClient);     // Call the 'handleRoot' function when a client requests URI "/"
  //server.on("/temp", HTTP_GET, handleTEMP);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
  //server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
  printCSV();

  

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


  if (firePing)
  {
    /*
    bool ret = Ping.ping(ip);
    //if (ret)
      {
        Serial.println("Device Still Connected");
        firePing = false;
      }
  
    else
   */
      {
        Serial.println("Device Disconnected");
        String jsError = "Disconnected";
        webSocket.broadcastTXT(jsError);
        firePing = false;
      }
  }

    

  /*
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
*/

}

/*
/////TEMP UPDATE EVERY SECOND ISR///////////

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
*/
///////////////////////////////////////////////////////////////////////////
// When URI / is requested, send a web page with a button to toggle the LED
///////////////////////////////////////////////////////////////////////////


ICACHE_RAM_ATTR void checkIfOnISR(){
  firePing = true;
  //timer1_write(1200000);
}



void getTempFromClient() {
    String payload = server.arg("plain");
    // Allocate JsonBuffer
    // Use arduinojson.org/assistant to compute the capacity.
    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
    DynamicJsonBuffer jsonBuffer(capacity);
  
   // Parse JSON object
    JsonObject& root = jsonBuffer.parseObject(payload);
    if (!root.success()) {
      Serial.println(F("Parsing failed!"));
      server.send(503,"Network Timeout");
      return;
    }

    String newTemp;
    if (softwareButtonPressed == false)
    {
    newTemp = root["temp"].as<char*>();
    server.send(200,"Data Retrieved");
    Serial.print(F("Response:"));
    Serial.println(newTemp);
    }

    else
    {
     newTemp = root["temp"].as<char*>();
      Serial.print(F("Response:"));
      Serial.println(newTemp);
      server.send(303,"Turn On Display");
      softwareButtonPressed = false;
      Serial.println("Turning on Display..");
    }
 
      updateData(newTemp);

}

/*

void handleTEMP() {                          // If a POST request is made to URI /LED
  server.sendHeader("Location", "/");
  char c[200];
  sprintf(c, "<form action=\"/temp\" method=\"GET\"><input type=\"submit\" value=\"Update Temp\"></form> <p>%.2f F</p> <p>%.2f C</p>", temperatureF, temperatureC); // Add a header to respond with a new location for the browser to go to the home page again
  server.send(200, "text/html", c);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect

}
*/

void updateData(String data){
  timer1_write(1562500);
if (data != "85")
{
  checkMinMax(data);
  writeValueToCSV(data);
  webSocket.broadcastTXT(data);
}
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
  if (path != "/softwareButton")
  {
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
  else
  {
    //getTempFromClient();
    return false;
  }
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received
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
      //Serial.printf("[%u] get Text: %s\n", num, payload);
      //Serial.println((char *)payload);
      char* softwarePush = "software";
      char* ATTPush = "AT&T";
      char* VerizonPush = "Verizon";
      char* SprintPush = "Sprint";
      char* minPush = "Min=";
      char* maxPush = "Max=";

      char* charpay = (char *)payload;

      
      if(strcmp(charpay, softwarePush) == 0)                  //Software Button Pressed
        {
        Serial.println("Software Button Pressed");
        if (softwareButtonPressed == false)
          {softwareButtonPressed = true;}
        }
      else if(strcmp(charpay, ATTPush) == 0)   
      {
        Serial.println("Carrier Set to AT&T");
        phoneEnding = "@txt.att.net";
      }
      else if(strcmp(charpay, VerizonPush) == 0)   
      {
        Serial.println("Carrier Set to Verizon");
        phoneEnding = "@txt.att.net";
      }
      else if(strcmp(charpay, SprintPush) == 0)   
      {
        Serial.println("Carrier Set to Sprint");
        phoneEnding = "@txt.att.net";
      }
      else if(strncmp(charpay,minPush,4) == 0)
      {
        Serial.println("new Min");
        String n = strtok(charpay,"=");
        n = strtok(NULL," ");
        double newMinTemp = stringToDouble(n);
        minTemp = newMinTemp;
      }
      else if(strncmp(charpay,maxPush,4) == 0)
      {
        Serial.println("new Max");
        String n = strtok(charpay,"=");
        n = strtok(NULL," ");
        //Serial.println(n);
        double newMaxTemp = stringToDouble(n);
        maxTemp = newMaxTemp;
      }
      else
      {
       Serial.println("Phone Number set to ");
        Serial.print(charpay);
        phoneNumber = charpay;
      }
      
      
      break;

  }
}


void checkMinMax(String data){
  double temp = stringToDouble(data);
  //Serial.println(temp);
  if (sentBelowText == true)
  {
    if (temp > minTemp)
    {
      sentBelowText = false;
    }
  }

  if (sentAboveText == true)
  {
    if (temp < maxTemp)
    {
      sentAboveText = false;
    }
  }


if (sentBelowText == false)
{
 if (temp < minTemp)
 {
  Serial.println("Sending Min Email");
  String rcpHolder = "";
  String phoneBeginningRCPT = "RCPT To: ";
  rcpHolder.concat(phoneBeginningRCPT);
  rcpHolder.concat(phoneNumber);
  rcpHolder.concat(phoneEnding);


  String toHolder;
  String phoneBeginningTo = "To: ";
  toHolder.concat(phoneBeginningTo);
  toHolder.concat(phoneNumber);
  toHolder.concat(phoneEnding);

  //byte ret = sendEmail(rcptChar,toChar);
  sentBelowText = true;
 }
}

if (sentAboveText == false)
{
 if (temp > maxTemp)
 {
  Serial.println("Sending Max Email");
  String rcpHolder = "";
  String phoneBeginningRCPT = "RCPT To: ";
  rcpHolder.concat(phoneBeginningRCPT);
  rcpHolder.concat(phoneNumber);
  rcpHolder.concat(phoneEnding);


  String toHolder;
  String phoneBeginningTo = "To: ";
  toHolder.concat(phoneBeginningTo);
  toHolder.concat(phoneNumber);
  toHolder.concat(phoneEnding);


//  Serial.println(rcpHolder[1] + rcpHolder[2] + rcpHolder[3]);
  //scanf(" %c",&toHolder);
//  Serial.print(&toHolder);
 // byte ret = sendEmail(rcpHolder,toHolder);
  sentAboveText = true;
 }
}
}
//////////////////EMAIL STUFF
/*
char* formatEmailRCPT(){

  Serial.println(myConcatenation);
  /*
  char* phoneBeginning = "RCPT To: ";
  char* sendTo = "";
  strcpy(sendTo, phoneBeginning);
  strcat(sendTo, phoneNumber);
  strcat(sendTo, phoneEnding);
  return sendTo;
  
  return myConcatenation;
}

char* formatEmailTo(){
  char* toConCat = "";
  char* phoneBeginningTo = "To: ";
  sprintf(toConCat,"%s%s%s",phoneBeginningTo,phoneNumber,phoneEnding);
  Serial.println(toConCat);
  /*
  char* phoneBeginning = "To: ";
  char* yeet = "";
  strcpy(yeet, phoneBeginning);
  strcat(yeet, phoneNumber);
  strcat(yeet, phoneEnding);

  return yeet;
  
  return toConCat;
}
*/
byte sendEmail(String rcptChar, String toChar)

{

//char* toChar = formatEmailTo();
//char* rcptChar = formatEmailRCPT();
Serial.println(toChar);
Serial.println(rcptChar);


  if (espClient.connect(serverName, 2525) == 1)
  {
    Serial.println(F("connected"));
  }
  else
  {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!emailResp())
    return 0;
  Serial.println(F("Sending EHLO"));
  espClient.println("EHLO www.example.com");
  if (!emailResp())
    return 0;
  Serial.println(F("Sending auth login"));
  espClient.println("AUTH LOGIN");
  if (!emailResp())
    return 0;
  Serial.println(F("Sending User"));
  espClient.println("ZXNwODI2NnVzZXI="); // Your encoded Username
  if (!emailResp())
    return 0;
  Serial.println(F("Sending Password"));
  espClient.println("ZXNwODI2NnBhc3M=");// Your encoded Password
  if (!emailResp())
    return 0;
  Serial.println(F("Sending From"));
  espClient.println(F("MAIL From: test@gmail.com")); // Enter Sender Mail Id
  if (!emailResp())
    return 0;
  Serial.println(F("Sending To"));
  espClient.println(rcptChar); // Enter Receiver Mail Id
  if (!emailResp())
    return 0;
  Serial.println(F("Sending DATA"));
  espClient.println(F("DATA"));
  if (!emailResp())
    return 0;
  Serial.println(F("Sending email"));
  espClient.println(toChar); // Enter Receiver Mail Id
  // change to your address
  espClient.println(F("From: ESP@fakedomain")); // Enter Sender Mail Id
  espClient.println(F("Subject: ESP8266 test e-mail\r\n"));
  espClient.println(F("This is is a test e-mail sent from ESP8266.\n"));
  espClient.println(F("Second line of the test e-mail."));
  espClient.println(F("Third line of the test e-mail."));
  //
  espClient.println(F("."));
  if (!emailResp())
    return 0;
  //
  Serial.println(F("Sending QUIT"));
  espClient.println(F("QUIT"));
  if (!emailResp())
    return 0;
  //
  espClient.stop();
  Serial.println(F("disconnected"));
  return 1;

}


 

byte emailResp()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient.available())
  {
    delay(1);
    loopCount++;
    if (loopCount > 20000)
    {
      espClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }
  responseCode = espClient.peek();
  while (espClient.available())
  {
    readByte = espClient.read();
    Serial.write(readByte);
  }
  if (responseCode >= '4')
  {
    return 0;
  }
  return 1;
}



void writeValueToCSV(String temperature)
{
      File tempLog = SPIFFS.open("/temp_data.csv", "a"); // Write the time and the temperature to the csv file
      //tempLog.print(timestamp);
      //tempLog.print(',');
      tempLog.print(temperature);
      tempLog.print(',');
      tempLog.close();
      //Serial.println("--Wrote Temp to CSV--");
      //printCSV();
      
}

void printCSV(){
  File file2 = SPIFFS.open("/temp_data.csv", "r");
 
  if (!file2) {
      Serial.println("No temperature data found. Creating a new file..");
      return;
  }
  
  while (file2.available()) {
      Serial.write(file2.read());
  }
}

double stringToDouble(String & str)   
{
  return atof( str.c_str() );
}
///////////////
