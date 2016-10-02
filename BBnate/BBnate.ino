// BBnate by BoneMARK

// Import LCD library and define lcd object
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

/*
Using Sainsmart LCD Shield for Arduino

Button Legend Used:
None   - 'N' - 5.00 V
Select - 'A' - 3.62 V - used as the acquire button
Left   - 'L' - 2.45 V
Up     - 'U' - 0.70 V
Down   - 'D' - 1.60 V
Right  - 'R' - 0.00 V - used as the clear button
Reset  - 'S' - n/a

Digital Pin Usage:
Pin 12 - Transducer
Pin 11 - LED
Pin 03 - LED
Pin 02 - LED

*/

void setup() {
  // turn on power LED
  int greenLED = 11;
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, HIGH);
  
  // serial setup
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.clear();
  lcd.begin(16, 2);

  // welcome screen
  BBnateWelcome();
  
  delay(5000);
}

void loop() {

  // define "global" variables (global in the sense of this void loop)
  const int arrayLength = 20;
  const int delayTime = 500; // in ms, so half a second  

  // display instructions and wait for user input
  BBnateInstructions();

  char BUTTON;
  while (BUTTON != 'A') {
    BUTTON = ButtonPress();
  }
    
  // Turn on Yellow LED for taking measurement & LCD print measuring
  int YellowLED = 3;
  pinMode(YellowLED, OUTPUT);
  digitalWrite(YellowLED, HIGH);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Measuring...");
/*
  // Send ping to U/S transducer
  SendPing();
*/
  // Read in analog signal
  Serial.println("\nAnalog Values:");
  float v[arrayLength];
  float *v_p = v;
  v_p = ReadSignal();
  delay(2 * delayTime);

  // Sum up averaged values
  float v_sum;
  v_sum = MyIntegrate(v_p);
  Serial.print("\nThe Sum: ");
  Serial.println(v_sum);
  delay(2 * delayTime);

  // Calculate bone mineral density
  float BMD;
  BMD = CalculateBMD(v_sum);
  Serial.print("\nCalculated BMD: ");
  Serial.println(BMD);
  delay(2 * delayTime);
  
  Serial.println("\nDone!\n");
  digitalWrite(YellowLED, LOW);

  // LCD print
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Baby's BMD: ");
    // Turn on Red LED if error occurs
    if (BMD == 0.00) {
      int RedLED = 2;
      pinMode(RedLED, OUTPUT);
      lcd.print("n/a");  
      digitalWrite(RedLED, HIGH);
    }
    else {
      lcd.print(BMD);
    }
  delay(5 * delayTime);
  lcd.setCursor(0,1);
  lcd.print("Press \"CLEAR\"   ");
    // Turn off Red LED if error occurred
    if (BMD == 0.00) {
      delay(5 * delayTime);
      int RedLED = 2;
      digitalWrite(RedLED, LOW);
    }

  // wait for the clear button to be pressed to restart loop
  while (BUTTON != 'R') {
    BUTTON = ButtonPress();
  }
}

// DEFINE ALL MY FUNCTIONS

float *ReadSignal() {
  /*
     Reads in analog signals and continuously sums them in increments
     of 50 values to obatin a running list of sums.

     :return float v_signal: array containing cumulative sums of signal (V)
  */

  const int Length = 20; // manageable size for gathering enough data and saving memory
  const int WindowLength = 50; // cutoff of ~5 Hz desired and Arduino has sampling rate of 10000/s 
  const float Bit2Volt = 5.000 / 1023;

  int inputPin = A1;
  float v_signal = 0;
  float v_all[Length];

  for (int i = 0; i < Length; i++) {
    
    for (int j = 0; j < WindowLength; j++) {
      v_signal = v_signal + analogRead(inputPin) * Bit2Volt;
    }
    Serial.print("Sum: ");
    Serial.println(v_signal);
    v_all[i] = v_signal;
  }
  
  float voltage[Length];
  voltage[0] = v_all[0] / WindowLength;
  
  for(int i = 1; i < Length; i++) {
    voltage[i] = (v_all[i] - v_all[i - 1]) / WindowLength;
    Serial.print("Filtered Sum: ");
    Serial.println(voltage[i]);
  }

  return voltage;
}

float *CreateVoltage(float sums[20]) {
  /*
   * Creates an array of individual sums from an array of cumulative
   * sums, then averages out the values.
   * 
   * :param float sums: array of cumulative sums
   * :return float voltage: averaged voltage values (V)
   */
  
  const int Length = 20;
  const int WindowLength = 50;
  float voltage[Length];
  voltage[0] = sums[0] / WindowLength;
  
  for(int i = 1; i < Length; i++) {
    voltage[i] = (sums[i] - sums[i - 1]) / WindowLength;
    Serial.println(voltage[i]);
  }

  return voltage;
}

float *CreateTime(int input_signal[50], const int Fs) {
  /*
     Creates a time array corresponding to read-in analog signal

     :param int input_signal: the read-in analog signal
     :param int Fs: sampling frequency in samples/sec
     :return float time_array: time array in sec
  */

  const int Length = 50;
  const float dt = 1 / Fs;
  float time_array[Length];

  for (int i = 0; i < Length; i++) {
    time_array[i] = i * dt;
  }

  return time_array;
}

