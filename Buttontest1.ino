/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Button
*/

// constants won't change. They're used here to set pin numbers:
const int fuelButtonPin = 53;     // the number of the pushbutton pin
const int fuelSolenoidPin =  37;      // the number of the LED pin
const int armPin = 35;
const int enableRelayPin = 39;

// variables will change:
int fuelButtonState = 0;         // variable for reading the pushbutton status
int armPinState = 1;
int enableRelayState = 1;
int fuelOutput = 0;

void setup() {
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(fuelSolenoidPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(fuelButtonPin, INPUT);
  pinMode(armPin, INPUT);
  pinMode(enableRelayPin, OUTPUT);
  digitalWrite(enableRelayPin, HIGH);
  digitalWrite(fuelSolenoidPin, HIGH);
  
}

void loop() {
  armPinState = digitalRead(armPin);
  enableRelayState = digitalRead(enableRelayPin);
  fuelButtonState = digitalRead(fuelButtonPin);
  fuelOutput = digitalRead(fuelSolenoidPin);
  if (armPinState == LOW) {

    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (fuelButtonState == HIGH) {
      // turn LED on:
      digitalWrite(fuelSolenoidPin, HIGH);
    } else {
      // turn LED off:
      digitalWrite(fuelSolenoidPin, LOW);
    }
    Serial.print("Status armPinState: ");
    Serial.print(armPinState);
    Serial.print(" fuelButtonState: ");
    Serial.print(fuelButtonState);
    Serial.print(" fuelOutput: ");
    Serial.print(fuelOutput);
    Serial.print(" enableRelayState: ");
    Serial.print(enableRelayState);
    Serial.print(" enableRelayPin: ");
    Serial.println(enableRelayPin);
  } else {
  //  enableRelayState = 0;
  //  digitalWrite(enableRelayPin, HIGH);
    digitalWrite(fuelSolenoidPin, LOW);
    Serial.print("Status armPinState: ");
    Serial.print(armPinState);
    Serial.print(" fuelButtonState: ");
    Serial.print(fuelButtonState);
    Serial.print(" fuelOutput: ");
    Serial.print(fuelOutput);
    Serial.print(" enableRelayState: ");
    Serial.print(enableRelayState);
    Serial.print(" enableRelayPin: ");
    Serial.println(enableRelayPin);
  }
  //udelay(1);
}
