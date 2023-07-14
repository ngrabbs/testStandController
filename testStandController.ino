#include <SPI.h>

// constants
// MEGA CONSTANTS
//const int fuelButtonPin = 53;     // the pin for the physical test stand fuel button
//const int fuelSolenoidPin =  37;  // the pin that fires the relay connected to the fuel
//                                  // solenoid
//const int oxidizerSolenoidPin = 0; // the pin that fires the relay connected to the 
//                                   // oxidizer solenoid
//const int armPin = 35;             // the master enable physical switch
//const int enableRelayPin = 39;     // power up the relay board ( might not need )
//const int igniterButtonPin = 0;    // igniter physical button pin
//const int igniterSolenoidPin = 0;  // the pin that fires the relay connected to the igniter
//                                   // solenoid
// END MEGA CONSTANTS

// UNO CONSTANTS
const int fuelButtonPin = 0;     // the pin for the physical test stand fuel button
const int fuelSolenoidPin =  1;  // the pin that fires the relay connected to the fuel
                                  // solenoid
const int oxidizerSolenoidPin = 2; // the pin that fires the relay connected to the 
                                   // oxidizer solenoid
const int oxidizerButtonPin = 7; 
const int armPin = 3;             // the master enable physical switch
const int enableRelayPin = 4;     // power up the relay board ( might not need )
const int igniterButtonPin = 5;    // igniter physical button pin
const int igniterSolenoidPin = 6;  // the pin that fires the relay connected to the igniter
                                   // solenoid
// END UNO CONSTANTS

// variables
int fuelButtonState = 0;
int oxidizerButtonState = 0;
int igniterButtonState = 0;
int armPinState = 0;
int enableRelayState = 0;

int fuelOUTPUT = 0;
int oxidizerOUTPUT = 0;
int igniterOUTPUT = 0;

void setup() {
  // SPI setup
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  // turn on interrupts
  SPI.attachInterrupt();

  // Serial setup
  Serial.begin(9600);

  // Button setup
  pinMode(fuelSolenoidPin, OUTPUT);
  pinMode(fuelButtonPin, INPUT);
  pinMode(oxidizerSolenoidPin, OUTPUT);
  pinMode(oxidizerButtonPin, INPUT);
  pinMode(armPin, INPUT);
  pinMode(enableRelayPin, OUTPUT);

  // init pins ?
  digitalWrite(enableRelayPin, HIGH);
  digitalWrite(fuelSolenoidPin, HIGH);

}

// SPI interrupt routine
ISR (SPI_STC_vect)
{
//  byte c = SPDR;

//  SPDR = c+10;
  int systemStatus[4] = { digitalRead(armPin), digitalRead(enableRelayPin), digitalRead(fuelButtonPin), digitalRead(oxidizerButtonPin) } ;
  SPDR = systemStatus;
}  // end of interrupt service routine (ISR) for SPI

void loop () {
  armPinState = digitalRead(armPin);
  enableRelayState = digitalRead(enableRelayPin);
  fuelButtonState = digitalRead(fuelButtonPin);
  fuelOUTPUT = digitalRead(fuelSolenoidPin);
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
    Serial.print(" fuelOUTPUT: ");
    Serial.print(fuelOUTPUT);
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
    Serial.print(" fuelOUTPUT: ");
    Serial.print(fuelOUTPUT);
    Serial.print(" enableRelayState: ");
    Serial.print(enableRelayState);
    Serial.print(" enableRelayPin: ");
    Serial.println(enableRelayPin);
  }
  //udelay(1);
}
