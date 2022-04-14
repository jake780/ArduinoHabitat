// Sensor pin = 13
// r1= 8 r2 = 6
// Screen button = 7
// light1button = 9, light2button = 10

// TODO: 
// 
// Cut 2 new back pieces (one with a slot for wire)
// 
// 


// include the library code:
#include <LiquidCrystal.h>
#include "DHT.h"
#define DHTTYPE DHT11
DHT dht(13, DHTTYPE);

// LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
String states[] = {" Alduin's Lair", "ECE-1000 Project", " Day: ", "(Time)", "Temperature: ", "Humidity: ", "Light1: ", "Light2: ", "    Nice", "    Cock"};
int currentState = 0;
const int lcdButtonPin = 7;

// Relays
const int r1 = 8, r2 = 6;
const int s1 = 9, s2 = 10;
bool relay1 = false;
bool relay2 = false;

// Time
int second;
int minute;
int hour;
int day;
int curTime[4] = {};

void nextState() {
  // Change LCD Screens
  currentState += 2;
  if(currentState >= (sizeof(states)/sizeof(states[0]))) {
    currentState = 0;
  }
}

bool button(int pin){
  return digitalRead(pin);
}

void displayText(String line1, String line2) {
  // Output line1 and line2 to the LCD Screen
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

void keepTime() {
  // set the days, hours, and minutes variables
  if ((second == 59) & (minute != 59) & (hour != 23)) {
    second = 0;
    minute++;
  }
  if ((second == 59)& (minute == 59)& (hour != 23)) { //59
    second = 0;
    minute = 0;
    hour++;
  }
  if ((second == 5) & (minute == 59) & (hour == 23)) { // 59
    second = 0;
    minute = 0;
    hour = 0;
    day++;
  }
}

void updateRelays() {
  // Update the states of the relays
  if ((hour == 12)) {
    relay1 = true;
    relay2 = true;
  }
  if ((hour == 0) & (day > 0)) {
    relay1 = false;
    relay2 = false;
  }
   digitalWrite(r1, relay1);
   digitalWrite(r2, relay2);
}

String timeString() {
  // returns a string of the current runtime
  String line = "Time: " + String(hour) + "h:" + String(minute) + "m:" + String(second) + "s";
  return line;
}

String relayStateString(bool relay) {
  String line;
  if (relay == true) {
    line = "OFF";
  }
  else {
    line = "ON";
  }
  return line;
}

String sensorText(String sensor) {
  String line;
  if (sensor == "temp") {
    return String(int(dht.readTemperature(true))) + "F";
  }
  else if (sensor == "humidity") {
    return String(int(dht.readHumidity())) + "%";
  }
  else {
    return "Bad parameters";
  }
}

bool toggle(bool relay) {
  if (relay == false) {
    return true;
  }
  if (relay == true) {
    return false;
  }
}

void setup() {
  // Serial Setup
  Serial.begin(9600);
  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Sensor init
  dht.begin();
  // Set relay pins
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  // Set Push Button pins
  pinMode(s1, INPUT_PULLUP);
  pinMode(s2, INPUT_PULLUP);
  pinMode(lcdButtonPin, INPUT_PULLUP);
  
}

void loop() {
  // Time
  second = (millis() / 1000) - (minute * 59) - (hour * 3600) - (day * 86400);
  //second++;  // for testing
  keepTime();
  updateRelays();
  
  // LCD state change by button
  if (button(lcdButtonPin)) {
    nextState();
    delay(500);
  }
  else {
    if (currentState == 2) {
      displayText(states[currentState] + String(day), timeString());
      delay(50);
    }
    else if (currentState == 4) {
      displayText(states[currentState] + sensorText("temp"), states[currentState+1] + sensorText("humidity"));
      delay(50);
    }
    else if (currentState == 6) {
      displayText(states[currentState] + relayStateString(relay1), states[currentState+1] + relayStateString(relay2));
      delay(50);
    }
    else {
      displayText(states[currentState], states[currentState+1]);
      delay(50);
    }
  }

  // Light Toggle buttons
  if (button(s1) == false) {
    relay1 = toggle(relay1);
    delay(500);
  }
  if (button(s2) == false) {
    relay2 = toggle(relay2);
    delay(500);
  }

  // Tick rate
  delay(10);
  // turn off automatic scrolling
  lcd.noAutoscroll();
}
