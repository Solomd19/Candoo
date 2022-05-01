/*
   Title: Candoo
   Author: Drew Solomon
   Last Updated: 5/1/22 06:03 PM :)
   Purpose: Code for an automatic candle extinguisher using an Arduino Uno/Nano
*/

// Libraries
#include <LiquidCrystal_I2C.h>
#include <SparkFunLIS3DH.h>
#include <Wire.h>

// Pin Assignments
#define redButton 3 //red button input to arduino pin D3
#define blueButton 4 //blue button input to arduino pin D4
#define greenButton 5 //green button input to arduino pin D5
#define buzzer 6 //buzzer to arduino pin D6

#define IN4 9 //motor input 4 to arduino pin D9
#define IN3 10 //motor input 3 to arduino pin D10
#define IN2 11 //motor input 2 to arduino pin D11
#define IN1 12 //motor input 1 to arduino pin D12
#define EN 13 //motor enable to arduino pin D13

#define LIGHT_SENSE_INPUT 1 //light sensor data input to arduino pin A1


// Global Variables
static unsigned int state = 0; //Starting state

static unsigned long time = 0; //Seconds left in timer mode
static unsigned long hrs = 0; //Hours left in timer mode
static unsigned long mins = 0; //Minutes left in timer mode
int maxHrs = 4; //Longest amount of hours that can be set for timer
static unsigned long newTime, oldTime; //Used for reference in decrementing time
boolean timerMode = false; //Indicates if timer mode is active
boolean EMERGENCY_MODE = false; //Indicates if alarm should be set off

int Steps = 0; //Indicates current motor phase state
int closeSteps = 5700; //Amount of steps to take to fully close/open iris
boolean Direction = true; //Direction motor will spin

// Button States - active LOW
int redButtonState;
int blueButtonState;
int greenButtonState;

// Light sensor variables
int lightCalStartup;
int lightCalCurrent;
int lightVal;

//Accelerometer variables
float xCal;
float yCal;
float zCal;
float sensitivity = .5; // Lower = more sensitivity

// Initialize LCD Library
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x3F for a 16 chars and 2 line display

LIS3DH myIMU( I2C_MODE, 0x19 ); //Default constructor is I2C, addr 0x19.


void setup() { // put your setup code here, to run once:

  //Initialize buttons as pullup buttons - no extra resistors needed
  pinMode(redButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);

  lcd.init(); // Initialize LCD more
  lcd.clear(); // Clear LCD text
  lcd.backlight(); // Make sure backlight is on

  // Print startup message on LCD
  lcd.setCursor(5, 0);  //Set cursor to character 5 on line 0
  lcd.print("Candoo");
  lcd.setCursor(4, 1);
  lcd.print("By Team 4");

  // Set buzzer as an output at pin D2
  pinMode(buzzer, OUTPUT);

  // Set motor enable as an output at pin D8
  pinMode(EN, OUTPUT);

  // Set motor phases as outputs from pin D9-12
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  //Set light sensor data input to pin A0
  pinMode(LIGHT_SENSE_INPUT, INPUT);

  //Call .begin() to configure the IMU
  myIMU.begin();

  startupJingle(); //Play startup jingle

  delay(1000); //Wait a moment before moving on to start menu

  lcd.clear(); //Wipe the LCD display

  lightCalStartup = calibrateLight(); // Calibrate initial light sensor default value (replaced when lid closes)

  // Calibrate starting x, y, and z axis values
  xCal = myIMU.readFloatAccelX();
  yCal = myIMU.readFloatAccelY();
  zCal = myIMU.readFloatAccelZ();

  lcd.clear(); //Wipe the LCD display

}

void loop() { // put your main code here, to run repeatedly:

  if (myIMU.readFloatAccelX() > xCal + sensitivity || myIMU.readFloatAccelX() < xCal - sensitivity
      || myIMU.readFloatAccelY() > yCal + sensitivity || myIMU.readFloatAccelY() < yCal - sensitivity
      || myIMU.readFloatAccelZ() > zCal + sensitivity || myIMU.readFloatAccelZ() < zCal - sensitivity) {
    state = 2; // Emergency shut off state (has alarm)
  }

  // Update button readings each loop
  redButtonState = digitalRead(redButton);
  blueButtonState = digitalRead(blueButton);
  greenButtonState = digitalRead(greenButton);

  // State Machine
  switch (state) {
    case 0: // Standby state

      // If a second has passed, decrement time
      if (time > 0) {
        newTime = millis() / 1000;
        if (newTime > oldTime) {
          time--;
        }
        oldTime = newTime;
      }

      // If button input has occured, move to appropriate state
      if (redButtonState == LOW) {
        state = 1; // Normal shut off state (no alarm)
      } else if (blueButtonState == LOW) {
        state = 5; // Timer edit state
      } else if  (greenButtonState == LOW) {
        state = 6; // Motor adjust state
      }

      updateHomeScreen(); // Updates display based on if timer is set or not

      break;
    case 1: // Normal shut off state (no alarm)

      lightCalCurrent = calibrateLight(); //Calculate current amount of light in the enclosure

      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("EXTINGUISHING");

      closeLid(); // Control motor to close lid

      lcd.clear();
      state = 4; // Light sensing state

      break;
    case 2: // Emergency shut off state (has alarm)

      lightCalCurrent = calibrateLight(); //Calculate current amount of light in the enclosure

      // Display emergency message
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("EMERGENCY  CLOSE");

      EMERGENCY_MODE = true;

      tone(buzzer, 1000);
      closeLid(); // Control motor to close lid

      lcd.clear();
      state = 4; // Light sensing state
      break;
    case 3: // Opening lid state

      openLid(); // Control motor to open lid

      noTone(buzzer); //Cut the alarm
      EMERGENCY_MODE = false;
      lcd.clear();
      state = 0; // Standby state

      break;
    case 4: // Light sensing state

      extinguishSense();

      lcd.clear();
      state = 3; // Opening lid state
      break;
    case 5: // Timer edit state

      timeEdit(); // Edit and submit time for timer mode

      lcd.clear();
      state = 0; // Standby state
      break;
    case 6: // Motor adjust state

      digitalWrite(EN, HIGH); // Enable motor power
      motorAdjust(); // Adjust motor starting position
      digitalWrite(EN, LOW); // Disable motor power

      lcd.clear();
      state = 0; // Standby state
      break;
  }
}
