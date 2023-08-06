#include <Vector.h>
// cli write to arduino:
// arduino-cli compile -b arduino:avr:uno ~/Arduino/testStandController
// arduino-cli compile -b arduino:avr:uno ~/Arduino/testStandController -u
// arduino-cli compile -b arduino:avr:mega ~/Arduino/testStandController -u

// constants
// MEGA CONSTANTS
//const int chamberPressureSensor = A2;  // chamber pressure sensor pin
//const int fuelPressureSensor = A5;     // fuel pressure sensor pin
//const int fuelPressure2Sensor = A4;    // fuel pressure before the tank pin
//const int oxidizerPressureSensor = A1; // oxidizer pressure sensor pin
//
//const int oxidizerButtonPin = 47;   // the pin for the physical test stand ox button
//const int fuelButtonPin = 53;       // the pin for the physical test stand fuel button
//const int fuelSolenoidPin = 35;     // the pin that fires the relay connected to the fuel
//                                    // solenoid
//const int oxidizerSolenoidPin = 37; // the pin that fires the relay connected to the
//                                    // oxidizer solenoid
//const int fillValveSolenoid = 39;   // fuel tank pressure fill valve
//const int dumpValveSolenoid = 41;   // fuel tank pressure dump valve
//
//const int igniterButtonPin = 0;    // igniter physical button pin
//const int igniterSolenoidPin = 43; // the pin that fires the relay connected to the igniter
//                                   // solenoid
// END MEGA CONSTANTS

// UNO CONSTANTS
const int fuelPressureSensor = A0; // the pin for the fuel pressure sensor
const int chamberPressureSensor = A1;
const int fuelPressure2Sensor = A2;
const int oxidizerPressureSensor = A3;

const int fuelButtonPin = 8;     // the pin for the physical test stand fuel button
const int oxidizerButtonPin = 9;

const int fuelSolenoidPin =  2;  // the pin that fires the relay connected to the fuel
                                   // solenoid
const int oxidizerSolenoidPin = 3; // the pin that fires the relay connected to the
                                    // oxidizer solenoid
const int fillValveSolenoid = 4;   // fuel tank pressure fill valve
const int dumpValveSolenoid = 5;   // fuel tank pressure dump valve
const int igniterButtonPin = 6;    // igniter physical button pin
const int igniterSolenoidPin = 7;  // the pin that fires the relay connected to the igniter
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
const int DEBUG = 1;

int fuelButtonState = 1;
int oxidizerButtonState = 0;
int igniterButtonState = 0;

int fuelOUTPUT = 0;
int oxidizerOUTPUT = 0;
int igniterOUTPUT = 0;
int dumpValveOUTPUT = 0;
int fillValveOUTPUT = 0;

unsigned long debounceDelay = 5;
Vector<String> globalLog; // vector should be a lib in arduino

int counter = 0;
unsigned long startTime = millis();

enum systemStates
{
  idle,
  coldFire,
  testLogger,
  testPin,
  pressureTest,
  hotFire,
  dePressure
};
systemStates systemState = idle;
systemStates systemStatePrev = idle;

void logger(void);
float getSensorVoltage(int sensor);
void printOptions(void);
void changeSystemState(systemStates newState);
float getFuelPressure(void);
float getFuelPressure2(void);
void printFuelPressure(void);
void runColdFire(void);
void runTestPin(void);
void runTestLogger(void);

void setup()
{
  // Serial setup
  Serial.begin(9600);

  // Button setup
  pinMode(fuelButtonPin, INPUT_PULLUP);
  pinMode(oxidizerButtonPin, INPUT);

  pinMode(fuelSolenoidPin, OUTPUT);
  pinMode(oxidizerSolenoidPin, OUTPUT);
  pinMode(igniterSolenoidPin, OUTPUT);

  pinMode(dumpValveSolenoid, OUTPUT);
  pinMode(fillValveSolenoid, OUTPUT);

  // init pins ?
  digitalWrite(fuelSolenoidPin, LOW);
  digitalWrite(oxidizerSolenoidPin, LOW);
  digitalWrite(dumpValveSolenoid, LOW);
  digitalWrite(fillValveSolenoid, LOW);
  digitalWrite(igniterSolenoidPin, LOW);

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
  case testPin:
    runTestPin();
    break;
  case testLogger:
    runTestLogger();
    break;
  case coldFire:
    runColdFire();
    break;
  default:
    runIdle();
    break;
  }
}

