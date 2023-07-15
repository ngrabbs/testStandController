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
const int LOOP_DELAY = 100000;
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

int testState = 0;

int counter = 0;

enum {idle, coldFire, pressureTest, hotFire, dePressure};
unsigned char systemState = idle;

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
  digitalWrite(enableRelayPin, LOW);
  digitalWrite(fuelSolenoidPin, LOW);
  digitalWrite(fuelButtonPin, LOW);

}

// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;
  //testState = c;

  if (c == 1) {
    testState = 1;
    SPDR = c+1;
  } else {
    testState = c;
    SPDR = c;
  }

  //SPDR = c+10;
  //int systemStatus[4] = { digitalRead(armPin), digitalRead(enableRelayPin), digitalRead(fuelButtonPin), digitalRead(oxidizerButtonPin) } ;
  //SPDR = digitalRead(armPin);
  //SPDR = 1;
}  // end of interrupt service routine (ISR) for SPI

void printStates(void) {
  // any statement(s)
    Serial.print("testState: ");
    Serial.print(testState);
    Serial.print(" Counter: ");
    Serial.print(counter);
    Serial.print(" Status armPinState: ");
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

void loop () {
  armPinState = digitalRead(armPin);
  enableRelayState = digitalRead(enableRelayPin);
  fuelButtonState = digitalRead(fuelButtonPin);
  fuelOUTPUT = digitalRead(fuelSolenoidPin);
  if (armPinState == HIGH) {
    enableRelayState = 1;

    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (fuelButtonState == HIGH) {
      // turn LED on:
      digitalWrite(fuelSolenoidPin, HIGH);
    } else {
      // turn LED off:
      digitalWrite(fuelSolenoidPin, LOW);
    }
  } else {
    // if armPin is off, everything should be off
    enableRelayState = 0;
    digitalWrite(fuelSolenoidPin, LOW);
    digitalWrite(oxidizerSolenoidPin, LOW);
    digitalWrite(enableRelayPin, LOW);
  }
  counter += 1;
  if (counter == LOOP_DELAY) {
    counter = 0;
    printStates();
  }
}
