/** 
 *  
 *  Author Davi DeBarros 
 *  University of Rhode Island Capstone Design Project 
**/
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <MPU6050_6Axis_MotionApps20.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <Servo.h>

#define DEBUG
//#define GYRO
#define MOTOR

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//MPU Variable
///////////////////////////////////////////////////////////////////////////////////////////////////////////
MPU6050 mpu;                           // mpu interface object
uint8_t mpuIntStatus;                  // mpu statusbyte
uint8_t devStatus;                     // device status    
uint16_t packetSize;                   // estimated packet size  
uint16_t fifoCount;                    // fifo buffer size   
uint8_t fifoBuffer[64];                // fifo buffer 

Quaternion q;                          // quaternion for mpu output
VectorFloat gravity;                   // gravity vector for ypr
float ypr[3] = {0.0f,0.0f,0.0f};       // yaw pitch roll values
float yprLast[3] = {0.0f, 0.0f, 0.0f};

volatile bool mpuInterrupt = false;    //interrupt flag



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Motor Variables 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_ANGLE 30                // We do not want our quadcopter going past this pitch/roll angle
#define MAX_PULSE 2000             // Max and min pulsewidth we can give the esc
#define MIN_PULSE 1000             
#define HOVER_PULSE 1500           // Manually determine the hover pulse by test and trial

#define m1_pin 8
#define m2_pin 9
#define m3_pin 10
#define m4_pin 11

Servo m1,m2,m3,m4 ;



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Setup
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    initiateESC();
    initiateMPU();
    
    #ifdef DEBUG
    Serial.begin(9600);                 // Serial only necessary if in DEBUG mode
    Serial.flush();
    #endif
  
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Main Loop 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  while(!mpuInterrupt && fifoCount < packetSize){
     
    /* Do nothing while MPU is not working
     * This should be a VERY short period
     */
      
  }
  getYPR();
  updateMotor(ypr[2]*180/PI, ypr[1]*180/PI);
                            



}




inline void dmpDataReady() {
    mpuInterrupt = true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Initiates the MPU chip
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void initiateMPU(){
  Wire.begin();
  mpu.initialize();
  devStatus = mpu.dmpInitialize();
  if(devStatus == 0){
  
    mpu.setDMPEnabled(true);
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    packetSize = mpu.dmpGetFIFOPacketSize();
    
  }
}


void initiateESC(){
  
  m1.attach(m1_pin);//,MIN_PULSE, MAX_PULSE);
  m2.attach(m2_pin);//,MIN_PULSE, MAX_PULSE);
  m3.attach(m3_pin);//,MIN_PULSE, MAX_PULSE);
  m4.attach(m4_pin);//,MIN_PULSE, MAX_PULSE);

  m1.writeMicroseconds(MIN_PULSE);
  m2.writeMicroseconds(MIN_PULSE);
  m3.writeMicroseconds(MIN_PULSE);
  m4.writeMicroseconds(MIN_PULSE);

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//getYPR function
//gets data from MPU and computes pitch, roll, yaw on the MPU's DMP
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void getYPR(){
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    fifoCount = mpu.getFIFOCount();
    
    if((mpuIntStatus & 0x10) || fifoCount >= 1024){ 
      
      mpu.resetFIFO(); 
    
    }else if(mpuIntStatus & 0x02){
    
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      
      fifoCount -= packetSize;
    
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    }

    #ifdef DEBUG && GYRO
      Serial.print(" ========== ") ;
      Serial.print(ypr[0]*180/PI);
      Serial.print(" ========== ") ;
      Serial.print(ypr[1]*180/PI);
      Serial.print(" ========== ") ;
      Serial.println(ypr[2]*180/PI);
    #endif
}
/*
 * Tested and works
 * m1_v            m2_v
 *          Center
 * m4_v            m3_v
 */
void updateMotor(float roll, float pitch) {
  // used to determine ratio for motors
  float total = abs(roll) + abs(pitch);

  // determine fast each motor rotates
  float m1_v = (abs(roll) / total) * -roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) +
                  (abs(pitch) / total) * -pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
  float m2_v = (abs(roll) / total) * roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) +
                  (abs(pitch) / total) * -pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
  float m4_v = (abs(roll) / total) * -roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) +
                  (abs(pitch) / total) * pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
  float m3_v = (abs(roll) / total) * roll / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) +
                  (abs(pitch) / total) * pitch / MAX_ANGLE * (MAX_PULSE - HOVER_PULSE) + HOVER_PULSE;
 
//  if (m1_v < MIN_PULSE) {
//    m1_v = MIN_PULSE;
//  } else if (m1_v > MAX_PULSE) {
//    m1_v = MAX_PULSE;
//  }
//  
//  if (m2_v < MIN_PULSE) {
//    m2_v = MIN_PULSE;
//  } else if (m2_v > MAX_PULSE) {
//    m2_v = MAX_PULSE;
//  }
//  
//  if (m3_v < MIN_PULSE) {
//    m3_v = MIN_PULSE;
//  } else if (m3_v > MAX_PULSE) {
//    m3_v = MAX_PULSE;
//  }
//  
//  if (m4_v < MIN_PULSE) {
//    m4_v = MIN_PULSE;
//  } else if (m4_v > MAX_PULSE) {
//    m4_v = MAX_PULSE;
//  }

  m1.writeMicroseconds(m1_v);
  m2.writeMicroseconds(m2_v);
  m3.writeMicroseconds(m3_v);
  m4.writeMicroseconds(m4_v);

 
  #ifdef DEBUG && MOTOR
    Serial.println();
    Serial.print(m1_v); Serial.print("\t\t\t"); Serial.println(m2_v);
    Serial.println();
    Serial.print(m4_v); Serial.print("\t\t\t"); Serial.println(m3_v);
    Serial.println();
  #endif
}