void logger(void)
{
  // millis from start, systemState, inputs, outputs, fuel pressure
  // millis()-startTime,systemStates(systemState),armState,fuelButtonState,oxidizerButtonState,igniterButtonState,fuelOUTPUT,oxidizerOUTPUT,igniterOUTPUT,fillValveOUTPUT,dumpValveOUTPUT
  if (DEBUG)
  {
    Serial.println(String(
        String(millis() - startTime) + "," + systemStates(systemState) + "," + "," + fuelButtonState + "," + oxidizerButtonState + "," + igniterButtonState + "," + fuelOUTPUT + "," + oxidizerOUTPUT + "," + igniterOUTPUT + "," + fillValveOUTPUT + "," + dumpValveOUTPUT + ",CP:" + getSensorVoltage(chamberPressureSensor) + ",FP:" + getSensorVoltage(fuelPressureSensor) + ",FP2:" + getSensorVoltage(fuelPressure2Sensor) + ",OP:" + getSensorVoltage(oxidizerPressureSensor)));
  }
}

float getSensorVoltage(int sensor)
{
  int sensorValue = analogRead(sensor);
  float voltage = sensorValue * (5.0 / 1023.0);
  return voltage;
}

void printOptions(void)
{
  Serial.println("");
  Serial.println("Menu: ");
  Serial.println("  Test Logger: 1");
  Serial.println("  Cold Fire: 2");
  Serial.println("  Test Pin: 3");
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
  int sensorValue = analogRead(fuelPressure2Sensor);
  float voltage = sensorValue * (5.0 / 1023.0);
  return voltage;
}

void printFuelPressure(void)
{
  Serial.print("FP: [");
  Serial.print(getFuelPressure());
  Serial.println("]");
}

void runTestPin(void)
{
  int counter = 0;
  while (counter < 10)
  {
    Serial.println("testPin34 HIGH");
    delay(1000);
    Serial.println("testPin34 LOW");
    delay(1000);
    counter = counter + 1;
  }
  Serial.println("testPin done");
  changeSystemState(idle);
}

void runTestLogger(void)
{
  int counter = 0;
  while (counter < 20)
  {
    logger();
    delay(100);
    logger();
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
      changeSystemState(testPin);
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
  logger();
  Serial.println("starting cold fire procedure");
  logger();
  delay(1000);

  // read sensors
  Serial.print("reading pressure sensors FP: [");
  Serial.print(getFuelPressure());
  Serial.println("]");
  delay(1000);

  // check if fuel solenoid is closed
  Serial.println("checking if fuel solenoid is closed");
  delay(1000);

  // check if oxidizer solenoid is closed
  Serial.println("checking if oxidizer solenoid is closed");
  delay(1000);

  // pressurize fuel tank
  Serial.println("pressurizing fuel tank, opening fill valve");
  delay(1000);
  digitalWrite(fillValveSolenoid, HIGH);
  counter = 0;

  while (counter < 10)
  {
    printFuelPressure();
    counter = counter + 1;
    delay(500);
  }
  digitalWrite(fillValveSolenoid, LOW);
  Serial.print("completed pressure fill ");
  printFuelPressure();
  delay(1000);

  // check pressure sensors
  Serial.print("checking if tank is pressurized ");
  printFuelPressure();
  delay(1000);

  // fire igniter
  Serial.println("firing igniter");
  digitalWrite(igniterSolenoidPin, HIGH);
  delay(1500);
  digitalWrite(igniterSolenoidPin, LOW);

  // open oxidizer solenoid for x seconds
  Serial.print("opening fuel solenoid 500 millis ");
  printFuelPressure();
  digitalWrite(fuelSolenoidPin, HIGH);
  delay(1500);
  digitalWrite(fuelSolenoidPin, LOW);
  Serial.print("closed fuel solenoid ");
  printFuelPressure();
  delay(100);
  Serial.print("opening oxidizer solenoid 500 millis ");
  printFuelPressure();
  digitalWrite(oxidizerSolenoidPin, HIGH);
  delay(500);
  digitalWrite(oxidizerSolenoidPin, LOW);
  Serial.print("closed oxidizer solenoid ");
  printFuelPressure();
  delay(5000);

  // check pressure sensors
  Serial.print("checking pressure sensors ");
  printFuelPressure();
  delay(1000);

  // depressurize fuel tank
  Serial.print("depressuring fuel tank ");
  printFuelPressure();
  float fuelPressureStart = getFuelPressure();
  unsigned long startTime = millis();

  digitalWrite(dumpValveSolenoid, HIGH);
  delay(1000);
  digitalWrite(dumpValveSolenoid, LOW);
  unsigned long endTime = millis() - startTime;
  float fuelPressureEnd = getFuelPressure();

  Serial.print("Fuel pressure difference: [");
  Serial.print(fuelPressureStart - fuelPressureEnd);
  Serial.print("] in [");
  Serial.print((endTime) / 1000);
  Serial.println("] seconds");

  // check sensors
  Serial.print("final pressure check ");
  printFuelPressure();
  delay(1000);

  // complete
  Serial.println("cold flow is complete setting state to idle");
  changeSystemState(idle);
  delay(1000);
  logger();
}