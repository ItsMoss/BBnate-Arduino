/* \file BBnate.h
 * @author Moseph Jackson-Atogi
 *  
 * Header file for library for the BBnate by BoneMARK
 * Created by Moseph Jackson-Atogi
 * Last edited April 24, 2016
 */

#ifndef BBnate_h
#define BBnate_h

#include "Arduino.h"

// Import LCD library and define lcd object
#include <LiquidCrystal.h>

/**
 * \class BBnate
 * 
 * Class for the BBnate by BoneMARK that allows for the user to
 * define digital pin assignment for overall device implementation
 * 
 * @param int transducerPin: digital pin connected to transducer
 * @param int greenLED: digital pin connected to green LED
 * @param int yellowLED: digital pin connected to yellow LED
 * @param int redLED: digital pin connected to red LED
 */
class BBnate{

  public:
    BBnate(int transducerPin, int greenLED, int yellowLED, int redLED);
    /**
     * \fn LCDSetup
     * 
     * Clears cursor and sets dimensions for LCD screen in preparation
     * for printing later
     */
    void LCDSetup();
    /**
     * \fn Welcome
     * 
     * Welcome screen for BBnate for a 16x2 LCD Arduino shield
     */
    void Welcome();
    /**
     * \fn Instructions
     * 
     * Instructions for the BBnate for 15x2 LCD Arduino shield
     */
    void Instructions();
    /**
     * \fn PowerOn
     * 
     * Turns on the LED for power
     */
    void PowerOn();
    /**
     * \fn MeasureOn
     * 
     * Turns on the LED for when a measurement is being taken
     */
    void MeasureOn();
    /**
     * \fn MeasureOff
     * 
     * Turns off the LED for when a measurement is being taken
     */
    void MeasureOff();
    /**
     * \fn ButtonPress
     * 
     * Waits for a button to be pressed and then returns a
     * corresponding character depending on what button is
     * pressed (using SainSmart LCD Keybad Shield v1.0)
     * 
     * Button Legend Used: \n
     * None   - 'N' - 5.00 V \n
     * Select - 'A' - 3.62 V - used as the acquire button \n
     * Left   - 'L' - 2.45 V \n
     * Up     - 'U' - 0.70 V \n
     * Down   - 'D' - 1.60 V \n
     * Right  - 'R' - 0.00 V - used as the clear button \n
     * Reset  - 'S' - n/a \n
     * 
     * @return char Button: character corresponding to button pressed
     */
    char ButtonPress();
    /**
     * \fn SendPing
     * 
     * Setting a designated digital pin high than low to send a ping
     */
    void SendPing();
    /**
     * \fn ReadSignal
     * 
     * Reads in analog signals and continuously sums them in increments
     * of 50 values to obatin a running list of sums.
     * 
     * @return float voltage: array containing cumulative sums of signal (V)
     */ 
    float* ReadSignal();
    /**
     * \fn MyIntegrate
     * 
     * Sums up all values of an input array, ignoring any zero padding on
     * both ends.
     *
     * @param int array: one dimensional array being integrated
     * @return int sum: the calculated sum of array
     */
    float MyIntegrate(float array[20]);
    /**
     * \fn CalculateBMD
     * 
     * Determines bone mineral density depending on total energy
     * Also turns on red LED if unexpected value is read in
     * 
     * @param float sum: total sum of signal
     * @return float bmd: calculated bone mineral density
     */
    float CalculateBMD(float sum);
    /**
     * \fn PrintBMD
     * 
     * Prints the calculated BMD to LCD, unless an expected value is
     * calculated. If this occurs the ErrorOn function runs.
     * 
     * @param float bmd: calculated bone mineral density
     */
    void PrintBMD(float bmd);
    /**
     * \fn PrintClear
     * 
     * Prints instructions for user to clear screen for retaking 
     * measurements
     */
    void PrintClear();
    /**
     * \fn ErrorOff
     * 
     * Turns off the LED for when an error occurs
     */
    void ErrorOff();
    
     /*! \brief 'delayTime' is an int referring to time in ms to delay program */
    int delayTime;

    /**
     * \fn ErrorOn (private)
     * 
     * Turns on the LED for when an unexpected BMD is calculated, and
     * prints 'n/a' to LCD screen
     */
     
    /*! \brief *IMPORTANT NOTE*
    Both ErrorOn() and ErrorOff() are only called if an error occurs in CalculateBMD() */
    
  private:
    void ErrorOn();
    
    // int _analogPin;
    int _transducerPin;
    int _greenLED;
    int _yellowLED;
    int _redLED;
};

#endif
