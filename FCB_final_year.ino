#include <Servo.h>

// Motor Pins
int leftWheelMotorPositivePin = 5;
int leftWheelMotorNegativePin = 6;
int rightWheelMotorPositivePin = 9;
int rightWheelMotorNegativePin = 10;

// IR Sensor Pins
int leftSensorPin = A0;
int middleSensorPin = A1;
int rightSensorPin = A2;

// Other Components
int waterPumpPin = 13;
int fanMotorPin = 8;

Servo myServo;
bool fireDetected = false;

void setup() {
  Serial.begin(9600);
  
  // Motor Setup
  pinMode(leftWheelMotorPositivePin, OUTPUT);
  pinMode(leftWheelMotorNegativePin, OUTPUT);
  pinMode(rightWheelMotorPositivePin, OUTPUT);
  pinMode(rightWheelMotorNegativePin, OUTPUT);

  // Sensor Setup
  pinMode(leftSensorPin, INPUT);
  pinMode(middleSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);
  
  // Other Components Setup
  pinMode(waterPumpPin, OUTPUT);
  pinMode(fanMotorPin, OUTPUT);

  // Servo Setup
  myServo.attach(7);
  myServo.write(90); // Start with servo facing forward

  // Ensure motors and components are off initially
  stopAll();
}

void stopAll() {
  digitalWrite(leftWheelMotorPositivePin, LOW);
  digitalWrite(leftWheelMotorNegativePin, LOW);
  digitalWrite(rightWheelMotorPositivePin, LOW);
  digitalWrite(rightWheelMotorNegativePin, LOW);
  digitalWrite(waterPumpPin, HIGH); // Water pump OFF
  digitalWrite(fanMotorPin, HIGH);  // Fan OFF
}

void moveRobot(String motion) {
  if (motion == "Forward") {
    digitalWrite(leftWheelMotorPositivePin, HIGH);
    digitalWrite(rightWheelMotorPositivePin, HIGH);
    digitalWrite(leftWheelMotorNegativePin, LOW);
    digitalWrite(rightWheelMotorNegativePin, LOW);
    Serial.println("Moving forward");
  } 
  else if (motion == "Stop") {
    stopAll();
    Serial.println("STOP");
  } 
  else if (motion == "Left") {
    digitalWrite(leftWheelMotorPositivePin, HIGH);
    digitalWrite(leftWheelMotorNegativePin, LOW);
    digitalWrite(rightWheelMotorPositivePin, LOW);
    digitalWrite(rightWheelMotorNegativePin, HIGH);
    Serial.println("Turning left");
    delay(250);
    stopAll();
  } 
  else if (motion == "Right") {
    digitalWrite(leftWheelMotorPositivePin, LOW);
    digitalWrite(leftWheelMotorNegativePin, HIGH);
    digitalWrite(rightWheelMotorPositivePin, HIGH);
    digitalWrite(rightWheelMotorNegativePin, LOW);
    Serial.println("Turning right");
    delay(250);
    stopAll();
  }
  delay(100);
}

void ExtinguishFire() {
  Serial.println("Extinguishing fire...");

  digitalWrite(waterPumpPin, LOW); // Water pump ON

  while (fireDetected) {
    for (int angle = 0; angle <= 180; angle += 3) {
      myServo.write(angle);
      delay(20);
    }
    for (int angle = 180; angle >= 0; angle -= 3) {
      myServo.write(angle);
      delay(20);
    }

    int leftSensorState = digitalRead(leftSensorPin);
    int middleSensorState = digitalRead(middleSensorPin);
    int rightSensorState = digitalRead(rightSensorPin);

    fireDetected = (leftSensorState == LOW || middleSensorState == LOW || rightSensorState == LOW);
  }

  digitalWrite(waterPumpPin, HIGH); // Water pump OFF

  myServo.write(90);
  delay(500);

  digitalWrite(fanMotorPin, LOW); // Fan ON
  Serial.println("Cooling down...");
  delay(5000);
  digitalWrite(fanMotorPin, HIGH); // Fan OFF
  Serial.println("Cooldown complete");
}

void loop() {
  int leftSensorState = digitalRead(leftSensorPin);
  int middleSensorState = digitalRead(middleSensorPin);
  int rightSensorState = digitalRead(rightSensorPin);

  int fireCount = 0;
  if (leftSensorState == LOW) fireCount++;
  if (middleSensorState == LOW) fireCount++;
  if (rightSensorState == LOW) fireCount++;

  fireDetected = (fireCount > 0);

  if (fireCount >= 2) {
    Serial.println("Multiple sensors detected fire! Stopping and extinguishing.");
    moveRobot("Stop");
    delay(300);
    ExtinguishFire();
  } 
  else if (fireCount == 1) {
    if (leftSensorState == LOW) {
      Serial.println("Fire detected on LEFT. Turning left...");
      moveRobot("Left");
    } 
    else if (rightSensorState == LOW) {
      Serial.println("Fire detected on RIGHT. Turning right...");
      moveRobot("Right");
    } 
    else if (middleSensorState == LOW) {
      Serial.println("Fire detected in CENTER. Moving forward...");
      moveRobot("Forward");
    }
  } 
  else {
    Serial.println("No fire detected. Stopping.");
    moveRobot("Stop");
    myServo.write(90);
  }

  delay(100);
}
