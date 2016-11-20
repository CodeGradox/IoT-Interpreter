#ifndef SENSE_HAT_HPP
#define SENSE_HAT_HPP

#include "RTIMULib.h"

extern "C" {
    #include "SenseHatSensors.h"
}

#include "cstdlib"
/*#include "string"

#include "glob.h"
#include "stdio.h"
#include "sys/ioctl.h"
#include "linux/fb.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"*/

// Wrapper class for the RTIMU classes used by the Sense Hat API
class Wrapper {
public:
    Wrapper();      // Constructor
    ~Wrapper();     // Destructor
    float get_humidity(void);
    float get_pressure(void);
    float temperature_from_humidity(void);
    float temperature_from_pressure(void);
    float temperature(void);
    void set_imu_config(bool compass_enabled, bool gyro_enabled, bool accel_enabled);
    Orientation orientation_radians(void);
    Orientation orientation_degrees(void);
    Orientation orientation(void);
    float compass(void);
    Coordinates compass_raw(void);
    Orientation gyroscope(void);
    Coordinates gyroscope_raw(void);
    Orientation accelerometer(void);
    Coordinates accelerometer_raw(void);
private:
    bool read_imu(void);
    void init_imu(void);
    void init_humidity(void);
    void init_pressure(void);
    RTIMUSettings *settings;
    RTIMU *imu;
    bool imu_init;          // Will be initialised as and when needed
    RTPressure *pressure;
    bool pressure_init;     // Will be initialised as and when needed
    RTHumidity *humidity;
    bool humidity_init;     // Will be initialised as and when needed
    int imu_poll_interval;
    bool _compass_enabled;
    bool _gyro_enabled;
    bool _accel_enabled;
    Coordinates last_compass;
    Orientation last_orientation;
    Coordinates last_gyro;
    Coordinates last_accel;
    // uint16_t frame[8][8];
};

#endif /* SENSE_HAT_HPP*/
