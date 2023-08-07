#include <Vector.h>
// cli write to arduino:
// arduino-cli compile -b arduino:avr:uno ~/Arduino/testStandController
// arduino-cli compile -b arduino:avr:uno ~/Arduino/testStandController -u
// arduino-cli compile -b arduino:avr:mega ~/Arduino/testStandController -u

// constants
// MEGA CONSTANTS
// const int chamberPressureSensor = A2;  // chamber pressure sensor pin
// const int fuelPressureSensor = A5;     // fuel pressure sensor pin
// const int fuelTankPressureSensor = A4;    // fuel pressure before the tank pin
// const int oxidizerPressureSensor = A1; // oxidizer pressure sensor pin
//
// const int oxidizerButtonPin = 47;   // the pin for the physical test stand ox button
// const int fuelButtonPin = 53;       // the pin for the physical test stand fuel button
// const int fuelOutPin = 35;     // the pin that fires the relay connected to the fuel
//                                    // solenoid
// const int oxidizerOutPin = 37; // the pin that fires the relay connected to the
//                                    // oxidizer solenoid
// const int fillValveOutPin = 39;   // fuel tank pressure fill valve
// const int dumpValveOutPin = 41;   // fuel tank pressure dump valve
//
// const int igniterButtonPin = 0;    // igniter physical button pin
// const int igniterOutPin = 43; // the pin that fires the relay connected to the igniter
//                                   // solenoid
// END MEGA CONSTANTS

// UNO CONSTANTS
const int fuelPressureSensor = A0; // the pin for the fuel pressure sensor
const int chamberPressureSensor = A1;
const int fuelTankPressureSensor = A2;
const int oxidizerPressureSensor = A3;

const int fuelButtonPin = 8; // the pin for the physical test stand fuel button
const int oxidizerButtonPin = 9;

const int fuelOutPin = 2;       // the pin that fires the relay connected to the fuel
                                // solenoid
const int oxidizerOutPin = 3;   // the pin that fires the relay connected to the
                                // oxidizer solenoid
const int fillValveOutPin = 4;  // fuel tank pressure fill valve
const int dumpValveOutPin = 5;  // fuel tank pressure dump valve
const int igniterButtonPin = 6; // igniter physical button pin
const int igniterOutPin = 7;    // the pin that fires the relay connected to the igniter
                                // solenoid
// END UNO CONSTANTS

// FUEL PRESSURE SENSOR DATA
// starts at 1.x volts and 40 psi is about 3.x volts
// AEM-50PSIA
// 0.5 = -14.5, 4.5 = 35.3 PSI
// 0.5 = 0,     4.5 = 343.385 kPa
// Transfer Function: PSI = (12.5*(Voltage))-20.95
//

// variables
const int fuelOperatingPressure = 1;
const int oxidizerOperatingPressure = 100;
const int tankPressureAbortTime = 30000;
const int logSampleTimeMillis = 20;

int DEBUG = 0;
int fuelButtonState = 1;
int oxidizerButtonState = 0;
int igniterButtonState = 0;

int fuelOUT = 0;
int oxidizerOUT = 0;
int igniterOUT = 0;
int dumpValveOUT = 0;
int fillValveOUT = 0;

unsigned long debounceDelay = 5;
Vector<String> globalLog; // vector should be a lib in arduino

int counter = 0;
unsigned long startTime = millis();

enum systemStates
{
  idle,
  coldFire,
  testLogger,
  hotFire,
  toggleDebug
};
systemStates systemState = idle;
systemStates systemStatePrev = idle;

float getSensorVoltage(int);
void printOptions(void);
void changeSystemState(systemStates);
float getFuelPressure(void);
float getFuelPressure2(void);
void printFuelPressure(void);
void runColdFire(void);
void runTestLogger(void);
void toggleOutput(String);
void timedLogger(int);
void runToggleDebug(void);

