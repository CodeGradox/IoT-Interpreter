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

#include "color.h"

typedef struct {
    uint16_t pixels[8][8];
} framebuffer;

// struct frame *framebuffer;

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

void set_pixel_rgb(framebuffer *fb, uint8_t x, uint8_t y, Rgb *color) {
    fb->pixels[y][x] = convert_rgb(color);
}

void set_pixel_hex565(framebuffer *fb, uint8_t x, uint8_t y, Hex565 *color) {
    fb->pixels[y][x] = convert_hex565(color);
}

void set_pixel_hex888(framebuffer *fb, uint8_t x, uint8_t y, Hex888 *color) {
    fb->pixels[y][x] = convert_hex888(color);
}


void clear(framebuffer *fb) {
    // Sets all values in the mapping to zero.
    // In other workds, clears the screen.
    memset(fb, 0, 128);
}

void sleepms(useconds_t time) {
    usleep(1000 * time);
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

    clear(fb);

    Hex565 red;
    red.rgb = 0xF800;
    set_pixel_hex565(fb, 0, 0, &red);

    Rgb green;
    green.r = 0; green.g = 255; green.b = 0;

    set_pixel_rgb(fb, 0, 1, &green);

    Hex888 blue;
    blue.rgb = 0x0000FF;
    set_pixel_hex888(fb, 0, 2, &blue);

    Hex888 yellow;
    yellow.rgb = 0xa0a000;
    set_pixel_hex888(fb, 0, 3, &yellow);
    
    /*uint8_t i, j;
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
    }*/

    sleepms(1000);
    clear(fb);

    return ret;
}

