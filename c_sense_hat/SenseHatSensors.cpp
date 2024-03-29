#include "SenseHatSensors.hpp"

/* Constructor*/
Wrapper::Wrapper() {
    open_framebuffer();

    // Get config file used by the Python SenseHat library
    char ini_name[80];
    snprintf(ini_name, sizeof(ini_name), "%s/.config/sense_hat/RTIMULib", std::getenv("HOME"));

    settings = new RTIMUSettings(ini_name);
    imu = RTIMU::createIMU(settings);
    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        throw "No IMU found";
    }
    imu_init = false;
    pressure = RTPressure::createPressure(settings);
    pressure_init = false;
    humidity = RTHumidity::createHumidity(settings);
    humidity_init = false;
    imu_poll_interval = 0;
    _compass_enabled = false;
    _gyro_enabled = false;
    _accel_enabled = false;
    memset(&last_compass, 0, sizeof(Coordinates));
    last_gyro = last_accel = last_compass;
    memset(&last_orientation, 0, sizeof(Orientation));
}

/* Destructor */
Wrapper::~Wrapper() {
    printf("Deleting Wrapper class!\n");
    delete settings;
    delete imu;
    delete pressure;
    delete humidity;
}

/***** Sensors *****/

/* Initialises the humidity sensor via RTIMU */
void Wrapper::init_humidity(void) {
    if (!humidity_init) {
        humidity_init = humidity->humidityInit();
        if (!humidity_init) {
            throw "Could not initialise humidity";
        }
    }
}

/* Initialises the pressure sensor via RTIMU */
void Wrapper::init_pressure(void) {
    if (!pressure_init) {
        pressure_init = pressure->pressureInit();
        if (!pressure_init) {
            throw "Could not initialise pressure";
        }
    }
}

/* Returns precentage of relative humidity */
float Wrapper::get_humidity(void) {
    init_humidity(); // Ensure humidity sensor is initialised
    float _humidity = 0.0;
    RTIMU_DATA data;
    humidity->humidityRead(data);
    if (data.humidityValid) {
        _humidity = data.humidity;
    }
    return _humidity;
}

/* Returns the pressure in Millibars */
float Wrapper::get_pressure(void) {
    init_pressure(); // Ensure pressure sensor is initialised
    float _pressure = 0.0;
    RTIMU_DATA data;
    pressure->pressureRead(data);
    if (data.pressureValid) {
        _pressure = data.pressure;
    }
    return _pressure;
}

/* Returns the temperature in Celsius from the pressure sensor */
float Wrapper::temperature_from_humidity(void) {
    init_humidity(); // Ensure humidity is initialised
    float temp = 0.0;
    RTIMU_DATA data;
    humidity->humidityRead(data);
    if (data.temperatureValid) {
        temp = data.temperature;
    }
    return temp;
}

/* Returns the temperature in Celsius from the pressure sensor */
float Wrapper::temperature_from_pressure(void) {
    init_pressure();
    float temp = 0.0;
    RTIMU_DATA data;
    pressure->pressureRead(data);
    if (data.temperatureValid) {
        temp = data.temperature;
    }
    return temp;
}

float Wrapper::temperature(void) {
    // Same as in Python SenseHat
    // Returns the temperature on Celsius
    return temperature_from_humidity();
}

/***** IMU sensor *****/

/* Initialises the IMU sensor via RTIMU */
void Wrapper::init_imu(void) {
    if (!imu_init) {
        imu_init = imu->IMUInit();
        if (imu_init) {
            imu_poll_interval = imu->IMUGetPollInterval() * 1000;
            // Enable everything on the IMU
            set_imu_config(true, true, true);
        } else {
            throw "Could not initialise IMU";
        }
    }
}

/* Enables and disables the gyroscope, accelerometer and/or magnetometer
 * inpput to the orientation functions */
void Wrapper::set_imu_config(bool compass_enabled, bool gyro_enabled, bool accel_enabled) {
    init_imu(); // Ensure the IMU is initialised

    if (_compass_enabled != compass_enabled) {
        _compass_enabled = compass_enabled;
        imu->setCompassEnable(_compass_enabled);
    }
    if (_gyro_enabled != gyro_enabled) {
        _gyro_enabled = gyro_enabled;
        imu->setCompassEnable(_gyro_enabled);
    }
    if (_accel_enabled != accel_enabled) {
        _accel_enabled = accel_enabled;
        imu->setCompassEnable(_accel_enabled);
    }
}

