#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>

#ifdef __linux__
#include <sys/mman.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#endif
#ifndef __linux__
struct input_event
{
    //struct timeval time;   // 记录了输入事件的触发事件
    //struct timeval tv_sec; // 记录秒钟  st_usec  //记录微妙
    unsigned short type;            // 记录输入事件的 类型
    unsigned short code;            // 记录输入事件的 事件编码
    int value;           // 记录输入事件的 值
};
#define ABS_MT_POSITION_X 0x35
#define ABS_MT_POSITION_Y 0x36
#define BTN_TOUCH 0x14a
#define ABS_X 0x00
#define ABS_Y 0x01
#define EV_SYN 0x00
#define EV_KEY 0x01
#define EV_ABS 0x03
#endif
struct input_event ev;
void input_close(int input_fd);
void ev_test(int input_fd,int *current_image);
int ev_open();
void screen_click();
void screen_slide_up(int *current_image);
void screen_slide_down(int *current_image);
void screen_slide_left(int *current_image);
void screen_slide_right(int *current_image);

#endif