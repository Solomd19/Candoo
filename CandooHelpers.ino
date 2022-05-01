void startupJingle() { // Small jingle which plays at startup
  tone(buzzer, 523); //C5
  delay(100);
  noTone(buzzer);
  delay(100);

  tone(buzzer, 659); //E5
  delay(100);
  noTone(buzzer);
  delay(100);

  tone(buzzer, 392); //G4
  delay(100);
  noTone(buzzer);
  delay(100);

  tone(buzzer, 493); //B4
  delay(100);
  noTone(buzzer);
  delay(100);

  tone(buzzer, 523); //C5
  delay(100);
  noTone(buzzer);
}

void updateHomeScreen() { // Updates display based on if timer is set or not
  if (timerMode == true && time > 0) { // If timer mode is active and time isn't up yet...
    // Print that device is still in timer mode
    lcd.setCursor(3, 0);  //Set cursor to character 2 on line 0
    lcd.print("Timer Mode");

    // Calculate hours and minutes left to display
    hrs = time / 3600;
    mins = ((time % 3600) / 60);

    lcd.setCursor(6, 1);
    lcd.print(hrs);
    lcd.setCursor(7, 1);
    lcd.print(":");

    if (mins < 10) {
      lcd.setCursor(8, 1);
      lcd.print("0");
      lcd.setCursor(9, 1);
      lcd.print(mins);
    } else {
      lcd.setCursor(8, 1);
      lcd.print(mins);
    }
  } else if (timerMode == true && time <= 0) { // If timer mode is active and time is up...
    state = 1; // Normal shut off state (no alarm)
    time = 0;
    timerMode = false; // Turn off timer mode since time is up
  } else { // Otherwise, display button directions
    lcd.setCursor(1, 0);
    lcd.print("R = Extinguish");

    lcd.setCursor(1, 1);
    lcd.print("B=Time G=Motor");
  }
}

void timeEdit() { // Edit and submit time for timer mode
  while (blueButtonState == LOW || greenButtonState == LOW) { // Wait for both B and G buttons to be unpressed
    blueButtonState = digitalRead(blueButton);
    greenButtonState = digitalRead(greenButton);
  }

  // Initialize time selection vars
  int hourSelect = 0;
  int minSelect = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Timer Hours:");

  lcd.setCursor(6, 1);
  lcd.print(hourSelect);
  lcd.setCursor(7, 1);
  lcd.print(":00");

  while (greenButtonState == HIGH) { //Until green button is pressed, blue button toggles hour amount
    blueButtonState = digitalRead(blueButton);
    delay(100);

    if (blueButtonState == LOW) {
      if (hourSelect >= maxHrs) {
        hourSelect = 0;
      } else {
        hourSelect++;
      }

      //lcd.clear();
      lcd.setCursor(6, 1);
      lcd.print(hourSelect);
      lcd.setCursor(7, 1);
      lcd.print(":00");
    }


    delay(100);
    greenButtonState = digitalRead(greenButton);
  }

  while (blueButtonState == LOW || greenButtonState == LOW) { // Wait for both B and G buttons to be unpressed
    blueButtonState = digitalRead(blueButton);
    greenButtonState = digitalRead(greenButton);
  }

  lcd.clear();
  lcd.setCursor(0, 0);  //Set cursor to character 2 on line 0
  lcd.print("Set Timer Mins:");

  lcd.setCursor(6, 1);  //Set cursor to character 2 on line 0
  lcd.print(hourSelect);
  lcd.setCursor(7, 1);  //Set cursor to character 2 on line 0
  lcd.print(":00");

  if (hourSelect >= maxHrs) {
    minSelect = 0;
  } else {
    while (greenButtonState == HIGH) { //Until green button is pressed, blue button toggles minute amount
      blueButtonState = digitalRead(blueButton);
      delay(100);

      if (blueButtonState == LOW) {
        if (minSelect >= 45) {
          minSelect = 0;
        } else {
          minSelect = minSelect + 15;
        }

        //lcd.clear();
        lcd.setCursor(6, 1);
        lcd.print(hourSelect);
        lcd.setCursor(7, 1);
        lcd.print(":");

        if (minSelect == 0) {
          lcd.setCursor(8, 1);
          lcd.print("00");
        } else {
          lcd.setCursor(8, 1);
          lcd.print(minSelect);
        }

      }

      delay(100);
      greenButtonState = digitalRead(greenButton);
    }
  }


  while (blueButtonState == LOW || greenButtonState == LOW) { // Wait for both B and G buttons to be unpressed
    blueButtonState = digitalRead(blueButton);
    greenButtonState = digitalRead(greenButton);
  }

  time = (hourSelect * 60 * 60) + (minSelect * 60); // Calculate time in seconds based on hours and minutes chosen

  if (time > 0) { // If time selected is 0:00, don't turn on timer mode
    timerMode = true;
  } else {
    timerMode = false;
  }

}

void extinguishSense() {

lightVal = analogRead(LIGHT_SENSE_INPUT);

  // Print wait message on LCD
  lcd.setCursor(2, 0);
  lcd.print("Waiting for");
  lcd.setCursor(1, 1);
  lcd.print("Extinguish...");

//  lcd.setCursor(0, 0); //Leaving for debugging
//  lcd.print(lightCalCurrent);
//  lcd.setCursor(0, 1);
//  lcd.print(lightCalStartup);
//
//  lcd.setCursor(8, 1);
//  lcd.print(lightVal);


  while (lightVal >= lightCalCurrent - 100) { //condition for if it is still light - lower = more sensitive
//    if (lightVal > lightCal + 25) { //Keep for possible future update
//      calibrateLight();
//    }
    if (EMERGENCY_MODE == true) {
      // Sound an ambulance-like alarm
      for (int i = 200; i < 1000; i++) {
        tone(buzzer, i);
        delay(1);
      }

      for (int j = 1000; j > 200; j--) {
        tone(buzzer, j);
        delay(1);
      }
    }
     lightVal = calibrateLight();
  }
  lightJingle();
  lcd.clear();
}

int calibrateLight() { //Calibrate light sensor default value
  int total = 0;

  for (int i = 0; i < 10; i++) { //Take 10 values and average
    total += analogRead(LIGHT_SENSE_INPUT);
    delay(1);
  }
  return total / 10;
}

void lightJingle() {
  tone(buzzer, 392); //G4
  delay(500);

  tone(buzzer, 495); //B4
  delay(500);

  tone(buzzer, 587); //D5
  delay(500);

  tone(buzzer, 784); //G5
  delay(500);

  noTone(buzzer);
}
