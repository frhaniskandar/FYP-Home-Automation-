
#define BLYNK_TEMPLATE_ID "TMPLDbDZZjF6"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "Sct72QZgvk6TKogK_iPPgi9iTJyS7oEk"
#include <SoftwareSerial.h>
#include <BlynkSimpleStream.h>

SoftwareSerial espSerial(2, 3); // RX, TX for communication with ESP8266

const int gasSensorPin = A0;
const int buzzerPin = 7;


void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  pinMode(buzzerPin, OUTPUT);

}

void loop() {
  int gasValue = analogRead(gasSensorPin);
  Serial.print("Gas Sensor Value: ");
  Serial.println(gasValue);

  // Send gasValue to ESP8266
  espSerial.print("Gas Value: ");
  espSerial.println(gasValue);

  if (gasValue >= 800) {
    digitalWrite(buzzerPin, HIGH); // Turn on the buzzer
  } else {
    digitalWrite(buzzerPin, LOW); // Turn off the buzzer
  }


  Blynk.run(); // Handle Blynk communications

  delay(1000);
}