/* Tries to read the IMU sensor three times before giving up */
bool Wrapper::read_imu(void) {
    init_imu(); // Ensure the IMU is initialised

    int attempts = 0;
    bool success = false;

    while (!success && (attempts < 3)) {
        success = imu->IMURead();
        attempts += 1;
        usleep(imu_poll_interval);
    }
    return success;
}

/* Returns a struct to represent the current orientation in
 * radians using the aicraft principal axes of pitch, roll and yaw */
Orientation Wrapper::orientation_radians(void) {
    if (read_imu()) {
        RTIMU_DATA data = imu->getIMUData();
        if (data.fusionPoseValid) {
            last_orientation.roll = data.fusionPose.x();
            last_orientation.pitch = data.fusionPose.y();
            last_orientation.yaw = data.fusionPose.z();
        }
    }
    return last_orientation;
}

/* Returns a struct to represent the current orientation in
 * degrees, 0 to 360, using the aircraft axes of
 * pitch, roll and yaw */
Orientation Wrapper::orientation_degrees(void) {
    Orientation ori = orientation_radians();
    ori.roll = ori.roll * RTMATH_RAD_TO_DEGREE;
    ori.roll = ori.roll < 0.0 ? ori.roll + 360.0 : ori.roll;
    ori.pitch = ori.pitch * RTMATH_RAD_TO_DEGREE;
    ori.pitch = ori.pitch < 0.0 ? ori.pitch + 360.0 : ori.pitch;
    ori.yaw = ori.yaw * RTMATH_RAD_TO_DEGREE;
    ori.yaw = ori.yaw < 0.0 ? ori.yaw + 360.0 : ori.yaw;
    return ori;
}

Orientation Wrapper::orientation(void) {
    return orientation_degrees();
}

/* Gets the direction of North form the magnetometer in degrees */
float Wrapper::compass(void) {
    set_imu_config(true, false, false);
    return orientation().yaw;
}

/* Megnetometer x y z raw data in uT (micro teslas) */
Coordinates Wrapper::compass_raw(void) {
    if (read_imu()) {
        RTIMU_DATA data = imu->getIMUData();
        if (data.compassValid) {
            last_compass.x = data.compass.x();
            last_compass.y = data.compass.y();
            last_compass.z = data.compass.z();
        }
    }
    return last_compass;
}

/* Get orientation in degrees from the gyroscope only */
Orientation Wrapper::gyroscope(void) {
    set_imu_config(false, true, false);
    return orientation_degrees();
}

/* Gyroscope x y z raw data in radians per second */
Coordinates Wrapper::gyroscope_raw(void) {
    if (read_imu()) {
        RTIMU_DATA data = imu->getIMUData();
        if (data.gyroValid) {
            last_gyro.x = data.gyro.x();
            last_gyro.y = data.gyro.y();
            last_gyro.z = data.gyro.z();
        }
    }
    return last_gyro;
}

/* Gets the orientation in degrees from the accelerometer only */
Orientation Wrapper::accelerometer(void) {
    set_imu_config(false, false, true);
    return orientation_degrees();
}

/* Accelerometer x y z raw data in Gs */
Coordinates Wrapper::accelerometer_raw(void) {
    if (read_imu()) {
        RTIMU_DATA data = imu->getIMUData();
        if (data.accelValid) {
            last_accel.x = data.accel.x();
            last_accel.y = data.accel.y();
            last_accel.z = data.accel.z();
        }
    }
    return last_accel;
}

/***** Framebuffer and LED *****/

// Tries to locate and mmap the framebuffer
void Wrapper::open_framebuffer(void) {
    struct fb_fix_screeninfo fix_info;
    glob_t globbuf;

    int err = glob("/dev/fb*", 0, NULL, &globbuf);
    if (!err) {
        for (size_t i = 0; i < globbuf.gl_pathc; i++) {
            int fd = open(globbuf.gl_pathv[i], O_RDWR);
            if (fd < 0) {
                continue;
            }
            ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
            if (strcmp(RPI_SENSE_FB, fix_info.id) == 0) {
                fb = (framebuffer *) mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                close(fd);
                if (!fb) {
                    throw "Could not open framebuffer";
                }
                return;
            }
            close(fd);
        }
        globfree(&globbuf);
    }
    throw "Could not locate the framebuffer";
}

