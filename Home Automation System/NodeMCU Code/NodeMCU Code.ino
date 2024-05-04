
-------------------------------------------------------------------------
Code for NodeMCU ESP8266
-------------------------------------------------------------------------

#define BLYNK_TEMPLATE_ID "TMPLDbDZZjF6"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "Sct72QZgvk6TKogK_iPPgi9iTJyS7oEk"

#include <SoftwareSerial.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

#define LED_PIN D8 // Pin for the LED
#define PIR_PIN D7 // Pin for the PIR sensor
#define BUZZER_PIN D6 // Pin for the new buzzer
#define RELAY_PIN D2     // Pin for the relay
#define DOORLOCK_PIN D3    // Pin for the solenoid lock
#define BUTTON_PIN D1 // Pin for the physical button
#define BELL_PIN D0     // Pin for the bell

#define GAS_THRESHOLD 800 // Set your desired gas threshold here

SoftwareSerial espSerial(D5, D4); // RX, TX

char auth[] = "Sct72QZgvk6TKogK_iPPgi9iTJyS7oEk";
char ssid[] = "Ahanjxck";
char pass[] = "123456789";

bool motionDetected = false;
bool switchState = false; // Variable to hold the state of the switch
bool fanState = false;    // Variable to hold the state of the fan
bool buttonPressed = false; // Variable to track button press

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(DOORLOCK_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW); // Turn off the fan initially
  digitalWrite(DOORLOCK_PIN, LOW);  // Initially unlock the solenoid
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set the button pin as an input with internal pull-up resistor
  pinMode(BELL_PIN, OUTPUT);    
}


BLYNK_WRITE(V4) { // This function gets called when the Blynk app writes a value to V4
  int ledValue = param.asInt();
  digitalWrite(LED_PIN, ledValue);
}

BLYNK_WRITE(V3) { // This function gets called when the Blynk app writes a value to V2
  int fanValue = param.asInt();
  fanState = fanValue;
  
  if (fanState) {
    digitalWrite(RELAY_PIN, LOW); // Turn on the fan via relay
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Turn off the fan via relay
  }
}

BLYNK_WRITE(V0) {
  int newSwitchState = param.asInt();
  
  // If switch state changes, update the variable
  if (newSwitchState != switchState) {
    switchState = newSwitchState;
    
    if (!switchState) {
      digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer if switch is off
    }
  }
}

BLYNK_WRITE(V2) { // This function gets called when the Blynk app writes a value to V2
  int lockValue = param.asInt();
  
  if (lockValue) {
    digitalWrite(DOORLOCK_PIN, LOW); // Lock the solenoid
  } else {
    digitalWrite(DOORLOCK_PIN, HIGH); // Unlock the solenoid
  }
}

void loop() {
  Blynk.run();

  int pirState = digitalRead(PIR_PIN);
 
  if (switchState) { // Check if switch is ON
    if (pirState == HIGH && motionDetected == false){
      motionDetected = true;
      Serial.println("Motion Detected"); 
      digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
      Blynk.logEvent("motion", "Motion was detected in the room"); // Log a motion detection event with a custom message
    } else if (pirState == LOW && motionDetected == true) {
      motionDetected = false;
      Serial.println("Motion not Detected"); 
      digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer if switch is off
  }

// Check the physical button state
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH) { // Button is pressed (assuming it's connected to GND when pressed)
    if (!buttonPressed) { // Button was just pressed
      buttonPressed = true;
      Blynk.logEvent("button","Someone at the door!"); // Send a notification
    digitalWrite(BELL_PIN, HIGH); // Turn on the buzzer when the button is pressed
    }
  } else {
    buttonPressed = false; // Button is released
    digitalWrite(BELL_PIN, LOW);
  }

  int gasValue; // Declare gasValue here
  if (espSerial.available()) {
    String data = espSerial.readStringUntil('\n');
    Serial.print("Received from Arduino: ");
    Serial.println(data);

    if (data.startsWith("Gas Value: ")) {
      int gasValue = data.substring(11).toInt();
      Blynk.virtualWrite(V1, gasValue);
      if (gasValue >= GAS_THRESHOLD) {
             Blynk.logEvent("gas","There has gas leak!"); 
      }
    }
       

    
  }
}

