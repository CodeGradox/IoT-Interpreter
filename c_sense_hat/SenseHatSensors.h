#ifndef SENSE_HAT_WRAPPER
#define SENSE_HAT_WRAPPER

#include "stdint.h"
#include "math.h"

typedef enum {
    TRUE = 1,
    FALSE = 0,
} Bool_t;

typedef struct Coordinates {
    float x;
    float y;
    float z;
} Coordinates;

typedef struct Orientation {
    float roll;
    float pitch;
    float yaw;
} Orientation;

/* Opaque type for the Wrapper (SenseHatSensors.cpp) */
struct SenseHatSensors;
typedef struct SenseHatSensors SenseHatSensors;

// Constructor
SenseHatSensors * SenseHatSensors_new(void);
// Destructor
void SenseHatSensors_delete(SenseHatSensors *sense);

void set_imu_config(SenseHatSensors *sense, Bool_t compass_enabled, Bool_t gyro_enabled, Bool_t accel_enabled);

// Envoiromental sensors
float get_humidity(SenseHatSensors *sense);
float get_pressure(SenseHatSensors *sense);
float get_temperature_from_humidity(SenseHatSensors *sense);
float get_temperature_from_pressure(SenseHatSensors *sense);
float get_temperature(SenseHatSensors *sense);

// IMU Sensor
Orientation get_orientation_radians(SenseHatSensors *sense);
Orientation get_orientation_degrees(SenseHatSensors *sense);
Orientation get_orientation(SenseHatSensors *sense);
float get_compass(SenseHatSensors *sense);
Coordinates get_compass_raw(SenseHatSensors *sense);
Orientation get_gyroscope(SenseHatSensors *sense);
Coordinates get_gyroscope_raw(SenseHatSensors *sense);
Orientation get_accelerometer(SenseHatSensors *sense);
Coordinates get_accelerometer_raw(SenseHatSensors *sense);

#endif /* SENSE_HAT_WRAPPER */

