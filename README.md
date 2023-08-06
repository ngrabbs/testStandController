# testStandController
code for a test stand

# Goal:
initiate a test and record data from a safe distance.

To do this we have a test stand, on the test stand is a test engine, fuel tank, oxidizer tank and potentially another source of pressure.

There are physical buttons that will enable a fuel or oxidizer solenoid for low pressure test fires for leaks and an arming switch.

The main controller at the test stand is an ardunio mega and a raspberry pi that will be connected over spi.  The raspberry pi acts as a wireless
base station so that a remote laptop can connect and issue commands over SSH to control the tests being performed and recorded by the arduino.

# Power
There is a 12volt car battery that can fire the solenoids, relays, and will server to power the arduino as well.
The relay is an MSD part number 7564 https://documents.holley.com/frm34288_7564_75643_lowres.pdf which contains four solid state relays.
This relay board may need to change as the design goes on due to grounding issues with the arduino.  TBD

# Tests
We should be able to do a variety of tests such as cold fire, hot fire, cold flow and various incremental pressure tests.

A hot fire should be fully automated, meaning the controller should automatically pressurize the tanks, light the igniter, start the motor, run and record data for x amount of time, shut the motor down, depressureize, and make everything safe.

It should also have parameters for safetey checks, if something goes over pressure or abnoral the run should abort and safe sequence should start.

# Resources

raspberry pi -> arduino spi stuff
https://roboticsbackend.com/introduction-to-wiringpi-for-raspberry-pi/
https://roboticsbackend.com/raspberry-pi-master-arduino-uno-slave-spi-communication-with-wiringpi/

state machines:
https://forum.arduino.cc/t/state-machines/580593

info on sensor calibration:
https://forum.arduino.cc/t/heavy-psi-air-pressure-sensor-reader/350154/8
```Try this.
Change the offset value (second line) to the number you get in the serial monitor without pressure (zeroing).
Adjust the "fullScale" value for the right readout at full pressure (calibration).

If that works, think about adding "smoothing" for more stable readings.
Leo..```
```int rawValue; // A/D readings
int offset = 102; // zero pressure adjust
int fullScale = 922; // max pressure (span) adjust
float pressure; // final pressure

void setup() {
  Serial.begin(9600);
}

void loop() {
  rawValue = analogRead(A0);
  Serial.print("Raw A/D is  ");
  Serial.print(rawValue);
  pressure = (rawValue - offset) * 1.2 / (fullScale - offset); // pressure conversion
  Serial.print("   The pressure is  ");
  Serial.print(pressure, 3); // three decimal places
  Serial.println("  Mpa");
  delay(500);
}```