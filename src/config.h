#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// PUSH BUTTON PIN ASSIGNMENTS (INPUT_PULLUP)
// ==========================================
const uint8_t PIN_BTN_UP    = 2;   // UP Menu Button
const uint8_t PIN_BTN_DOWN  = 3;   // DOWN Menu Button
const uint8_t PIN_BTN_ENTER = 11;  // ENTER Menu Button
const uint8_t PIN_BTN_BACK  = 12;  // BACK Menu Button

// ==========================================
// TB6612FNG MOTOR DRIVER PIN ASSIGNMENTS
// ==========================================
const uint8_t PIN_TB_STBY = 4;   // Standby (HIGH = Enable, LOW = Standby)
const uint8_t PIN_TB_PWMA = 5;   // Left Motor Speed PWM (0-255)
const uint8_t PIN_TB_PWMB = 6;   // Right Motor Speed PWM (0-255)
const uint8_t PIN_TB_AIN1 = 7;   // Left Motor Direction 1
const uint8_t PIN_TB_AIN2 = 8;   // Left Motor Direction 2
const uint8_t PIN_TB_BIN1 = 9;   // Right Motor Direction 1
const uint8_t PIN_TB_BIN2 = 10;  // Right Motor Direction 2

// ==========================================
// I2C 16x2 LCD DEFINITIONS (ATmega328P I2C)
// ==========================================
// Hardware I2C: SDA on A4, SCL on A5
#define LCD_I2C_ADDRESS 0x27  // Check 0x3F if 0x27 does not respond
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// ==========================================
// LINE SENSOR ARRAY PIN RESERVATIONS
// ==========================================
const uint8_t SENSOR_PINS[] = {A0, A1, A2, A3, A6, A7};
const uint8_t NUM_SENSORS   = sizeof(SENSOR_PINS) / sizeof(SENSOR_PINS[0]);

// ==========================================
// ROBOT MOTOR CALIBRATION & SAFETY DEFAULTS
// ==========================================
const int DEFAULT_BASE_SPEED = 150;
const int MAX_MOTOR_PWM      = 255;
const int MIN_MOTOR_PWM      = 0;

#endif // CONFIG_H
