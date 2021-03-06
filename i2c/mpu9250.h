#ifndef MPU9250
#define MPU9250

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdlib.h>
#include <i2cMaster.h>

#define ACCEL_XOUT_H  0x3B
#define ACCEL_XOUT_L  0x3C
#define ACCEL_YOUT_H  0x3D
#define ACCEL_YOUT_L  0x3E
#define ACCEL_ZOUT_H  0x3F
#define ACCEL_ZOUT_L  0x40
#define TEMP_OUT_H    0x41
#define TEMP_OUT_L    0x42
#define GYRO_XOUT_H   0x43
#define GYRO_XOUT_L   0x44
#define GYRO_YOUT_H   0x45
#define GYRO_YOUT_L   0x46
#define GYRO_ZOUT_H   0x47
#define GYRO_ZOUT_L   0x48

#define HXL 0x03
#define HXH 0x04
#define HYL 0x05
#define HYH 0x06
#define HZL 0x07
#define HZH 0x08

int getAccelData(void);
int getGyroData(void);
int getTempData(void);
int getMagData(void);

int getAccelData(void)
{
  int accelData[3];

  accelData[0] = ((SingleMRST(SLA1, ACCEL_XOUT_H) << 8) | SingleMRST(SLA1, ACCEL_XOUT_L));
  accelData[1] = ((SingleMRST(SLA1, ACCEL_YOUT_H) << 8) | SingleMRST(SLA1, ACCEL_YOUT_L));
  accelData[2] = ((SingleMRST(SLA1, ACCEL_ZOUT_H) << 8) | SingleMRST(SLA1, ACCEL_ZOUT_L));

  return(accelData);
}

int getGyroData(void)
{
  int gyroData[3];

  gyroData[0] = ((SingleMRST(SLA1, GYRO_XOUT_H) << 8) | SingleMRST(SLA1, GYRO_XOUT_L));
  gyroData[1] = ((SingleMRST(SLA1, GYRO_YOUT_H) << 8) | SingleMRST(SLA1, GYRO_YOUT_L));
  gyroData[2] = ((SingleMRST(SLA1, GYRO_ZOUT_H) << 8) | SingleMRST(SLA1, GYRO_ZOUT_L));

  return(gyroData);
}

int getTempData(void)
{
  int tempData;

  tempData = ((SingleMRST(SLA1, TEMP_OUT_H) << 8) | SingleMRST(SLA1, TEMP_OUT_L));

  return(tempData);
}

int getMagData(void)
{
  int magData[3];

  magData[0] = ((SingleMRST(SLA1, HXH) << 8) | SingleMRST(SLA1, HXL));
  magData[1] = ((SingleMRST(SLA1, HYH) << 8) | SingleMRST(SLA1, HYL));
  magData[2] = ((SingleMRST(SLA1, HZH) << 8) | SingleMRST(SLA1, HZL));

  return(magData);
}

#endif
