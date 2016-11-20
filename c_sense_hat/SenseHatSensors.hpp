#ifndef SENSE_HAT_HPP
#define SENSE_HAT_HPP

#include "RTIMULib.h"

extern "C" {
    #include "SenseHatSensors.h"
}

#include "cstdlib"
#include "string"

#include "glob.h"
#include "stdio.h"
#include "sys/ioctl.h"
#include "linux/fb.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "sys/mman.h"

const char * RPI_SENSE_FB = "RPi-Sense FB";

typedef struct framebuffer {
    uint16_t frame[8][8];
} framebuffer;

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
    void set_imu_config(bool, bool, bool);
    Orientation orientation_radians(void);
    Orientation orientation_degrees(void);
    Orientation orientation(void);
    float compass(void);
    Coordinates compass_raw(void);
    Orientation gyroscope(void);
    Coordinates gyroscope_raw(void);
    Orientation accelerometer(void);
    Coordinates accelerometer_raw(void);

    void set_pixel(uint16_t, uint8_t, uint8_t);
    void set_pixels(void);
    void set_pixels(uint16_t);
    void set_image(uint16_t [64]);
    void clear(void);
private:
    void open_framebuffer(void);
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
    // The framebuffer is automaticaly closed when the program terminates
    // so there is no need to free it.
    framebuffer *fb;
};

#endif /* SENSE_HAT_HPP*/
