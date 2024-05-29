// servoesp32fix library
// ps4controller library

#include <PS4Controller.h>
#include <Servo.h>
#include <vector>

#define sinyal_sag 14
#define sinyal_sol 12
#define stoplar 13
#define kirmizi 16
#define yesil 21
#define SERVO_FORWARD_STEP_ANGLE 3
#define SERVO_BACKWARD_STEP_ANGLE -1


int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 300;

int ledState_1 = LOW;
unsigned long previousMillis_1 = 0;
const long interval_1 = 300;

int ledState_2 = LOW;
unsigned long previousMillis_2 = 0;
const long interval_2 = 200;

int ledState_3 = LOW;
unsigned long previousMillis_3 = 0;
const long interval_3 = 300;

struct ServoPins {
  Servo servo;
  int servoPin;
  String servoName;
  int initialPosition;
};
std::vector<ServoPins> servoPins = {
  { Servo(), 33, "Base", 85 },
  { Servo(), 25, "Shoulder", 90 },
  { Servo(), 26, "Elbow", 90 },
  { Servo(), 27, "Gripper", 90 },
};

bool gripperSwitch = false;

//Right motor
int enableRightMotor = 22;
int rightMotorPin1 = 19;
int rightMotorPin2 = 18;
//Left motor
int enableLeftMotor = 23;
int leftMotorPin1 = 5;
int leftMotorPin2 = 17;

#define MAX_MOTOR_SPEED 225  //Its value can range from 0-255. 255 is maximum speed.

const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;
const int PWMSpeedChannel = 4;

void writeServoValues(int servoIndex, int servoMoveStepSize, bool servoStepSizeIsActualServoPosition = false) {
  int servoPosition;
  if (servoStepSizeIsActualServoPosition) {
    servoPosition = servoMoveStepSize;
  } else {
    servoPosition = servoPins[servoIndex].servo.read();
    servoPosition = servoPosition + servoMoveStepSize;
  }
  if (servoPosition > 180 || servoPosition < 0) {
    return;
  }

  servoPins[servoIndex].servo.write(servoPosition);
}

void onConnect() {
  digitalWrite(kirmizi, LOW);
  digitalWrite(yesil, HIGH);
}

void onDisConnect() {
  digitalWrite(yesil, LOW);
  digitalWrite(kirmizi, HIGH);
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
  }

  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
  }
}

void setUpPinModes() {
  for (int i = 0; i < servoPins.size(); i++) {
    servoPins[i].servo.attach(servoPins[i].servoPin);
    servoPins[i].servo.write(servoPins[i].initialPosition);
  }

  pinMode(yesil, OUTPUT);
  pinMode(kirmizi, OUTPUT);

  pinMode(sinyal_sag, OUTPUT);
  pinMode(sinyal_sol, OUTPUT);
  pinMode(stoplar, OUTPUT);

  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  //Set up PWM for motor speed
  ledcSetup(PWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(enableRightMotor, PWMSpeedChannel);
  ledcAttachPin(enableLeftMotor, PWMSpeedChannel);
  ledcWrite(PWMSpeedChannel, MAX_MOTOR_SPEED);

  rotateMotor(0, 0);
}


void setup() {
  setUpPinModes();
  digitalWrite(kirmizi, HIGH);
  Serial.begin(9600);
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisConnect);
  PS4.begin();
}

void loop() {
  int rx = (PS4.RStickX());  //Base       =>  Right stick - x axis
  int ry = (PS4.RStickY());  //Shoulder   =>  Right stick  - y axis
  int ly = (PS4.LStickX());  //Elbow      =>  Left stick  - y axis
  int lx = (PS4.LStickY());  //Gripper    =>  Left stick - x axis

  unsigned long currentMillis_3 = millis();
  if (PS4.Triangle()) {
    if (currentMillis_3 - previousMillis_3 >= interval_3) {
      previousMillis_3 = currentMillis_3;
      if (ledState_3 == LOW) {
        ledState_3 = HIGH;
      } else {
        ledState_3 = LOW;
      }
      digitalWrite(sinyal_sol, ledState_3);
      digitalWrite(sinyal_sag, ledState_3);
    }
  } else {
    unsigned long currentMillis = millis();
    if (PS4.R1()) {
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        digitalWrite(sinyal_sag, ledState);
      }
    } else {
      digitalWrite(sinyal_sag, LOW);
    }

    unsigned long currentMillis_1 = millis();
    if (PS4.L1()) {
      if (currentMillis_1 - previousMillis_1 >= interval_1) {
        previousMillis_1 = currentMillis_1;
        if (ledState_1 == LOW) {
          ledState_1 = HIGH;
        } else {
          ledState_1 = LOW;
        }
        digitalWrite(sinyal_sol, ledState_1);
      }
    } else {
      digitalWrite(sinyal_sol, LOW);
    }
  }

  if (rx > 80) {
    writeServoValues(0, SERVO_BACKWARD_STEP_ANGLE);
  } else if (rx < -80) {
    writeServoValues(0, SERVO_FORWARD_STEP_ANGLE);
  }

  if (ry > 80) {
    writeServoValues(1, SERVO_FORWARD_STEP_ANGLE);
  } else if (ry < -80) {
    writeServoValues(1, SERVO_BACKWARD_STEP_ANGLE);
  }

  if (lx > 80) {
    writeServoValues(2, SERVO_BACKWARD_STEP_ANGLE);
  } else if (lx < -80 && servoPins[2].servo.read() <= 130) {
    writeServoValues(2, SERVO_FORWARD_STEP_ANGLE);
  }

  if (ly > 80 && servoPins[3].servo.read() >= 40) {
    writeServoValues(3, SERVO_BACKWARD_STEP_ANGLE);
  } else if (ly < -80) {
    writeServoValues(3, SERVO_FORWARD_STEP_ANGLE);
  }

  if (PS4.Up()) {
    rotateMotor(MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);
    digitalWrite(stoplar, LOW);

  } else if (PS4.Down()) {
    rotateMotor(-MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
    unsigned long currentMillis_2 = millis();
    if (currentMillis_2 - previousMillis_2 >= interval_2) {
      previousMillis_2 = currentMillis_2;
      if (ledState_2 == LOW) {
        ledState_2 = HIGH;
      } else {
        ledState_2 = LOW;
      }
      digitalWrite(stoplar, ledState_2);
    }
  } else if (PS4.Right()) {
    rotateMotor(MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
    digitalWrite(stoplar, LOW);
  } else if (PS4.Left()) {
    rotateMotor(-MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);
    digitalWrite(stoplar, LOW);
  } else {
    rotateMotor(0, 0);
    digitalWrite(stoplar, HIGH);
  }
}