#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#ifdef __linux__
#include <sys/mman.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#endif

struct input_event ev;
void input_close(int input_fd);
void ev_test(int input_fd);
int ev_open();

#endif