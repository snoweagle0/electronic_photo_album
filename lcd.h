#ifndef _LCD_H_
#define _LCD_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/mman.h>
#include <linux/fb.h>


#endif


extern struct fb_var_screeninfo var;
extern struct fb_fix_screeninfo fix;

// LCD初始化
int lcd_init(void);

// LCD内存映射初始化
int *lcd_map_init(int lcd_fd, int xres, int yres);

// LCD内存映射释放
void lcd_map_release(int *plcd, int xres, int yres);

// LCD退出
void lcd_exit(int lcd_fd);

#endif
