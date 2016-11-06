typedef void C_RTIMUSettings;
typedef void C_RTIMU;

// constructor and destructor for RTIMUSettings
C_RTIMUSettings * C_RTIMUSettings_new(const char *productType);
void C_RTIMUSettings_destroy(C_RTIMUSettings *s);

C_RTIMU * C_RTIMU_new(C_RTIMUSettings *s);
void C_RTIMU_destroy(C_RTIMU *r);

