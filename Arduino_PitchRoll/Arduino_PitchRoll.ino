#include <I2Cdev.h>
#include <MPU6050.h>

/*
 * Created by Stanley Wang
 * 10/28/15
 *
 * Team Dronimator
 *
 * Connections
 * SCL -> A5
 * SDA -> A4
 * VCC -> 5V
 * GND, AD0 -> GND
 * INT -> Any digital pin
 * XDA, XCL -> Unconnected
 *
 */

#include<Wire.h>

// I2C address of the MPU-6050 
// AD0 -> GND for 0x68
// AD0 -> 5V for 0x69
const int MPU = 0x68;

// We do not want our quadcopter going past this pitch/roll angle
const int MAX_ANGLE = 20;

// Max and min pulsewidth we can give the esc
// Manually determine the hover pulse by test and trial
const int MAX_PULSE = 2000;
const int MIN_PULSE = 1000;
const int HOVER_PULSE = 1500;

// Stores acccelerometer Data from MPU 6050
long accX, accY, accZ;

//make this a constant later?
long accXOffset, accYOffset, accZOffset;

void setup() {
  setupMPU();
  Serial.begin(9600);
}
void setupMPU() {
  Wire.begin();
  // We are reading from 0x68 and not 0x69
  Wire.beginTransmission(MPU);
  // Start address for pwr_mgmt_1 register
  Wire.write(0x6B);
  // Turns on MPU-6050
  Wire.write(0);
  Wire.endTransmission(true);

  //set accelerometer offset
  accXOffset = 0;
  accYOffset = 0;
  accZOffset = 0;
}
void loop() {
  getMPUData();

  //pitch and roll are basically the same because its equal on all sides
  float roll = atan2(accX, sqrt(accY * accY + accZ * accZ)) * 180 / PI; //degrees in +- 90
  float pitch = atan2(accY, sqrt(accX * accX + accZ * accZ)) * 180 / PI; //degrees in +- 90

  Serial.print("Pitch = "); Serial.print(pitch);
  Serial.print(" | Roll = "); Serial.println(roll);

  control1(roll, pitch);
  control2(roll, pitch);

  delay(500);
}

/*
 * Tested and works
 *           MotorN
 * MotorW    Center    MotorE
 *           MotorS
 */
void control1(float roll, float pitch) {
  // determine fast each motor rotates
  float motorN = -pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
  float motorE = -roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
  float motorS = pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
  float motorW = roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;

  if (motorN < MIN_PULSE) {
    motorN = MIN_PULSE;
  } else if (motorN > MAX_PULSE) {
    motorN = MAX_PULSE;
  }
  
  if (motorE < MIN_PULSE) {
    motorE = MIN_PULSE;
  } else if (motorE > MAX_PULSE) {
    motorE = MAX_PULSE;
  }
  
  if (motorS < MIN_PULSE) {
    motorS = MIN_PULSE;
  } else if (motorS > MAX_PULSE) {
    motorS = MAX_PULSE;
  }
  
  if (motorW < MIN_PULSE) {
    motorW = MIN_PULSE;
  } else if (motorW > MAX_PULSE) {
    motorW = MAX_PULSE;
  }
  
  Serial.println();
  Serial.print("\t"); Serial.println(motorN);
  Serial.print(motorW); Serial.print("\t\t\t"); Serial.println(motorE);
  Serial.print("\t"); Serial.println(motorS);
  Serial.println();
}

/*
 * Tested and works
 * MotorNW            MotorNE
 *          Center
 * MotorSW            MotorSE
 */
void control2(float roll, float pitch) {
  // used to determine ratio for motors
  float total = abs(roll) + abs(pitch);

  // determine fast each motor rotates
  float motorNW = (abs(pitch) / total) * -pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) +
                  (abs(roll) / total) * roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
  float motorNE = (abs(pitch) / total) * -pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) +
                  (abs(roll) / total) * -roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
   float motorSE = (abs(pitch) / total) * pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) +
                  (abs(roll) / total) * -roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
  float motorSW = (abs(pitch) / total) * pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) +
                  (abs(roll) / total) * roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
 if (motorNW < MIN_PULSE) {
    motorNW = MIN_PULSE;
  } else if (motorNW > MAX_PULSE) {
    motorNW = MAX_PULSE;
  }
  
  if (motorNE < MIN_PULSE) {
    motorNE = MIN_PULSE;
  } else if (motorNE > MAX_PULSE) {
    motorNE = MAX_PULSE;
  }
  
  if (motorSE < MIN_PULSE) {
    motorSE = MIN_PULSE;
  } else if (motorSE > MAX_PULSE) {
    motorSE = MAX_PULSE;
  }
  
  if (motorSW < MIN_PULSE) {
    motorSW = MIN_PULSE;
  } else if (motorSW > MAX_PULSE) {
    motorSW = MAX_PULSE;
  }

  Serial.println();
  Serial.print(motorNW); Serial.print("\t\t\t"); Serial.println(motorNE);
  Serial.println();
  Serial.print(motorSW); Serial.print("\t\t\t"); Serial.println(motorSE);
  Serial.println();
}

void getMPUData() {
  Wire.beginTransmission(MPU);
  //0x3B is start of accelerometer data
  Wire.write(0x3B);
  Wire.endTransmission(false);
  //0x3B to 0x40 are accelerometer data from low and high clocks
  Wire.requestFrom(MPU, 6, true);
  accX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  accY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  accZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  //Serial.print("accX = "); Serial.print(accX);
  //Serial.print("accY = "); Serial.print(accY);
  //Serial.print("accZ = "); Serial.println(accZ);
}

