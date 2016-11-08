#include "RTIMULib.h"

extern "C" {
    #include "RTIMULibWrapper.h"
}

class H_RTIMU_DATA {
    public:
        RTIMU_DATA imuData;
};

// SETTINGS CONSTRUCTOR
C_RTIMUSettings * C_RTIMUSettings_new(const char *productType) {
    return reinterpret_cast<C_RTIMUSettings*>(new RTIMUSettings(productType));
}

// SETTINGS DESTRUCTOR
void C_RTIMUSettings_destroy(C_RTIMUSettings *s) {
    delete reinterpret_cast<RTIMUSettings*>(s);
}

// IMU CONSTRUCTOR
C_RTIMU * C_RTIMU_new(C_RTIMUSettings *s) {
    // Should check if IMU is NULL || imu->IMUType() == RTIMU_TYPE_NULL
    // Return error if that is teh caste.
    // We cannot exit in this function because we need to clean up the Settings object
    // Use goto to clean up in C
    return reinterpret_cast<C_RTIMU*>(RTIMU::createIMU(reinterpret_cast<RTIMUSettings*>(s)));
}

// IMU DESTRUCTOR
void C_RTIMU_destroy(C_RTIMU *r) {
    delete reinterpret_cast<RTIMU*>(r);
}

// IMU INIT
void imu_init(C_RTIMU *imu) {
    reinterpret_cast<RTIMU*>(imu)->IMUInit();
}

// IMU CONFIG
void set_imu_config(C_RTIMU *imu, float slerpPower, int compass, int gyro, int accel) {
    RTIMU *tmp = reinterpret_cast<RTIMU*>(imu);
    // Check if IMU is initialised or not first

    tmp->setSlerpPower(slerpPower);
    tmp->setCompassEnable(compass ? true : false);
    tmp->setGyroEnable(gyro ? true : false);
    tmp->setAccelEnable(accel ? true : false);
}

// CHECK IF IMU CAN BE READ
int imu_read(C_RTIMU *imu) {
    return reinterpret_cast<RTIMU*>(imu)->IMURead() ? 1 : 0;
}

// GET IMU DATA
C_RTIMU_DATA * get_imu_data(C_RTIMU *imu) {
    H_RTIMU_DATA *imuData = new H_RTIMU_DATA();
    imuData->imuData = reinterpret_cast<RTIMU*>(imu)->getIMUData();

    //return reinterpret_cast<C_RTIMU_DATA>(reinterpret_cast<RTIMU*>(imu)->getIMUData());

    return reinterpret_cast<C_RTIMU_DATA*>(imuData);
}

void destroy_imu_data(C_RTIMU_DATA *imuData) {
    delete reinterpret_cast<H_RTIMU_DATA*>(imuData);
}

// PRESSURE CMSTRUCTOR
C_RTPressure * C_create_pressure(C_RTIMUSettings *s) {
    return reinterpret_cast<C_RTPressure*>(RTPressure::createPressure(
                reinterpret_cast<RTIMUSettings*>(s)));
}

// PRESSURE DESTRUCTOR
void C_RTPressure_destroy(C_RTPressure *p) {
    delete reinterpret_cast<RTPressure*>(p);
}

// PRESSURE INIT
void pressure_init(C_RTPressure *p) {
    RTPressure *pressure = reinterpret_cast<RTPressure*>(p);
    if (pressure != NULL) {
        pressure->pressureInit();
    }
}

// HUMIDITY CONSTRUCTOR
C_RTHumidity * C_create_humidity(C_RTIMUSettings *s) {
    return reinterpret_cast<C_RTHumidity*>(RTHumidity::createHumidity(
                reinterpret_cast<RTIMUSettings*>(s)));
}

// HUMIDITY DESTRUCTOR
void C_RTHumidity_destroy(C_RTHumidity *p) {
    delete reinterpret_cast<RTHumidity*>(p);
}

void humidity_init(C_RTHumidity *h) {
    RTHumidity *humidity = reinterpret_cast<RTHumidity*>(h);
    if (humidity != NULL) {
        humidity->humidityInit();
    }
}
