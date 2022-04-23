// Sensor pin = 13
// r1= 8 r2 = 6
// Screen button = 7
// light1button = 9, light2button = 10

// include the library code:
#include <LiquidCrystal.h>
#include "DHT.h"
#define DHTTYPE DHT11
DHT dht(13, DHTTYPE);

// LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
String states[] = {" Alduin's Lair", "ECE-1000 Project", " Day: ", "(Time)", "Temperature: ", "Humidity: ", "Light1: ", "Light2: "};
int currentState = 0;
const int lcdButtonPin = 7;

// Relays
const int r1 = 8, r2 = 6;
bool relay1 = false;
bool relay2 = false;
bool relay1Toggled = false;
bool realy2Toggled = false;
// Push buttons
const int s1 = 9, s2 = 10;

// Time
int second;
int minute;
int hour;
int day;
int curTime[4] = {};
int lastsecond;

bool button(int pin){
  // Returns the state of a button
  return digitalRead(pin);
}

void nextState() {
  // Change LCD Screens
  currentState += 2;
  if(currentState >= (sizeof(states)/sizeof(states[0]))) {
    currentState = 0;
  }
}

void updateDisplay() {
  // Update the display based on the current state
   if (lastsecond != second) {
     if (currentState == 2) {
       displayText(states[currentState] + String(day), timeString());
     }
     else if (currentState == 4) {
       displayText(states[currentState] + sensorText("temp"), states[currentState+1] + sensorText("humidity"));
     }
     else if (currentState == 6) {
       displayText(states[currentState] + relayStateString(relay1), states[currentState+1] + relayStateString(relay2));
     }
     else {
       displayText(states[currentState], states[currentState+1]);
     }
   }
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
  // If a minute has passed
  if ((second == 60)&(minute != 59)) {
    second = 0;
    minute++;
  }
  // If an hour has passed
  if ((second == 60)&(minute == 59)&(hour != 23)) { 
    second = 0;
    minute = 0;
    hour++;
  }
  // If a day has passed
  if ((second == 60)&(minute == 59)&(hour == 23)) { 
    second = 0;
    minute = 0;
    hour = 0;
    day++;
  }
}

void updateRelays() {
  // Update the states of the relays
  if ((hour == 12) & (relay1 == false) & (relay1Toggled == false)) {
    offMessage();
    relay1 = true;
    relay1Toggled = true;
  }
  if ((hour == 0) & (day > 0) & (relay1 == true) & (relay1Toggled == true)) {
    onMessage();
    relay1 = false;
    relay1Toggled = false;
  }
   digitalWrite(r1, relay1);
   digitalWrite(r2, relay2);
}

void onMessage() {
  // Display a light on message
  displayText("  Turning on", "     Light");
  delay(500);
}

void offMessage() {
  // Display a light off message
  displayText("  Turning off", "     Light");
  delay(500);
}

String timeString() {
  // Returns a string of the current runtime
  String line = "Time: " + String(hour) + "h:" + String(minute) + "m:" + String(second) + "s";
  return line;
}

String relayStateString(bool relay) {
  // Returns a string of the current relay state
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
  // Returns a string of the current temperature and humidity
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
  // Returns the opposite of the relay state
  if (relay == false) {
    offMessage();
    return true;
  }
  if (relay == true) {
    onMessage();
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
  second = (millis() / 1000) - (minute * 60) - (hour * 3600) - (day * 86400);
  keepTime();
  updateRelays();
  
  // LCD state change by button
  if (button(lcdButtonPin)) {
    nextState();
    delay(500);
  }
  else {
    updateDisplay();
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
  lastsecond = second;
}