float MyIntegrate(float input[20]) {
  /*
     Sums up all values of an input array, ignoring any zero padding on
     both ends.

     :param int input: one dimensional array being integrated
     :return int sum: the calculated sum of array
  */
  const int Length = 20;/*
  const float threshold = 0.1;

  int loop_start = 0;
  int takeoff = 0;
  int loop_end;

  while (input[loop_start] <= threshold) {
    loop_start++;
  }
  Serial.print("Last Value: ");
  Serial.println(input[19]);
  while (input[Length - takeoff] <= threshold) {
    takeoff++;
  }
  Serial.print("Takeoff: ");
  Serial.println(takeoff);

  loop_end = Length - takeoff;

  Serial.print("Loop Start=");
  Serial.println(loop_start);
  Serial.print("Loop End=");
  Serial.println(loop_end);
  */
  
  float sum = 0.00;
  
  for (int i = 0; i < Length; i++) {
    sum = sum + input[i];
  }

  return sum;
}

float *LowPassFilter(int input_signal[50], float input_time[50], int cutoff = 10) {
  /*
     Filters out all frequencies of a given signal above a defined cutoff
     frequency via a convolution between the signal and a windowed square wave.

     :param int input_signal: signal to be filtered with values in volts
     :param int input_time: corresponding time array of the given signal in sec
     :param int cutoff: cutoff frequency in Hz
     :return float f_signal: the filtered signal, w/ length of input signal - window
  */

  int window_len =  10;

  int signal_len = sizeof(input_signal) / sizeof(input_signal[0]);

  int F_Length = signal_len - window_len + 1;

  float f_signal[41];

  for (int i = 0; i <= window_len - 1; i++) {
    f_signal[0] = f_signal[0] + input_signal[i];
  }

  for (int j = 1; j <= F_Length; j++) {
    f_signal[j] = f_signal[j - 1] + ((input_signal[j + window_len - 1] - input_signal[j - 1]) / window_len);
  }

  return f_signal;
}

float *Subtract(float minuend[], float subtrahend[]) {
  /*
     Subtracts an array (element-by-element) from the other.
     Although both arrays should be the same length, if they
     happen to be different the resulting array will be the
     same length as the shorter array and only the elements
     both minuend and subtrahend share will be used.

     :param float minuend: array that is being subtracted from
     :param float subtrahend: array that is being subtracted
     :return float difference: difference between arrays
  */
  int min_len = sizeof(minuend) / sizeof(minuend[0]);
  int sub_len = sizeof(subtrahend) / sizeof(subtrahend[0]);
  int len = min_len;

  min_len <= sub_len ? len = min_len : len = sub_len;
  const int diff_len = len;

  float difference[diff_len];

  for (int i = 0; i < diff_len; i++) {
    difference[i] = minuend[i] - subtrahend[i];
  }

  return difference;
}
/*
struct output {
  float BMD;
  bool Error;
};
*/
float CalculateBMD(float energy_T) {
  /*
     Determines bone mineral density depending on total energy
     Also turns on red LED if unexpected value is read in

     :param float energy_T: total energy of signal
     :return float bmd: calculated bone mineral density
  */

  float bmd = 0.00;
  const float Max = 17.00; // placeholder
  const float Mid1 = 12.00; // placeholder
  const float Mid2 = 8.00; // placeholder
  const float Min = 3.00; // placeholder

  if (energy_T > Max) {
    //Serial.println("HERE!");
  }
  else if (energy_T == Max) {
    bmd = Max / Max; // placeholder
    //Serial.println("HERE!");
  }
  else if (energy_T < Max && energy_T >= Mid1)  {
    bmd = (Max + Mid1) / 2 / Max; // placeholder
    //Serial.println("HERE!");
  }
  else if (energy_T < Mid1 && energy_T >= Mid2) {
    bmd = (Mid1 + Mid2) / 2 / Max; // placeholder
    //Serial.println("HERE!");
  }
  else if (energy_T < Mid2 && energy_T > Min) {
    bmd = (Mid2 + Min) / 2 / Max; // placeholder
    //Serial.println("HERE!");
  }
  else if (energy_T == Min) {
    bmd = Min / Max; // placeholder
    //Serial.println("HERE!");
  }
  else {
    //Serial.println("HERE!");
  }
  
  /*
  typedef struct output Output;
  Output decision;
  decision.BMD = bmd;
  decision.Error = error;
  
  return decision;
  */

  return bmd;
}

void Error(const int pin, const int onTime) {
  /*
   * Turns on a digital pin (intended to be connected to an LED)
   * if an error occurs
   * 
   * :param int pin: digital pin number
   * :param int onTime: the desired amount of time fot LED to be on (seconds)
   */

  pinMode(pin, OUTPUT);

  digitalWrite(pin, HIGH);
  delay(1000 * onTime);
  digitalWrite(pin, LOW);
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

void BBnateWelcome () {
  /*
   * Welcome screen for BBnate for a 16x2 LCD Arduino shield
   */
   
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Welcome to the ");
  lcd.setCursor(0,1);
  lcd.print("BBnate Interface");
}

void BBnateInstructions () {
  /*
   * Instructions for the BBnate for 15x2 LCD Arduino shield
   */
   
  String firstInstructionsA = "Press \"ACQUIRE\" ";
  String firstInstructionsB = "to take measure.";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(firstInstructionsA);
  lcd.setCursor(0,1);
  lcd.print(firstInstructionsB);
}

void SendPing () {
  /*
   * Setting a designated digital pin high than low to send a ping
   */

  const int ToTransducer = 12;
  pinMode(ToTransducer, OUTPUT);
  digitalWrite(ToTransducer, HIGH);
  digitalWrite(ToTransducer, LOW);  
}

