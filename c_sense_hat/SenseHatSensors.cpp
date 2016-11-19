#include "RTIMULib.h"
#include "cstdlib"

extern "C" {
    #include "SenseHatSensors.h"
}

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

    uint16_t frame[8][8];
};

/* Constructor*/
Wrapper::Wrapper() {
    char ini_name[80];
    snprintf(ini_name, sizeof(ini_name), "%s/.config/sense_hat/RTIMULib", std::getenv("HOME"));
    printf("path: %s\n", ini_name);

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
