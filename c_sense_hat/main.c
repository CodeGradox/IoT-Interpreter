#define _BSD_SOURCE
#include "SenseHatSensors.h"
#include "unistd.h"
#include "stdio.h"

void sleepms(uint32_t time) {
    usleep(1000 * time);
}

void draw_ok(SenseHatSensors *sense) {
    uint16_t g = 0x07E0; // green
    uint16_t b = 0x0000; // blank
    uint16_t ok[64] = {
        g, g, g, b, g, b, b, g,
        g, b, g, b, g, b, g, b,
        g, b, g, b, g, g, b, b,
        g, b, g, b, g, b, g, b,
        g, g, g, b, g, b, b, g,
        b, b, b, b, b, b, b, b,
        b, b, b, b, b, b, b, b,
        b, b, b, b, b, b, b, b};
    set_image(sense, ok);
}

void draw_err(SenseHatSensors *sense) {
    uint16_t r = 0xF800; // red
    uint16_t b = 0x0000; // blank
    uint16_t ok[64] = {
        r, r, b, b, b, b, b, b,
        r, b, b, b, b, b, b, b,
        r, r, b, r, r, b, r, r,
        r, b, b, r, b, b, r, b,
        r, r, b, r, b, b, r, b,
        b, b, b, b, b, b, b, b,
        b, b, b, b, b, b, b, b,
        b, b, b, b, b, b, b, b};
    set_image(sense, ok);
}

void draw_smile(SenseHatSensors *sense) {
    uint16_t c = 0x041F; // blue
    uint16_t b = 0x0000; // blank
    uint16_t ok[64] = {
        b, b, c, c, c, c, b, b,
        b, c, b, b, b, b, c, b,
        c, b, c, b, b, c, b, c,
        c, b, b, b, b, b, b, c,
        c, b, c, b, b, c, b, c,
        c, b, b, c, c, b, b, c,
        b, c, b, b, b, b, c, b,
        b, b, c, c, c, c, b, b};
    set_image(sense, ok);
}

void draw_bluetooth(SenseHatSensors *sense) {
    uint16_t c = 0x041F; // white
    uint16_t b = 0xFFFF; // blue
    uint16_t ok[64] = {
        b, b, b, b, b, c, c, c,
        c, b, c, c, b, c, c, c,
        c, c, b, c, b, c, c, c,
        c, c, c, b, b, b, b, b,
        c, b, b, b, b, c, c, b,
        c, c, c, b, c, b, c, b,
        c, c, c, b, c, c, b, b,
        c, c, c, c, c, c, c, b};
    set_image(sense, ok);
}

void test1(SenseHatSensors *sense) {
    uint8_t i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            set_pixel(sense, 0xFFFF, i, j);
        }
        sleepms(200);
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            set_pixel(sense, 0xF800, j, i);
        }
        sleepms(200);
}

    for (i = 7; i != 255 ; i--) {
        for (j = 0; j < 8; j++) {
            set_pixel(sense, 0x041F, i, j);
        }
        sleepms(200);
    }

    for (i = 7; i != 255; i--) {
        for (j = 7; j != 255; j--) {
            set_pixel(sense, 0x87E0, j, i);
        }
        sleepms(200);
    }

    sleepms(500);
}

int main(void) {
    Orientation ori;
    Coordinates cor;

    SenseHatSensors *sense = SenseHatSensors_new();
    if (sense == NULL) {
        goto Err;
    }

    set_imu_config(sense, TRUE, TRUE, TRUE);

    printf("Temperature: %f\n", get_temperature(sense));
    printf("Temperature from humidity: %f\n", get_temperature_from_humidity(sense));
    printf("Temperature from pressure: %f\n", get_temperature_from_pressure(sense));
    printf("Humidity: %f\n", get_humidity(sense));
    printf("Pressure: %f\n", get_pressure(sense));

    ori = get_orientation_radians(sense);
    printf("Orientation radians: roll: %f, pitch:%f, yaw: %f\n", ori.roll, ori.pitch, ori.yaw);

    ori = get_orientation_degrees(sense);
    printf("Orientation degrees: roll: %f, pitch:%f, yaw: %f\n", ori.roll, ori.pitch, ori.yaw);

    ori = get_orientation(sense);
    printf("Orientation: roll: %f, pitch:%f, yaw: %f\n", ori.roll, ori.pitch, ori.yaw);

    printf("Compass: %f\n", get_compass(sense));

    cor = get_compass_raw(sense);
    printf("Compass raw: x: %f, y:%f, z: %f\n", cor.x, cor.y, cor.z);

    ori = get_gyroscope(sense);
    printf("Gyroscope: roll: %f, pitch:%f, yaw: %f\n", ori.roll, ori.pitch, ori.yaw);

    cor = get_gyroscope_raw(sense);
    printf("Gyroscope raw: x: %f, y:%f, z: %f\n", cor.x, cor.y, cor.z);

    ori = get_accelerometer(sense);
    printf("Accelerometer: roll: %f, pitch:%f, yaw: %f\n", ori.roll, ori.pitch, ori.yaw);

    cor = get_accelerometer_raw(sense);
    printf("Accelerometer raw: x: %f, y:%f, z: %f\n", cor.x, cor.y, cor.z);

    SenseHatSensors_delete(sense);

    clear(sense);

    /*draw_ok(sense);
    sleepms(1000);
    draw_err(sense);
    sleepms(1000);
    draw_smile(sense);
    sleepms(1000);
    draw_bluetooth(sense);
    sleepms(1000);
    clear(sense);*/
    /*test1(sense);
    sleepms(1000);
    clear(sense);*/
Err:
    return 0;
}

