#define _GNU_SOURCE // Required to use versionsort in scandir
#define DEV_FB "/dev/"
#define FB_DEV_NAME "fb"

#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "fcntl.h"
#include "unistd.h"
#include "dirent.h"
#include "time.h"

#include "sys/ioctl.h"
#include "linux/fb.h"
#include "sys/mman.h"

#include "SenseHatSensors.h"

typedef struct {
    uint16_t pixels[8][8];
} framebuffer;

void test1(framebuffer * fb);

static int is_framebuffer_device(const struct dirent *dir) {
    return strncmp(FB_DEV_NAME, dir->d_name,
            strlen(FB_DEV_NAME) - 1) == 0;
}

static int open_fbdev(const char *dev_name) {
    struct dirent **namelist;
    int i, ndev;
    int fd = -1;
    struct fb_fix_screeninfo fix_info;

    // Opens all fb* devices in /dev and puts the path name into
    // the namelist struct. The list is sorted with versionsort.
    ndev = scandir(DEV_FB, &namelist, is_framebuffer_device, versionsort);
    if (ndev <= 0) {
        return ndev;
    }

    for (i = 0; i < ndev; i++) {
        char fname[64];

        // Writes the full path of the fb file into fname
        snprintf(fname, sizeof(fname),
                "%s/%s", DEV_FB, namelist[i]->d_name);
        fd = open(fname, O_RDWR);
        if (fd < 0) {
            continue;
        }
        // Reads the frame buffer into a special struct which
        // gives us the ability to read its information.
        // We can now read the ID of the framebuffer.
        ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
        if (strcmp(dev_name, fix_info.id) == 0) {
            break;
        }
        // Wrong framebuffer
        close(fd);
        fd = -1;
    }

    // Cleanup
    for (i = 0; i < ndev; i++) {
        free(namelist[i]);
    }

    return fd;
}

void set_pixel(framebuffer *fb, uint8_t x, uint8_t y, uint16_t color) {
    assert(x <= 7 && "X position must be between 0 and 7");
    assert(y <= 7 && "Y position must be between 0 and 7");
    fb->pixels[y][x] = color;
}

void set_pixels(framebuffer *fb, uint16_t color) {
    uint16_t i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            fb->pixels[i][j] = color;
        }
    }
}

void set_image(framebuffer *fb, uint16_t image[64]) {
    int i;
    for (i = 0; i < 64; i++) {
        fb->pixels[i / 8][i % 8] = image[i];
    }
}

void clear(framebuffer *fb) {
    // Sets all values in the mapping to zero.
    // In other words, clears the screen.
    memset(fb, 0, 128);
}

void sleepms(useconds_t time) {
    usleep(1000 * time);
}

void draw_ok(framebuffer *fb) {
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
    set_image(fb, ok);
}

void draw_err(framebuffer *fb) {
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
    set_image(fb, ok);
}

void draw_smile(framebuffer *fb) {
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
    set_image(fb, ok);
}

void draw_bluetooth(framebuffer *fb) {
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
    set_image(fb, ok);
}

void test1(framebuffer *fb) {
    uint8_t i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            set_pixel(fb, i, j, 0xFFFF);
        }
        sleepms(200);
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            set_pixel(fb, j, i, 0xF800);
        }
        sleepms(200);
    }

    for (i = 7; i != 255 ; i--) {
        for (j = 0; j < 8; j++) {
            set_pixel(fb, i, j, 0x041F);
        }
        sleepms(200);
    }

    for (i = 7; i != 255; i--) {
        for (j = 7; j != 255; j--) {
            set_pixel(fb, j, i, 0x87E0);
        }
        sleepms(200);
    }

    sleepms(500);
    clear(fb);
}

int main(void) {
    int ret = 0;
    // framebuffer file descriptor
    int fbfd = 0;
    framebuffer *fb;

    fbfd = open_fbdev("RPi-Sense FB");
    if (fbfd <= 0) {
        ret = fbfd;
        printf("Error: cannot open framebuffer device.\n");
        return ret;
    }

    // Create a mapping in the virtual address space for this process.
    // Maps the device into memory.
    fb =  (framebuffer *) mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (!fb) {
        ret = EXIT_FAILURE;
        printf("Failed to mmap the device.\n");
        close(fbfd);
        return ret;
    }

    SenseHatSensors *sense = SenseHatSensors_new();
    float temp = get_temperature(sense);
    printf("Temperature: %f\n", temp);

    draw_ok(fb);
    sleep(1);
    draw_err(fb);
    sleep(1);
    draw_smile(fb);
    sleep(1);
    draw_bluetooth(fb);
    sleep(1);
    clear(fb);
    
    SenseHatSensors_delete(sense);
    return ret;
}

