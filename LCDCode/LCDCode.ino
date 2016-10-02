/*
Using Sainsmart LCD Shield for Arduino

Button Legend Used:
None   - 'N' - 5.00 V
Select - 'A' - 3.62 V
Left   - 'L' - 2.45 V
Up     - 'U' - 0.70 V
Down   - 'D' - 1.60 V
Right  - 'R' - 0.00 V
Reset  - 'C' - n/a
*/

#include <LiquidCrystal.h>

//Pin assignments for SainSmart LCD Keypad Shield
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7); 
//---------------------------------------------
                 
void setup() { 
  
  // set up the LCD's number of columns and rows:
  lcd.clear();
  lcd.begin(16, 2);

  // welcome screen
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Welcome to the ");
  lcd.setCursor(0,1);
  lcd.print("BBnate Interface");
  delay(5000);

}

void loop() { 

  // display instructions and wait for user input
  String firstInstructionsA = "Press \"ACQUIRE\" ";
  String firstInstructionsB = "to take measure.";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(firstInstructionsA);
  lcd.setCursor(0,1);
  lcd.print(firstInstructionsB);/*
  delay(1000);
  lcd.clear();
  delay(1000);*/
  char Button;
  Button = ButtonPress();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Button pressed: ");
  lcd.setCursor(8,1);
  lcd.print(Button);
  delay(3000);
    
}

char ButtonPress() {
  /*
   * Waits for a button to be pressed and then returns a
   * corresponding character depending on what button is
   * pressed (using SainSmart LCD Keybad Shield v1.0)
   * 
   * :return char Button: character corresponding to button pressed
   */

  //initialize variables
  char Button = 'N';
  int inputPin = A0;
  float Raw;
  const float Bit2Volt = 5.00 / 1023;
  const float thr = 0.05;

  // define button voltages
  const float right = 0.00; // R
  const float up = 0.70; // U
  const float down = 1.60; // D
  const float left = 2.45; // L
  const float select = 3.62; // A (for Acquire)
  
  while (Button == 'N') {
    Raw = analogRead(inputPin) * Bit2Volt;
    
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

