/* \file BBnate.cpp
 * @author Moseph Jackson-Atogi
 *  
 * Source file for library for the BBnate by BoneMARK
 * Created by Moseph Jackson-Atogi
 * Last edited April 24, 2016
 */
 
#include "Arduino.h"
#include "BBnate.h"

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

/** \class BBnate
 *  
 *  This class is designed specifically for use of the BBnate by BoneMARK
 *  with an Arduino Uno microcontroller and the SainSmart LCD Shield. 
 *  Different instances of this class have the flexibility of choosing
 *  which digital pins are connected to the transducer and LEDs.
 *  
 *  @param int transducerPin: digital pin number connected to transducer
 *  @param int greenLED: digital pin number connected to green (power) LED
 *  @param int yellowLED: digital pin number connected to yellow (measurement) LED
 *  @param int redLED: digital pin number connected to red (error) LED
 */
BBnate::BBnate(int transducerPin, int greenLED, int yellowLED, int redLED) {

  // _analogPin = analogPin;
  
  pinMode(transducerPin, OUTPUT);
  _transducerPin = transducerPin;
  
  pinMode(greenLED, OUTPUT);
  _greenLED = greenLED;
  
  pinMode(yellowLED, OUTPUT);
  _yellowLED = yellowLED;
  
  pinMode(redLED, OUTPUT);
  _redLED = redLED;

}

/**
 * \fn LCDSetup
 * 
 * Clears cursor and sets dimensions for LCD screen in preparation
 * for printing later
 */
void BBnate::LCDSetup () {

  lcd.clear();
  lcd.begin(16, 2);   
}

/**
 * \fn Welcome
 * 
 * Welcome screen for BBnate for a 16x2 LCD Arduino shield
 */
void BBnate::Welcome () {
   
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Welcome to the ");
  lcd.setCursor(0,1);
  lcd.print("BBnate Interface");
}

/**
 * \fn Instructions
 * 
 * Instructions for the BBnate for 15x2 LCD Arduino shield
 */
void BBnate::Instructions () {
   
  String firstInstructionsA = "Press \"ACQUIRE\" ";
  String firstInstructionsB = "to take measure.";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(firstInstructionsA);
  lcd.setCursor(0,1);
  lcd.print(firstInstructionsB);
}

/**
 * \fn PowerOn
 * 
 * Turns on the LED for power
 */
void BBnate::PowerOn () {

  digitalWrite(_greenLED, HIGH);
}

/**
 * \fn MeasureOn
 * 
 * Turns on the LED for when a measurement is being taken
 */
void BBnate::MeasureOn () {

  digitalWrite(_yellowLED, HIGH);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Measuring...");
}

/**
 * \fn MeasureOff
 * 
 * Turns off the LED for when a measurement is being taken
 */
void BBnate::MeasureOff () {

  digitalWrite(_yellowLED, LOW);
}

/**
 * \fn ErrorOn
 * 
 * Turns on the LED for when an unexpected BMD is calculated, and
 * prints 'n/a' to LCD screen
 */
void BBnate::ErrorOn () {

  digitalWrite(_redLED, HIGH);
  lcd.print("n/a");
}

/**
 * \fn ErrorOff
 * 
 * Turns off the LED for when an error occurs
 */
void BBnate::ErrorOff () {
   
  digitalWrite(_redLED, LOW);
}

/**
 * \fn ButtonPress
 * 
 * Waits for a button to be pressed and then returns a
 * corresponding character depending on what button is
 * pressed (using SainSmart LCD Keybad Shield v1.0)
 * 
 * Button Legend Used:
 * None   - 'N' - 5.00 V
 * Select - 'A' - 3.62 V - used as the acquire button
 * Left   - 'L' - 2.45 V
 * Up     - 'U' - 0.70 V
 * Down   - 'D' - 1.60 V
 * Right  - 'R' - 0.00 V - used as the clear button
 * Reset  - 'S' - n/a
 * 
 * @return char Button: character corresponding to button pressed
 */
char BBnate::ButtonPress() {
  
  //initialize variables
  int buttonPin = A0;
  char Button = 'N';
  float Raw;
  const float thr = 0.05;
  float Bit2Volt = 5.00 / 1023;

  // define button voltages
  const float right = 0.00; // R
  const float up = 0.70; // U
  const float down = 1.60; // D
  const float left = 2.45; // L
  const float select = 3.62; // A (for Acquire)
  
  while (Button == 'N') {
    Raw = analogRead(buttonPin) * Bit2Volt;
    
    if (Raw >= right - thr && Raw <= right + thr) {
      Button = 'R';
    }
    else if (Raw >= up - thr && Raw <= up + thr) {
      Button = 'U';
    }
    else if (Raw >= down - thr && Raw <= down + thr) {
      Button = 'D';
    }
    else if (Raw >= left - thr && Raw <= left + thr) {
      Button = 'L';
    }
    else if (Raw >= select - thr && Raw <= select + thr) {
      Button = 'A';
    }
  }

  return Button;
}

