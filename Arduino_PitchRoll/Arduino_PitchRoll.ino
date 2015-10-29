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

// I2C address of the MPU-6050 for AD0 -> GND
const int MPU = 0x68;

// We do not want our quadcopter going past this pitch/roll angle
const int MAX_ANGLE = 45;

// Max and min pulsewidth we can give the esc
// Change min later to pulse width to hover
const int MAX_PULSE = 2000;
const int MIN_PULSE = 1000;

// Stores acccelerometer Data from MPU 6050
long accX, accY, accZ;

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
  float motorN = -pitch / MAX_ANGLE * (MAX_PULSE - MIN_PULSE) + MIN_PULSE;
  float motorE = -roll / MAX_ANGLE * (MAX_PULSE - MIN_PULSE) + MIN_PULSE;
  float motorS = pitch / MAX_ANGLE * (MAX_PULSE - MIN_PULSE) + MIN_PULSE;
  float motorW = roll / MAX_ANGLE * (MAX_PULSE - MIN_PULSE) + MIN_PULSE;

  Serial.println();
  Serial.print("\t"); Serial.println(motorN);
  Serial.print(motorW); Serial.print("\t\t\t"); Serial.println(motorE);
  Serial.print("\t"); Serial.println(motorS);
  Serial.println();
}

/*
 * Untested
 * MotorNW            MotorNE
 *          Center
 * MotorSW            MotorSE
 */
void control2(float roll, float pitch) {
  // used to determine ratio for motors
  float total = abs(roll) + abs(pitch);

  // determine fast each motor rotates
  float motorNW = (abs(pitch) / total) * -pitch / MAX_ANGLE * (MAX_PULSE - MIN_PULSE) 
                  (abs(roll) / total) * roll / MAX_ANGLE * (MAX_PULSE - MIN_PULSE) + MIN_PULSE;
  float motorNE = (abs(pitch) / total) * -pitch / MAX_ANGLE * (MAX_PULSE - MIN_PULSE)
                  (abs(roll) / total) * -roll / MAX_ANGLE * (MAX_PULSE - MIN_PULSE) + MIN_PULSE;
  float motorSW = (abs(pitch) / total) * pitch / MAX_ANGLE * (MAX_PULSE - MIN_PULSE)
                  (abs(roll) / total) * roll / MAX_ANGLE * (MAX_PULSE - MIN_PULSE) + MIN_PULSE;
  float motorSE = (abs(pitch) / total) * pitch / MAX_ANGLE * (MAX_PULSE - MIN_PULSE)
                  (abs(roll) / total) * -roll / MAX_ANGLE * (MAX_PULSE - MIN_PULSE) + MIN_PULSE;

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