void Wrapper::set_pixel(uint16_t color, uint8_t x, uint8_t y) {
    if (x > 7) {
        throw "X coordinates value must be between 0 and 7";
    }
    if (y > 7) {
        throw "Y coordinates value must be between 0 and 7";
    }
    fb->frame[y][x] = color;

}
void Wrapper::set_pixels(uint16_t color) {
    uint8_t i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            fb->frame[i][j] = color;
        }
    }
}

void Wrapper::set_image(uint16_t image[64]) {
    uint8_t i;
    for (i = 0; i < 64; i++) {
        fb->frame[i / 8][i % 8] = image[i];
    }
}

void Wrapper::clear(void) {
    memset(fb, 0, 128);
}

/***** Code for C functions *****/

SenseHatSensors * SenseHatSensors_new(void) {
    try {
        Wrapper *wrapper = new Wrapper();
        return reinterpret_cast<SenseHatSensors*>(wrapper);
    } catch (...) {
        return NULL;
    }
}

void SenseHatSensors_delete(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        delete wrapper;
    } catch (...) {}
}

/***** IMU config *****/


void set_imu_config(SenseHatSensors *sense, Bool_t compass_enabled, Bool_t gyro_enabled, Bool_t accel_enabled) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        wrapper->set_imu_config(compass_enabled ? true : false,
                gyro_enabled ? true : false,
                accel_enabled ? true : false);
    } catch (...) {}
}
/***** Sensors *****/

float get_humidity(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->get_humidity();
    } catch (...) {
        return 0.0;
    }
}
float get_pressure(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->get_pressure();
    } catch (...) {
        return 0.0;
    }
}
float get_temperature_from_humidity(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->temperature_from_humidity();
    } catch (...) {
        return 0.0;
    }
}

float get_temperature_from_pressure(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->temperature_from_pressure();
    } catch (...) {
        return 0.0;
    }
}
float get_temperature(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->temperature();
    } catch (...) {
        return 0.0;
    }
}

Orientation get_orientation_radians(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->orientation_radians();
    } catch (...) {
        Orientation ret = {}; // Zero out all values
        return ret;
    }
}

Orientation get_orientation_degrees(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->orientation_degrees();
    } catch (...) {
        Orientation ret = {}; // Zero out all values
        return ret;
    }
}

Orientation get_orientation(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->orientation();
    } catch (...) {
        Orientation ret = {}; // Zero out all values
        return ret;
    }
}

float get_compass(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->compass();
    } catch (...) {
        return 0.0;
    }
}

Coordinates get_compass_raw(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->compass_raw();
    } catch (...) {
        Coordinates ret = {}; // Zero out all values
        return ret;
    }
}

Orientation get_gyroscope(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->gyroscope();
    } catch (...) {
        Orientation ret = {}; // Zero out all values
        return ret;
    }
}

Coordinates get_gyroscope_raw(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->gyroscope_raw();
    } catch (...) {
        Coordinates ret = {}; // Zero out all values
        return ret;
    }
}

Orientation get_accelerometer(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->accelerometer();
    } catch (...) {
        Orientation ret = {}; // Zero out all values
        return ret;
    }
}

Coordinates get_accelerometer_raw(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->accelerometer_raw();
    } catch (...) {
        Coordinates ret = {}; // Zero out all values
        return ret;
    }
}

/***** Framebuffer and LED *****/

void set_pixel(SenseHatSensors *sense, uint16_t color, uint8_t x, uint8_t y) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        wrapper->set_pixel(color, x, y);
    } catch (...) {}
}

void set_pixels(SenseHatSensors *sense, uint16_t color) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        wrapper->set_pixels(color);
    } catch (...) {}
}

void set_image(SenseHatSensors *sense, uint16_t image[64]) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        wrapper->set_image(image);
    } catch (...) {}
}

void clear(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        wrapper->clear();
    } catch (...) {}
}

