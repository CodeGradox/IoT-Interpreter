#ifndef SENSE_HAT_WRAPPER
#define SENSE_HAT_WRAPPER

#include "stdint.h"

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
void SenseHatSensors_delete(SenseHatSensors *);

void set_imu_config(SenseHatSensors *, Bool_t, Bool_t, Bool_t);

// Envoiromental sensors
float get_humidity(SenseHatSensors *);
float get_pressure(SenseHatSensors *);
float get_temperature_from_humidity(SenseHatSensors *);
float get_temperature_from_pressure(SenseHatSensors *);
float get_temperature(SenseHatSensors *);

// IMU Sensor
Orientation get_orientation_radians(SenseHatSensors *);
Orientation get_orientation_degrees(SenseHatSensors *);
Orientation get_orientation(SenseHatSensors *);
float get_compass(SenseHatSensors *);
Coordinates get_compass_raw(SenseHatSensors *);
Orientation get_gyroscope(SenseHatSensors *);
Coordinates get_gyroscope_raw(SenseHatSensors *);
Orientation get_accelerometer(SenseHatSensors *);
Coordinates get_accelerometer_raw(SenseHatSensors *);

// LED
void set_pixel(SenseHatSensors *, uint16_t, uint8_t, uint8_t);
void set_pixels(SenseHatSensors *, uint16_t);
void set_image(SenseHatSensors *, uint16_t [64]);
void clear(SenseHatSensors *);

#endif /* SENSE_HAT_WRAPPER */

