#ifndef _IMAGE_H_
#define _IMAGE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

typedef struct {
    int width;
    int height;
    unsigned char *data;
} Image;

// 扫描图片目录
char **scan_images(const char *dir_path, int *num_images);

// 释放图片文件列表
void free_image_list(char **image_files, int num_images);

// 加载BMP图片
Image *image_load(const char *filename);

// 显示图片到LCD
void image_display(Image *img, int *plcd, int xres, int yres, int line_length);

// 释放图片资源
void image_free(Image *img);

// 图片缩放函数
Image *image_resize(Image *orig, int new_width, int new_height);

#endif