/**
 * \fn SendPing
 * 
 * Setting a designated digital pin high than low to send a ping
 */
void BBnate::SendPing () {
   
  digitalWrite(_transducerPin, HIGH);
  delay(5);
  digitalWrite(_transducerPin, LOW);  
}

/**
 * \fn ReadSignal
 * 
 * Reads in analog signals and continuously sums them in increments
 * of 50 values to obatin a running list of sums.
 * 
 * @return float voltage: array containing cumulative sums of signal (V)
 */ 
float* BBnate::ReadSignal() {

  int arraylen = 20;
  int windowlen = 50;
  float Bit2Volt = 5.00 / 1023;
  int analogPin = A1;
  
  float v_signal = 0;
  float v_all[arraylen];

  for (int i = 0; i < arraylen; i++) {
    
    for (int j = 0; j < windowlen; j++) {
      v_signal = v_signal + analogRead(analogPin) * Bit2Volt;
    }
    Serial.print("Sum: ");
    Serial.println(v_signal);
    v_all[i] = v_signal;
  }
  
  float voltage[arraylen];
  voltage[0] = v_all[0] / windowlen;
  
  for(int i = 1; i < arraylen; i++) {
    voltage[i] = (v_all[i] - v_all[i - 1]) / windowlen;
    Serial.print("Filtered Sum: ");
    Serial.println(voltage[i]);
  }

  return voltage;
}

/**
 * \fn MyIntegrate
 * 
 * Sums up all values of an input array, ignoring any zero padding on
 * both ends.
 *
 * @param int array: one dimensional array being integrated
 * @return int sum: the calculated sum of array
 */
float BBnate::MyIntegrate(float array[20]) {

  const int Length = 20;
  float total = 0.00;
  
  for (int i = 0; i < Length; i++) {
    total = total + array[i];
  }

  return total;
}

/**
 * \fn CalculateBMD
 * 
 * Determines bone mineral density depending on total energy
 * Also turns on red LED if unexpected value is read in
 * 
 * @param float sum: total sum of signal
 * @return float bmd: calculated bone mineral density
 */
float BBnate::CalculateBMD(float sum) {

  float bmd = 0.00;
  const float Max = 17.00; // placeholder
  const float Mid1 = 12.00; // placeholder
  const float Mid2 = 8.00; // placeholder
  const float Min = 3.00; // placeholder

  if (sum > Max) {
    //Serial.println("HERE!");
  }
  else if (sum == Max) {
    bmd = Max / Max; // placeholder
    //Serial.println("HERE!");
  }
  else if (sum < Max && sum >= Mid1)  {
    bmd = (Max + Mid1) / 2 / Max; // placeholder
    //Serial.println("HERE!");
  }
  else if (sum < Mid1 && sum >= Mid2) {
    bmd = (Mid1 + Mid2) / 2 / Max; // placeholder
    //Serial.println("HERE!");
  }
  else if (sum < Mid2 && sum > Min) {
    bmd = (Mid2 + Min) / 2 / Max; // placeholder
    //Serial.println("HERE!");
  }
  else if (sum == Min) {
    bmd = Min / Max; // placeholder
    //Serial.println("HERE!");
  }
  else {
    //Serial.println("HERE!");
  }

  return bmd;
}

/**
 * \fn PrintBMD
 * 
 * Prints the calculated BMD to LCD, unless an expected value is
 * calculated. If this occurs the ErrorOn function runs.
 * 
 * @param float bmd: calculated bone mineral density
 */
void BBnate::PrintBMD (float bmd) {
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Baby's BMD: ");
  
  // Turn on Red LED if error occurs
  if (bmd == 0.00) {
    ErrorOn();
  }
  else {
    lcd.print(bmd);
  }
}

/**
 * \fn PrintClear
 * 
 * Prints instructions for user to clear screen for retaking 
 * measurements
 */
void BBnate::PrintClear () {
  
  lcd.setCursor(0,1);
  lcd.print("Press \"CLEAR\"   ");
}

