#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Rotary.h>
#include <PID_v1.h>
#define TACHO 3 // tacho signals input pin
#define DETECT 2 // zero cross detect pin
#define GATE 17 // TRIAC gate pin
#define RANGE1 9 // range one switch pin
#define RANGE2 10 // range two switch pin
#define BUTTON 4 // rottary encoder button pin
#define RELAY 5 // relay pin
#define PULSE 2 // number of triac trigger pulse width counts. One count is 16 microseconds
#define TACHOPULSES 8 // number of pulses per revolution
void setup() {
Serial.begin(115200);
// set up pins
pinMode(BUTTON, INPUT); // set the button pin
pinMode(RELAY, OUTPUT); // set the relay pin
pinMode(DETECT, INPUT); // set the zero cross detect pin
pinMode(GATE, OUTPUT); // set the TRIAC gate control pin
pinMode(TACHO, INPUT); // set the tacho pulses detect pin
pinMode(RANGE1, INPUT); // set the range 1 switch pin
pinMode(RANGE2, INPUT); // set the range 1 switch pin
digitalWrite(BUTTON, HIGH); // turn on pullup resistors
digitalWrite(RANGE1, HIGH); // turn on pullup resistors
digitalWrite(RANGE2, HIGH); // turn on pullup resistors
digitalWrite(RELAY, relayState); // initialize relay output
counterR1 = minrpmR1; // assign start value for range 1
counterR2 = minrpmR2; // assign start value for range 2
Input = 200; // asiign initial value for PID
Setpoint = 200; // asiign initial value for PID
//turn the PID on
myPID.SetMode(AUTOMATIC);
myPID.SetOutputLimits(minoutputlimit, maxoutputlimit);
myPID.SetSampleTime(sampleRate); // Sets the sample rate
// set up Timer1
OCR1A = 100; // initialize the comparator
TIMSK1 = 0x03; // enable comparator A and overflow interrupts
TCCR1A = 0x00; // timer control registers set for
TCCR1B = 0x00; // normal operation, timer disabled
attachInterrupt(0, zeroCrossingInterrupt, RISING);
attachInterrupt(1, tacho, FALLING);
lcd.init(); // initialize the lcd
lcd.backlight(); // turn on the backlight
// check the RPM range state at startup and display it
int rangeOne = digitalRead(RANGE1);
int rangeTwo = digitalRead(RANGE2);
if (rangeOne == 1 && rangeTwo == 1) {
range = 0;
range0();
}
if (rangeOne == 0 && rangeTwo == 1) {
range = 1;
RPMrange1();
}
if (rangeOne == 1 && rangeTwo == 0) {
range = 2;
RPMrange2();
}
}
// Interrupt Service Routines
void zeroCrossingInterrupt() { // zero cross detect
TCCR1B = 0x04; // start timer with divide by 256 input
TCNT1 = 0; // reset timer - count from zero
OCR1A = dimming; // set the compare register brightness desired.
}
ISR(TIMER1_COMPA_vect) { // comparator match
if (startflag == true) { // flag for start up delay
digitalWrite(GATE, HIGH); // set TRIAC gate to high
TCNT1 = 65536 - PULSE; // trigger pulse width
}
}