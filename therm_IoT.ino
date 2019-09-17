
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>               // include SPI library
#include <Adafruit_GFX.h>      // include adafruit graphics library
#include <Adafruit_PCD8544.h>  // include adafruit PCD8544 (Nokia 5110) library
 
// Nokia 5110 LCD module connections (CLK, DIN, D/C, CS, RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(D6, D2, D1);

// GPIO where the DS18B20 is connected to
const int oneWireBus = D3;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

const int buttonPin = D4;
volatile int displayStatus = 0;
volatile int requestTemp = 0;
volatile int updateDisplay = 0;
volatile float temperatureC = 0;

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();
  sensors.setResolution(10);
  sensors.requestTemperatures();
  
  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Temp: ");
  display.display();
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), displayButtonPushedISR, CHANGE);

  timer1_attachInterrupt(updateTempISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(1200000);



}

void loop() {
  
  if (requestTemp){
    temperatureC = sensors.getTempCByIndex(0);
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

ICACHE_RAM_ATTR void displayButtonPushedISR(){
  updateDisplay = 1;
  if(digitalRead(buttonPin)==HIGH){
    displayStatus = 1;
  }
  else{
    displayStatus = 0;
  }
}

ICACHE_RAM_ATTR void updateTempISR(){
  requestTemp = 1;
  timer1_write(1200000);
  
}
