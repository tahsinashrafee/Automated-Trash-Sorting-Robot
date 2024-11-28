#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// IR Sensor Pin
const int irPin = 7;

// Metal Detector Sensor Pin
const int sensorPin = 2;

// Moisture Sensor Pin
const int potPin = A0;

// Servo Motors
Servo lowerServo;  // Servo on pin 4
Servo upperServo;  // Servo on pin 8

// LCD Display
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

// Variables for Sensors
int objectDetected;
int sensorValue;
int fsoil;

void setup() {
  // Start Serial Communication
  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();                      
  lcd.backlight();

  // Display a welcome message
  lcd.setCursor(0, 0);
  lcd.print("CSE461 Group 8");

  // Pin Modes
  pinMode(irPin, INPUT);
  pinMode(sensorPin, INPUT);

  // Attach Servos
  lowerServo.attach(4);
  upperServo.attach(8);

  // Initial position of servos
  lowerServo.write(0); // Lower servo starts at 0 degrees, closed position
  upperServo.write(0); // Upper servo starts at 0 degrees, closed position
}

void loop() {
  // IR Sensor Check
  objectDetected = digitalRead(irPin);

  // Check and print the object detection status continuously
  if (objectDetected == LOW) {
    Serial.println("Object Detected!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Object Detected!");

    sensorValue = digitalRead(sensorPin);
    if (sensorValue == HIGH) { // Metal Detected
      Serial.println("Metal");
      lcd.setCursor(0, 1);
      lcd.print("Metal");
      lowerServo.write(180); // Rotate lowerServo to 180 degrees for metal detection
    } else {
      fsoil = 0; // Reset soil moisture average before calculation
      for (int i = 0; i < 3; i++) {
        int soil = analogRead(potPin);
        soil = constrain(soil, 485, 1023);
        fsoil += map(soil, 485, 1023, 100, 0);
        delay(75);
      }
      fsoil /= 3;

      if (fsoil > 3) {
        Serial.println("Wet");
        lcd.setCursor(0, 1);
        lcd.print("Wet");
        lowerServo.write(90); // Rotate lowerServo to 180 degrees for wet soil
      } else {
        Serial.println("Dry");
        lcd.setCursor(0, 1);
        lcd.print("Dry");
        lowerServo.write(0); // Rotate lowerServo to 0 degrees for dry soil
      }
    }

    // Delay to allow the lower servo to move into position
    delay(1000);

    // Open the upper compartment to drop the element
    upperServo.write(180);
    delay(1000);

    // Reset positions
    upperServo.write(0);
    lowerServo.write(0);
    delay(1000);
  }

  delay(1000); // Main loop delay to manage cycle time
}