void setup()
{
  // Serial setup
  Serial.begin(9600);

  // Button setup
  pinMode(fuelButtonPin, INPUT_PULLUP);
  pinMode(oxidizerButtonPin, INPUT);

  pinMode(fuelOutPin, OUTPUT);
  pinMode(oxidizerOutPin, OUTPUT);
  pinMode(igniterOutPin, OUTPUT);

  pinMode(dumpValveOutPin, OUTPUT);
  pinMode(fillValveOutPin, OUTPUT);

  // init pins ?
  digitalWrite(fuelOutPin, LOW);
  digitalWrite(oxidizerOutPin, LOW);
  digitalWrite(dumpValveOutPin, LOW);
  digitalWrite(fillValveOutPin, LOW);
  digitalWrite(igniterOutPin, LOW);

  // wait a few seconds so the cli is up then present the message
  delay(15);
  Serial.println("");
  printOptions();
}

// main program loop
void loop()
{
  switch (systemState)
  {
  case testLogger:
    runTestLogger();
    break;
  case coldFire:
    runColdFire();
    break;
  case toggleDebug:
    runToggleDebug();
    break;
  default:
    runIdle();
    break;
  }
}

void runToggleDebug(void)
{
  DEBUG = !DEBUG;
  changeSystemState(idle);
  return;
}

void timedLogger(int totalTime = 1)
{
  // what we want to do here is have a loop that runs ever x mills of sample time then exit
  int count = 0;
  int maxCount = totalTime / logSampleTimeMillis;
  if (maxCount == 0) {
    maxCount = 1;
  }
  while (count < maxCount)
  {
    Serial.println(String(
      String(millis() - startTime) + "," + systemState + "," + fuelButtonState + "," + oxidizerButtonState + "," + igniterButtonState + "," + fuelOUT + "," + oxidizerOUT + "," + igniterOUT + "," + fillValveOUT + "," + dumpValveOUT + ",CP:" + getSensorVoltage(chamberPressureSensor) + ",FP:" + getSensorVoltage(fuelPressureSensor) + ",FP2:" + getSensorVoltage(fuelTankPressureSensor) + ",OP:" + getSensorVoltage(oxidizerPressureSensor)));
    count = count + 1;
  }
}

void toggleOutput(String output)
{
  if (output == "fuelOUT")
  {
    fuelOUT = !fuelOUT;
    digitalWrite(fuelOutPin, fuelOUT);
  }
  else if (output == "oxidizerOUT")
  {
    oxidizerOUT = !oxidizerOUT;
    digitalWrite(oxidizerOutPin, oxidizerOUT);
  }
  else if (output == "igniterOUT")
  {
    igniterOUT = !igniterOUT;
    digitalWrite(igniterOutPin, igniterOUT);
  }
  else if (output == "fillValveOUT")
  {
    fillValveOUT = !fillValveOUT;
    digitalWrite(fillValveOutPin, fillValveOUT);
  }
  else if (output == "dumpValveOUT")
  {
    dumpValveOUT = !dumpValveOUT;
    digitalWrite(dumpValveOutPin, dumpValveOUT);
  }
}

void printOptions(void)
{
  Serial.println("");
  Serial.print("Menu:  DEBUG:[");
  Serial.print(DEBUG);
  Serial.println("]");
  Serial.println("  Test Logger: 1");
  Serial.println("  Cold Fire: 2");
  Serial.println("  Toggle Debug: 3");
  Serial.println("");
}

void changeSystemState(systemStates newState)
{
  systemStatePrev = systemState;
  systemState = newState;
}

float getFuelPressure(void)
{
  int sensorValue = analogRead(fuelPressureSensor);
  float voltage = sensorValue * (5.0 / 1023.0);
  // PSI = (12.5*(Voltage))-20.95 AEM 50PSiA sensor
  return ((12.5 * voltage) - 20.95); // need a better way to do this for multiple sensors
}

float getFuelPressure2(void)
{
  int sensorValue = analogRead(fuelTankPressureSensor);
  float voltage = sensorValue * (5.0 / 1023.0);
  return voltage;
}

