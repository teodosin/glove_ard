
#include <MadgwickAHRS.h>
#include "LSM6DS3.h"
#include "Wire.h"

LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A

Madgwick filter;

unsigned long microsNow;
unsigned long microsPrevious;
const unsigned int microsPerReading = 10000; // 100 Hz update rate

float roll, pitch, heading;
float ax, ay, az;
float gx, gy, gz;

float currentX = 0.0;
float currentY = 0.0;
float currentZ = 0.0;

void imu_setup() {
  
  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("Device OK!");
  }
  
  filter.begin(100.0); // Initialize the Madgwick filter with a sample rate of 100 Hz
  microsPrevious = micros();
}

void imu_loop() {
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {
    // Read raw data from LSM6DS3 IMU
    ax = myIMU.readFloatAccelX();
    ay = myIMU.readFloatAccelY();
    az = myIMU.readFloatAccelZ();
    gx = myIMU.readFloatGyroX();
    gy = myIMU.readFloatGyroY();
    gz = myIMU.readFloatGyroZ();

    float imuData[] = {ax, ay, az, gx, gy, gz};
    send_imu(imuData);
    
    // Convert gyroscope readings from degrees per second to radians per second
    //gx *= 0.0174533;
    //gy *= 0.0174533;
    //gz *= 0.0174533;
    
    // Update the Madgwick filter
    filter.updateIMU(gx, gy, gz, ax, ay, az);
    
    // Get the roll, pitch, and heading (yaw)
    roll = filter.getRoll();
    pitch = filter.getPitch();
    heading = filter.getYaw();
    
    // Convert Euler angles from radians to degrees
    roll *= 180.0 / PI;
    pitch *= 180.0 / PI;
    heading *= 180.0 / PI;
    
    // Integrate the accelerometer readings to get the current position
    currentX += ax * (microsPerReading / 1000000.0);
    currentY += ay * (microsPerReading / 1000000.0);
    currentZ += az * (microsPerReading / 1000000.0);
    
    // Print the current position and orientation
    Serial.print("Position: ");
    Serial.print(currentX, 4);
    Serial.print(", ");
    Serial.print(currentY, 4);
    Serial.print(", ");
    Serial.print(currentZ, 4);
    Serial.print(" | Orientation: ");
    Serial.print(heading, 2);
    Serial.print(", ");
    Serial.print(pitch, 2);
    Serial.print(", ");
    Serial.println(roll, 2);
    
    // Increment previous time
    microsPrevious = microsPrevious + microsPerReading;
  }
}