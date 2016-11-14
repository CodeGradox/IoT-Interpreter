#ifndef RTIMULIB_WRAPPER_H
#define RTIMULIB_WRAPPER_H

#include "stdint.h"

enum boolean {
    TRUE = 1,
    FALSE = 0,
};

struct SenseHatSensors;
typedef struct SenseHatSensors SenseHatSensors;

// Constructor
SenseHatSensors * SenseHatSensors_new(void);
// Destructor
void SenseHatSensors_delete(SenseHatSensors *sense);

// Sensors
float get_humidity(SenseHatSensors *sense);
float get_pressure(SenseHatSensors *sense);
float get_temperature_from_humidity(SenseHatSensors *sense);
float get_temperature_from_pressure(SenseHatSensors *sense);
float get_temperature(SenseHatSensors *sense);

#endif

