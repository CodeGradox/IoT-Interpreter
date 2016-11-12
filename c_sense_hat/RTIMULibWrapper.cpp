#include "RTIMULib.h"

extern "C" {
    #include "RTIMULibWrapper.h"
}

// Dummy class for RTIMU_DATA
// This is the easiest way to send the struct back to C code
class H_RTIMU_DATA {
    public:
        RTIMU_DATA imuData;
};

// SETTINGS CONSTRUCTOR
C_RTIMUSettings * C_RTIMUSettings_new(const char *productType) {
    try {
        RTIMUSettings *settings = new RTIMUSettings(productType);
        return reinterpret_cast<C_RTIMUSettings*>(settings);
    } catch (...) {
        return NULL;
    }
}

// SETTINGS DESTRUCTOR
int C_RTIMUSettings_destroy(C_RTIMUSettings *s) {
    try {
        delete reinterpret_cast<RTIMUSettings*>(s);
        return 0;
    } catch (...) {
        return -1;
    }
}

// IMU CONSTRUCTOR
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

// IMU DESTRUCTOR
int C_RTIMU_destroy(C_RTIMU *imu) {
    try {
        delete reinterpret_cast<RTIMU*>(imu);
        return 0;
    } catch (...) {
        return -1;
    }
}

// IMU INIT
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

// IMU CONFIG
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

// CHECK IF IMU CAN BE READ
int imu_read(C_RTIMU *imu) {
    try {
        return reinterpret_cast<RTIMU*>(imu)->IMURead() ? TRUE : FALSE;
    } catch (...) {
        return -1;
    }
        
}

// GET IMU DATA
C_RTIMU_DATA * get_imu_data(C_RTIMU *imu) {
    try {
        H_RTIMU_DATA *imuData = new H_RTIMU_DATA();
        imuData->imuData = reinterpret_cast<RTIMU*>(imu)->getIMUData();
        return reinterpret_cast<C_RTIMU_DATA*>(imuData);
    } catch (...) {
        return NULL;
    }
}

int destroy_imu_data(C_RTIMU_DATA *data) {
    try {
        delete reinterpret_cast<H_RTIMU_DATA*>(data);
        return 0;
    } catch (...) {
        return -1;
    }
}

// PRESSURE CMSTRUCTOR
C_RTPressure * C_create_pressure(C_RTIMUSettings *s) {
    try {
        RTPressure *pressure = RTPressure::createPressure(
                reinterpret_cast<RTIMUSettings*>(s));
        return reinterpret_cast<C_RTPressure*>(pressure);
    } catch (...) {
        return NULL;
    }
}

// PRESSURE DESTRUCTOR
int C_RTPressure_destroy(C_RTPressure *p) {
    try {
        delete reinterpret_cast<RTPressure*>(p);
        return 0;
    } catch (...) {
        return -1;
    }
}

// PRESSURE INIT
int pressure_init(C_RTPressure *p) {
    try {
        RTPressure *pressure = reinterpret_cast<RTPressure*>(p);
        if (pressure != NULL) {
            pressure->pressureInit();
        }
        return 0;
    } catch (...) {
        return -1;
    }
}

// HUMIDITY CONSTRUCTOR
C_RTHumidity * C_create_humidity(C_RTIMUSettings *s) {
    try {
        RTHumidity *humidity = RTHumidity::createHumidity(
                (RTIMUSettings*) reinterpret_cast<RTIMUSettings*>(s));
        return reinterpret_cast<C_RTHumidity*>(humidity);
    } catch (...) {
        return NULL;
    }
}

// HUMIDITY DESTRUCTOR
int C_RTHumidity_destroy(C_RTHumidity *p) {
    try {
        delete reinterpret_cast<RTHumidity*>(p);
        return 0;
    } catch (...) {
        return -1;
    }
}

int humidity_init(C_RTHumidity *h) {
    try {
        RTHumidity *humidity = reinterpret_cast<RTHumidity*>(h);
        if (humidity != NULL) {
            humidity->humidityInit();
        }
        return 0;
    } catch (...) {
        return -1;
    }
}
