#ifndef RTIMULIB_WRAPPER_H
#define RTIMULIB_WRAPPER_H

#include "stdint.h"

enum boolean {
    TRUE = 1,
    FALSE = 0,
};

struct C_RTIMUSettings;
typedef struct C_RTIMUSettings C_RTIMUSettings;

struct C_RTIMU;
typedef struct C_RTIMU C_RTIMU;

struct C_RTPressure;
typedef struct C_RTPressure C_RTPressure;

struct C_RTHumidity;
typedef struct C_RTHumidity C_RTHumidity;

struct C_RTIMU_DATA;
typedef struct C_RTIMU_DATA C_RTIMU_DATA;

// Settings
// constructor and destructor for RTIMUSettings object
C_RTIMUSettings * C_RTIMUSettings_new(const char *productType);
int C_RTIMUSettings_destroy(C_RTIMUSettings *s);

// IMU
// Constructor and destructor the RTIMU object
C_RTIMU * C_RTIMU_new(C_RTIMUSettings *s);
int C_RTIMU_destroy(C_RTIMU *imu);
// Set up the IMU
int imu_init(C_RTIMU *imu);
// Change config
int set_imu_config(C_RTIMU *imu, float slerpPower, int compass, int gyro, int accel);
int imu_read(C_RTIMU *imu);

// IMU_DATA
C_RTIMU_DATA * get_imu_data(C_RTIMU *imu);
int destroy_imu_data(C_RTIMU_DATA *data);

// Pressure
// Constructor and destructor
C_RTPressure * C_create_pressure(C_RTIMUSettings *s);
int C_RTPressure_destroy(C_RTPressure *p);
// Set up the pressure sensor
int pressure_init(C_RTPressure *p);

// Humidity
C_RTHumidity * C_create_humidity(C_RTIMUSettings *s);
int C_RTHumidity_destroy(C_RTHumidity *p);
// Set up the humidity sensor
int humidity_init(C_RTHumidity *h);

#endif

