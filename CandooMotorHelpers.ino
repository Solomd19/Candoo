void stepper(int xw) { // Drives the stepper for as many steps as xw indicates
  for (int x = 0; x < xw; x++) {

    switch (Steps) {
      case 0:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      case 1:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
        break;
      case 2:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 3:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 4:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 5:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 6:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 7:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      default:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
    }
    SetDirection();
  }
}

void SetDirection() { // Alters variable Steps based on direction of motor and current active phase
  if (Direction == 1) {
    Steps++;
  }
  if (Direction == 0) {
    Steps--;
  }
  if (Steps > 7) { // If incrementing at case 7 move to case 0
    Steps = 0;
  }
  if (Steps < 0) { // If decrementing at case 0 move to case 7
    Steps = 7;
  }
}

void closeLid() { // Closes iris diaphragm by controlling motor
  digitalWrite(EN, HIGH); // Enable motor power
  Direction = false;

  lcd.setCursor(2, 1);
  lcd.print("Closing Iris");

  for (int i = 0; i < closeSteps; i++) { // Run stepper for 4096 steps
    stepper(1);
    delay(3);
  }

  // Cut control signals to motor once it stops moving
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  digitalWrite(EN, LOW); // Disable motor power
}

void openLid() { // Opens iris diaphragm by controlling motor
  digitalWrite(EN, HIGH); // Enable motor power
  Direction = true;

  lcd.setCursor(2, 0);
  lcd.print("Extinguished");
  lcd.setCursor(2, 1);
  lcd.print("Opening Iris");

  for (int i = 0; i < closeSteps; i++) { // Run stepper for 4096 steps in opposite direction as closing
    stepper(1);
    delay(3);
  }

  // Cut control signals to motor once it stops moving
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  digitalWrite(EN, LOW); // Disable motor power
}

void motorAdjust() {
  
  while (redButtonState == LOW || blueButtonState == LOW || greenButtonState == LOW) { // Wait for R, G, B buttons to be unpressed
    redButtonState = digitalRead(redButton);
    blueButtonState = digitalRead(blueButton);
    greenButtonState = digitalRead(greenButton);
  }

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Motor Adjust");
  lcd.setCursor(1, 1);
  lcd.print("B=Close R=Open");

  while (greenButtonState == HIGH) { //Until green button is pressed, blue button toggles hour amount
    redButtonState = digitalRead(redButton);
    blueButtonState = digitalRead(blueButton);
    delay(1);

    if (redButtonState ^ blueButtonState) {
      if (redButtonState == LOW) {
        Direction = true;
      } else if (blueButtonState == LOW) {
        Direction = false;
      }
      stepper(1);
      delay(1);
    }

    delay(1);
    greenButtonState = digitalRead(greenButton);
  }

  while (redButtonState == LOW || blueButtonState == LOW || greenButtonState == LOW) { // Wait for R, G, B buttons to be unpressed
    redButtonState = digitalRead(redButton);
    blueButtonState = digitalRead(blueButton);
    greenButtonState = digitalRead(greenButton);
  }
}
