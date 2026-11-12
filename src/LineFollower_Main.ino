/**
 * Arduino Nano Line Follower Robot
 * 
 * Hardware Configuration:
 * - Arduino Nano (ATmega328P)
 * - TB6612FNG Dual H-Bridge Motor Driver
 * - 16x2 I2C Character LCD (A4=SDA, A5=SCL)
 * - 4x Tactile Push Buttons (D2=UP, D3=DOWN, D11=ENTER, D12=BACK)
 * - Line Sensor Array on A0-A3, A6, A7
 * - Buck Converter Regulated 5.0V Logic Supply to Nano 5V Pin
 * 
 * Safety:
 * - Motors powered solely through TB6612 VM from switched battery rail.
 * - Nano 5V pin receives regulated 5.0V from buck converter OUT+.
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

// Instantiate LCD Object (address 0x27, 16 chars, 2 lines)
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Robot States
enum RobotState {
  STATE_MENU,
  STATE_RUNNING,
  STATE_CALIBRATING,
  STATE_TEST_MOTORS,
  STATE_READ_SENSORS
};

RobotState currentState = STATE_MENU;

// Menu System Variables
const char* menuItems[] = {
  "1. Start Run",
  "2. Test Motors",
  "3. Sensor Check",
  "4. Calibrate"
};
const uint8_t TOTAL_MENU_ITEMS = 4;
int currentMenuIndex = 0;

// Button Debounce Timers
unsigned long lastButtonCheck = 0;
const unsigned long DEBOUNCE_DELAY = 180; // ms

// Motor Control Prototypes
void setupMotors();
void setMotors(int leftSpeed, int rightSpeed);
void stopMotors();
void brakeMotors();
void setupButtons();
bool isButtonPressed(uint8_t pin);
void updateMenuDisplay();
void handleMenuInput();
void executeLineFollowing();
void executeMotorTest();
void executeSensorCheck();

void setup() {
  Serial.begin(115200);
  Serial.println(F("--- Arduino Nano Line Follower Starting ---"));

  // 1. Initialize LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Nano LineFollow"));
  lcd.setCursor(0, 1);
  lcd.print(F("System Ready"));
  delay(1200);

  // 2. Initialize Push Buttons with internal pull-up resistors
  setupButtons();

  // 3. Initialize TB6612FNG Motor Driver Pins
  setupMotors();

  // 4. Initialize Line Sensor Analog Pins
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(SENSOR_PINS[i], INPUT);
  }
  // Note: A6 and A7 are dedicated analog inputs on ATmega328P and do not require pinMode.

  lcd.clear();
  updateMenuDisplay();
  Serial.println(F("Initialization complete. Entering UI loop."));
}

void loop() {
  switch (currentState) {
    case STATE_MENU:
      handleMenuInput();
      break;

    case STATE_RUNNING:
      executeLineFollowing();
      if (isButtonPressed(PIN_BTN_BACK)) {
        stopMotors();
        currentState = STATE_MENU;
        lcd.clear();
        updateMenuDisplay();
      }
      break;

    case STATE_TEST_MOTORS:
      executeMotorTest();
      currentState = STATE_MENU;
      lcd.clear();
      updateMenuDisplay();
      break;

    case STATE_READ_SENSORS:
      executeSensorCheck();
      if (isButtonPressed(PIN_BTN_BACK)) {
        currentState = STATE_MENU;
        lcd.clear();
        updateMenuDisplay();
      }
      break;

    case STATE_CALIBRATING:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Calibrating..."));
      delay(1500);
      currentState = STATE_MENU;
      lcd.clear();
      updateMenuDisplay();
      break;
  }
}

// ==========================================
// BUTTON INITIALIZATION & HANDLING
// ==========================================
void setupButtons() {
  // Configured as specified in Wiring Reference:
  pinMode(PIN_BTN_UP, INPUT_PULLUP);    // D2: UP
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);  // D3: DOWN
  pinMode(PIN_BTN_ENTER, INPUT_PULLUP); // D11: ENTER
  pinMode(PIN_BTN_BACK, INPUT_PULLUP);  // D12: BACK
}

bool isButtonPressed(uint8_t pin) {
  if (digitalRead(pin) == LOW) {
    if (millis() - lastButtonCheck > DEBOUNCE_DELAY) {
      lastButtonCheck = millis();
      return true;
    }
  }
  return false;
}

void updateMenuDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("MENU: [UP/DOWN]"));
  lcd.setCursor(0, 1);
  lcd.print(F("> "));
  lcd.print(menuItems[currentMenuIndex]);
}

void handleMenuInput() {
  if (isButtonPressed(PIN_BTN_DOWN)) {
    currentMenuIndex = (currentMenuIndex + 1) % TOTAL_MENU_ITEMS;
    updateMenuDisplay();
  } 
  else if (isButtonPressed(PIN_BTN_UP)) {
    currentMenuIndex = (currentMenuIndex - 1 + TOTAL_MENU_ITEMS) % TOTAL_MENU_ITEMS;
    updateMenuDisplay();
  } 
  else if (isButtonPressed(PIN_BTN_ENTER)) {
    lcd.clear();
    switch (currentMenuIndex) {
      case 0:
        currentState = STATE_RUNNING;
        lcd.print(F("Running..."));
        lcd.setCursor(0, 1);
        lcd.print(F("Press BACK stop"));
        break;
      case 1:
        currentState = STATE_TEST_MOTORS;
        break;
      case 2:
        currentState = STATE_READ_SENSORS;
        break;
      case 3:
        currentState = STATE_CALIBRATING;
        break;
    }
  }
}

// ==========================================
// TB6612FNG MOTOR DRIVER IMPLEMENTATION
// ==========================================
void setupMotors() {
  pinMode(PIN_TB_STBY, OUTPUT);
  pinMode(PIN_TB_PWMA, OUTPUT);
  pinMode(PIN_TB_PWMB, OUTPUT);
  pinMode(PIN_TB_AIN1, OUTPUT);
  pinMode(PIN_TB_AIN2, OUTPUT);
  pinMode(PIN_TB_BIN1, OUTPUT);
  pinMode(PIN_TB_BIN2, OUTPUT);

  // Enable the driver from standby mode
  digitalWrite(PIN_TB_STBY, HIGH);
  stopMotors();
}

/**
 * Control left and right motor speed and direction.
 * Positive speed = Forward, Negative speed = Reverse, 0 = Stop.
 */
