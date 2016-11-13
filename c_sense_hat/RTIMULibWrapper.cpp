#include "RTIMULib.h"

extern "C" {
    #include "RTIMULibWrapper.h"
}

// Dummy class for the RTIMU_DATA struct
// This is the easiest way to send the struct back to C code
class H_RTIMU_DATA {
    public:
        RTIMU_DATA imuData;
};

// ##### SETTINGS #####
// Settings constructor
C_RTIMUSettings * C_RTIMUSettings_new(const char *productType) {
    try {
        RTIMUSettings *settings = new RTIMUSettings(productType);
        return reinterpret_cast<C_RTIMUSettings*>(settings);
    } catch (...) {
        return NULL;
    }
}

// Settings destructor
int C_RTIMUSettings_destroy(C_RTIMUSettings *s) {
    try {
        delete reinterpret_cast<RTIMUSettings*>(s);
        return 0;
    } catch (...) {
        return -1;
    }
}

// ##### IMU #####
// IMU Constructor
C_RTIMU * C_RTIMU_new(C_RTIMUSettings *s) {
    try {
        RTIMU *rtimu = RTIMU::createIMU(reinterpret_cast<RTIMUSettings*>(s));
        if ((rtimu == NULL) || (rtimu->IMUType() == RTIMU_TYPE_NULL)) {
            // No IMU found!
            return NULL;
        }
        return reinterpret_cast<C_RTIMU*>(rtimu);
        return 0;
    } catch (...) {
        return NULL;
    }
}

// IMU destructor
int C_RTIMU_destroy(C_RTIMU *imu) {
    try {
        delete reinterpret_cast<RTIMU*>(imu);
        return 0;
    } catch (...) {
        return -1;
    }
}

// IMU init
int imu_init(C_RTIMU *imu) {
    try {
        bool ret = reinterpret_cast<RTIMU*>(imu)->IMUInit();
        if (!ret) {
            // TODO imu is not initialized properly, return a different value?
            return -1;
        }
        return 0;
    } catch (...) {
        return -1;
    }
}

// IMU config
int set_imu_config(C_RTIMU *imu, float slerpPower, int compass, int gyro, int accel) {
    try {
        RTIMU *tmp = reinterpret_cast<RTIMU*>(imu);
        tmp->setSlerpPower((RTFLOAT) slerpPower);
        tmp->setCompassEnable(compass ? true : false);
        tmp->setGyroEnable(gyro ? true : false);
        tmp->setAccelEnable(accel ? true : false);
        return 0;
    } catch (...) {
        return -1;
    }
}

// Check if IMU can be read
int imu_read(C_RTIMU *imu) {
    try {
        return reinterpret_cast<RTIMU*>(imu)->IMURead() ? TRUE : FALSE;
    } catch (...) {
        return -1;
    }
        
}

// ##### RTIMU DATA #####
// Get IMU data (constructor)
C_RTIMU_DATA * get_imu_data(C_RTIMU *imu) {
    try {
        H_RTIMU_DATA *data = new H_RTIMU_DATA();
        data->imuData = reinterpret_cast<RTIMU*>(imu)->getIMUData();
        return reinterpret_cast<C_RTIMU_DATA*>(data);
    } catch (...) {
        return NULL;
    }
}

// RTIMU DATA Destructor
int destroy_imu_data(C_RTIMU_DATA *data) {
    try {
        delete reinterpret_cast<H_RTIMU_DATA*>(data);
        return 0;
    } catch (...) {
        return -1;
    }
}

// ##### PRESSURE #####
// Pressure constructor
C_RTPressure * C_create_pressure(C_RTIMUSettings *s) {
    try {
        RTPressure *pressure = RTPressure::createPressure(
                reinterpret_cast<RTIMUSettings*>(s));
        return reinterpret_cast<C_RTPressure*>(pressure);
    } catch (...) {
        return NULL;
    }
}

// Pressure destructor
int C_RTPressure_destroy(C_RTPressure *p) {
    try {
        delete reinterpret_cast<RTPressure*>(p);
        return 0;
    } catch (...) {
        return -1;
    }
}

// Pressure init
int pressure_init(C_RTPressure *p) {
    try {
        RTPressure *pressure = reinterpret_cast<RTPressure*>(p);
        if (pressure != NULL) {
            if (!pressure->pressureInit()) {
                // Could not init pressure
                // use a different code for this error
                return -1;
            }
            return 0;
        }
    } catch (...) {}
    return -1;
}

// Pressure read to data struct
int pressure_read(C_RTPressure *p, C_RTIMU_DATA *data) {
    try {
        RTPressure *pressure = reinterpret_cast<RTPressure*>(p);
        H_RTIMU_DATA *imuData = reinterpret_cast<H_RTIMU_DATA*>(data);
        if (pressure != NULL && imuData != NULL) {
            pressure->pressureRead(imuData->imuData);
        }
        return 0;
    } catch (...) {}
    return -1;
}

// Get pressure data
float pressure_get(C_RTIMU_DATA *data) {
    // Alternatively, get RTIMU_DATA here only and read data from it
    try {
        H_RTIMU_DATA *imuData = reinterpret_cast<H_RTIMU_DATA*>(data);
        if (imuData != NULL) {
            return imuData->imuData.pressure;
        }
    } catch (...) {}
    return 0.0;
}

// ##### HUMIDITY #####
// Hunidity constructor
C_RTHumidity * C_create_humidity(C_RTIMUSettings *s) {
    try {
        RTHumidity *humidity = RTHumidity::createHumidity(
                (RTIMUSettings*) reinterpret_cast<RTIMUSettings*>(s));
        return reinterpret_cast<C_RTHumidity*>(humidity);
    } catch (...) {
        return NULL;
    }
}

// Humidity destructor
int C_RTHumidity_destroy(C_RTHumidity *p) {
    try {
        delete reinterpret_cast<RTHumidity*>(p);
        return 0;
    } catch (...) {
        return -1;
    }
}

// Humidity init
int humidity_init(C_RTHumidity *h) {
    try {
        RTHumidity *humidity = reinterpret_cast<RTHumidity*>(h);
        if (humidity != NULL) {
            if (humidity->humidityInit()) {
                // See pressure_init()
                return -1;
            }
            return 0;
        }
    } catch (...) {}
    return -1;
}

// Writes the humidity into the data struct
int humidity_read(C_RTHumidity *h, C_RTIMU_DATA *data) {
    try {
        RTHumidity *humidity = reinterpret_cast<RTHumidity*>(h);
        H_RTIMU_DATA *imuData = reinterpret_cast<H_RTIMU_DATA*>(data);
        if (humidity != NULL && imuData != NULL) {
            humidity->humidityRead(imuData->imuData);
        }
        return 0;
    } catch (...) {}
    return -1;
}

// Get humidity data
float humidity_get(C_RTIMU_DATA *data) {
    // Alternatively, get RTIMU_DATA here only and read data from it
    try {
        H_RTIMU_DATA *imuData = reinterpret_cast<H_RTIMU_DATA*>(data);
        if (imuData != NULL) {
            return imuData->imuData.humidity;
        }
    } catch (...) {}
    return 0.0;
}

// Temperature
float temperature_get(C_RTIMU_DATA *data) {
    try {
        H_RTIMU_DATA *imuData = reinterpret_cast<H_RTIMU_DATA*>(data);
        if (imuData != NULL) {
            return imuData->imuData.temperature;
        }
    } catch (...) {}
    return 0.0;
}