void printFuelPressure(void)
{
  Serial.print("FP: [");
  Serial.print(getFuelPressure());
  Serial.println("]");
}

float getSensorVoltage(int sensor)
{
  int sensorValue = analogRead(sensor);
  float voltage = sensorValue * (5.0 / 1023.0);
  return voltage;
}

void runTestLogger(void)
{
  int counter = 0;
  while (counter < 20)
  {
    timedLogger();
    delay(100);
    timedLogger();
    counter = counter + 1;
  }
  changeSystemState(idle);
}

void runIdle(void)
{
  if (systemStatePrev != idle)
  {
    printOptions();
    systemStatePrev = systemState;
  }

  if (Serial.available() > 0)
  {
    // read the incoming byte:
    String request = Serial.readString();
    request.trim();
    Serial.print("Got: [");
    Serial.print(request); // print as an ASCII-encoded decimal
    Serial.println("]");
    if (request == "1")
    {
      changeSystemState(testLogger);
    }
    else if (request == "2")
    {
      changeSystemState(coldFire);
    }
    else if (request == "3")
    {
      changeSystemState(toggleDebug);
    }
    /*
    switch(request) {
      case "1":
        changeSystemState(testLogger);
        break;
      case "2":
        changeSystemState(coldFire);
        break;
      case "3":
        changeSystemState(testPin);
        break;
      default:
        Serial.println("Incoming byte does not match, setting state to Idle");
        printOptions();
        changeSystemState(idle);
    }
    */
  }
  else
  {
    fuelButtonState = digitalRead(fuelButtonPin);
    if (fuelButtonState == 0)
    {
      changeSystemState(coldFire);
    }
  }
}

void runColdFire(void)
{
  Serial.println("starting cold fire procedure");
  timedLogger();

  // pressurize fuel tank
  if (DEBUG) Serial.println("pressurizing fuel tank, closing dump valve, opening fill valve");
  // this means to close the normally open dump valve
  toggleOutput("dumpValveOUT");
  // open the fill valve
  toggleOutput("fillValveOUT");

  // check if fuel pressure comes up to operating pressure
  float pressureStartTime = millis();
  while (getSensorVoltage(fuelTankPressureSensor) < fuelOperatingPressure)
  {
    timedLogger(500);
    //    timedLogger();
    //    delay(500);
    if ((millis() - pressureStartTime) > tankPressureAbortTime)
    {
      if (DEBUG) Serial.println("Fuel pressure never came up, hit time out, aborting...");
      toggleOutput("fillValveOUT");
      toggleOutput("dumpValveOUT");
      if (DEBUG) Serial.println("Cold Fire failed, returning to Idle");
      changeSystemState(idle);
      return;
    }
  }
  if (DEBUG) Serial.println("completed pressure fill ");
  printFuelPressure();

  // check if oxidizer comes up to operating pressure
  // TBD

  // fire igniter
  /* not for cold fire
  Serial.println("firing igniter");
  digitalWrite(igniterOutPin, HIGH);
  delay(1500);
  digitalWrite(igniterOutPin, LOW);
  */

  // open oxidizer solenoid for x seconds
  if (DEBUG) Serial.println("opening fuel solenoid 500 millis ");
  timedLogger();
  toggleOutput("fuelOUT");
  timedLogger(1500);
  toggleOutput("fuelOUT");
  if (DEBUG) Serial.println("closed fuel solenoid ");
  timedLogger();
  delay(100);
  if (DEBUG) Serial.println("opening oxidizer solenoid 500 millis ");
  timedLogger();
  toggleOutput("oxidizerOUT");
  timedLogger(500);
  toggleOutput("oxidizerOUT");
  if (DEBUG) Serial.println("closed oxidizer solenoid ");
  timedLogger();

  // depressurize fuel tank
  toggleOutput("fillValveOUT");
  toggleOutput("dumpValveOUT");

  // complete
  timedLogger();
  if (DEBUG) Serial.println("cold flow is complete setting state to idle");

  changeSystemState(idle);
}