#include "lcd.h"
#include "image.h"
#include "touch.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

int main()
{
    int input_fd=ev_open();
    if(input_fd==-1)
    {
        return 0;
    }
    pthread_t tid;
    int res= pthread_create(&tid,NULL,ev_test,input_fd);
    int lcd_fd = lcd_init();
    if (lcd_fd == -1)
    {
        return -1;
    }

    int *plcd = lcd_map_init(lcd_fd, var.xres, var.yres);
    if (plcd == NULL)
    {
        lcd_exit(lcd_fd);
        return -1;
    }

    // 动态扫描图片目录
    int num_images = 0;
    char **image_files = scan_images("picture", &num_images);
    if (!image_files || num_images == 0) {
        fprintf(stderr, "No images found in picture directory\n");
        lcd_map_release(plcd, var.xres, var.yres);
        lcd_exit(lcd_fd);
        return -1;
    }
    int current_image = 0;

    while (1)
    {
        printf("Displaying %s\n", image_files[current_image]);
        
        // 直接使用image_load和image_display函数
        Image *img = image_load(image_files[current_image]);
        if (img) {
            image_display(img, plcd, var.xres, var.yres, fix.line_length);
            image_free(img);
        }
        
        current_image = (current_image + 1) % num_images;
        sleep(3);
    }

    lcd_map_release(plcd, var.xres, var.yres);
    lcd_exit(lcd_fd);
    free_image_list(image_files, num_images); // 释放图片列表内存

    return 0;
}