void setMotors(int leftSpeed, int rightSpeed) {
  // Constrain speeds to allowable PWM limits
  leftSpeed = constrain(leftSpeed, -MAX_MOTOR_PWM, MAX_MOTOR_PWM);
  rightSpeed = constrain(rightSpeed, -MAX_MOTOR_PWM, MAX_MOTOR_PWM);

  // Left Motor (Motor A)
  if (leftSpeed > 0) {
    digitalWrite(PIN_TB_AIN1, HIGH);
    digitalWrite(PIN_TB_AIN2, LOW);
    analogWrite(PIN_TB_PWMA, leftSpeed);
  } else if (leftSpeed < 0) {
    digitalWrite(PIN_TB_AIN1, LOW);
    digitalWrite(PIN_TB_AIN2, HIGH);
    analogWrite(PIN_TB_PWMA, abs(leftSpeed));
  } else {
    digitalWrite(PIN_TB_AIN1, LOW);
    digitalWrite(PIN_TB_AIN2, LOW);
    analogWrite(PIN_TB_PWMA, 0);
  }

  // Right Motor (Motor B)
  if (rightSpeed > 0) {
    digitalWrite(PIN_TB_BIN1, HIGH);
    digitalWrite(PIN_TB_BIN2, LOW);
    analogWrite(PIN_TB_PWMB, rightSpeed);
  } else if (rightSpeed < 0) {
    digitalWrite(PIN_TB_BIN1, LOW);
    digitalWrite(PIN_TB_BIN2, HIGH);
    analogWrite(PIN_TB_PWMB, abs(rightSpeed));
  } else {
    digitalWrite(PIN_TB_BIN1, LOW);
    digitalWrite(PIN_TB_BIN2, LOW);
    analogWrite(PIN_TB_PWMB, 0);
  }
}

void stopMotors() {
  digitalWrite(PIN_TB_AIN1, LOW);
  digitalWrite(PIN_TB_AIN2, LOW);
  digitalWrite(PIN_TB_BIN1, LOW);
  digitalWrite(PIN_TB_BIN2, LOW);
  analogWrite(PIN_TB_PWMA, 0);
  analogWrite(PIN_TB_PWMB, 0);
}

void brakeMotors() {
  digitalWrite(PIN_TB_AIN1, HIGH);
  digitalWrite(PIN_TB_AIN2, HIGH);
  digitalWrite(PIN_TB_BIN1, HIGH);
  digitalWrite(PIN_TB_BIN2, HIGH);
  analogWrite(PIN_TB_PWMA, 0);
  analogWrite(PIN_TB_PWMB, 0);
}

// ==========================================
// BEHAVIORAL ROUTINES
// ==========================================
void executeMotorTest() {
  lcd.clear();
  lcd.print(F("Testing Motors"));

  // Forward
  lcd.setCursor(0, 1);
  lcd.print(F("Forward 30%..."));
  setMotors(100, 100);
  delay(1000);

  // Reverse
  lcd.setCursor(0, 1);
  lcd.print(F("Reverse 30%..."));
  setMotors(-100, -100);
  delay(1000);

  // Stop
  stopMotors();
  lcd.setCursor(0, 1);
  lcd.print(F("Test Complete!"));
  delay(1000);
}

void executeSensorCheck() {
  static unsigned long lastDisp = 0;
  if (millis() - lastDisp > 250) {
    lastDisp = millis();

    int valA0 = analogRead(A0);
    int valA1 = analogRead(A1);
    int valA2 = analogRead(A2);

    lcd.setCursor(0, 0);
    lcd.print(F("S0:"));
    lcd.print(valA0 / 10);
    lcd.print(F(" S1:"));
    lcd.print(valA1 / 10);

    lcd.setCursor(0, 1);
    lcd.print(F("S2:"));
    lcd.print(valA2 / 10);
    lcd.print(F(" [BACK]"));
  }
}

void executeLineFollowing() {
  // Placeholder line-following control algorithm
  // Reads analog line sensors and applies differential steering
  int sensorLeft = analogRead(A0);
  int sensorRight = analogRead(A3);

  int error = sensorLeft - sensorRight;
  int baseSpeed = DEFAULT_BASE_SPEED;
  int correction = error / 4;

  int leftMotorSpeed = baseSpeed - correction;
  int rightMotorSpeed = baseSpeed + correction;

  setMotors(leftMotorSpeed, rightMotorSpeed);
}
