#include "RTIMULib.h"

extern "C" {
    #include "RTIMULibWrapper.h"
}

C_RTIMUSettings * C_RTIMUSettings_new(const char *productType) {
    return reinterpret_cast<C_RTIMUSettings*>(new RTIMUSettings(productType));
}

void C_RTIMUSettings_destroy(C_RTIMUSettings *s) {
    delete reinterpret_cast<RTIMUSettings*>(s);
}

C_RTIMU * C_RTIMU_new(C_RTIMUSettings *s) {
    return reinterpret_cast<C_RTIMU*>(RTIMU::createIMU(reinterpret_cast<RTIMUSettings*>(s)));
}

void C_RTIMU_destroy(C_RTIMU *r) {
    delete reinterpret_cast<RTIMU*>(r);
}
