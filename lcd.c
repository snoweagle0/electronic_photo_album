#include "lcd.h"

struct fb_var_screeninfo var;
struct fb_fix_screeninfo fix;

int lcd_init(void)
{
    int lcd_fd = open("/dev/fb0", O_RDWR);
    if (lcd_fd == -1)
    {
        perror("open lcd error");
        return -1;
    }

    if (ioctl(lcd_fd, FBIOGET_VSCREENINFO, &var) == -1)
    {
        perror("get var info error");
        return -1;
    }

    if (ioctl(lcd_fd, FBIOGET_FSCREENINFO, &fix) == -1)
    {
        perror("get fix info error");
        return -1;
    }

    return lcd_fd;
}

int *lcd_map_init(int lcd_fd, int xres, int yres)
{
    int *plcd = mmap(NULL, xres * yres * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if (plcd == MAP_FAILED)
    {
        perror("mmap error");
        return NULL;
    }
    return plcd;
}

void lcd_map_release(int *plcd, int xres, int yres)
{
    munmap(plcd, xres * yres * 4);
}

void lcd_exit(int lcd_fd)
{
    close(lcd_fd);
}
