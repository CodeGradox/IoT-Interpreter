#include "RTIMULib.h"

extern "C" {
    #include "SenseHatSensors.h"
}

class Wrapper {
public:
    Wrapper();      // Constructor
    ~Wrapper();     // Destructor
    void init_imu(void);
    void set_imu_config(bool compass_enabled, bool gyro_enabled, bool accel_enabled);
    void init_humidity(void);
    void init_pressure(void);
    float get_humidity(void);
    float get_pressure(void);
    float get_temperature_from_humidity(void);
    float get_temperature_from_pressure(void);
    float get_temperature(void);

private:
    RTIMUSettings *settings;
    RTIMU *imu;
    bool imu_init; // Will be initialised as and when needed
    RTPressure *pressure;
    bool pressure_init; // Will be initialised as and when needed
    RTHumidity *humidity;
    bool humidity_init; // Will be initialised as and when needed
};

Wrapper::Wrapper() {
    settings = new RTIMUSettings("RTIMULib");
    imu = RTIMU::createIMU(settings);
    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        // No IMU found
        // Throw exception here
    }
    imu_init = false;
    pressure = RTPressure::createPressure(settings);
    pressure_init = false;
    humidity = RTHumidity::createHumidity(settings);
    humidity_init = false;
}

Wrapper::~Wrapper() {
    printf("Deleting Wrapper class!\n");
    delete settings;
    delete imu;
    delete pressure;
    delete humidity;
}

void Wrapper::init_humidity(void) {
    if (!humidity_init) {
        humidity_init = humidity->humidityInit();
        if (!humidity_init) {
            // Throw exception here, could not init humidity
        }
    }
}

void Wrapper::init_pressure(void) {
    if (!pressure_init) {
        pressure_init = pressure->pressureInit();
        if (!pressure_init) {
            // Throw exception here, could not init pressure
        }
    }
}

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

float Wrapper::get_temperature_from_humidity(void) {
    init_humidity(); // Ensure humidity is initialised
    float temp = 0.0;
    RTIMU_DATA data;
    humidity->humidityRead(data);
    if (data.temperatureValid) {
        temp = data.temperature;
    }
    return temp;
}

float Wrapper::get_temperature_from_pressure(void) {
    init_pressure();
    float temp = 0.0;
    RTIMU_DATA data;
    pressure->pressureRead(data);
    if (data.temperatureValid) {
        temp = data.temperature;
    }
    return temp;
}

float Wrapper::get_temperature(void) {
    // Same as in Python SenseHat
    // Returns the temperature on Celsius
    return get_temperature_from_humidity();
}

// ##### Code for C functions #####

SenseHatSensors * SenseHatSensors_new(void) {
    try {
        Wrapper *wrapper = new Wrapper();
        return reinterpret_cast<SenseHatSensors*>(wrapper);
    } catch (...) {
        return NULL;
    }
}

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
        return wrapper->get_temperature_from_humidity();
    } catch (...) {
        return 0.0;
    }
}

float get_temperature_from_pressure(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->get_temperature_from_pressure();
    } catch (...) {
        return 0.0;
    }
}
float get_temperature(SenseHatSensors *sense) {
    try {
        Wrapper *wrapper = reinterpret_cast<Wrapper*>(sense);
        return wrapper->get_temperature();
    } catch (...) {
        return 0.0;
    }
}

