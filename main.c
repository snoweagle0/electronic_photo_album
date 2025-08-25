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
    int current_image = 0; // 当前图片索引

    int input_fd = ev_open();
    if (input_fd == -1)
    {
        return 0;
    }
    /*
    pthread_t tid;
    //int res= pthread_create(&tid,NULL,ev_test,input_fd,&current_image);

    */

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
    if (!image_files || num_images == 0)
    {
        fprintf(stderr, "No images found in picture directory\n");
        lcd_map_release(plcd, var.xres, var.yres);
        lcd_exit(lcd_fd);
        return -1;
    }

    int bx, by; // 触摸起始点
    int ex, ey; // 触摸中止点
    int dx, dy; // 变化
    int event_flag = 0;
    while (1)
    {
        if (read(input_fd, &ev, sizeof(ev)) > 0)
        {
            // printf("ev_type:%x ev_code:%x ev_var:%d\n", ev.type, ev.code,ev.value);
            // 判断输入做了什么操作
            if (ev.type == EV_KEY && ev.value == 0)
            {
                // 手指不在屏幕上，有可能是没有操作或者是操作结束的表示
                // 没有操作也可以看作操作结束，相当于进行了一个什么都不做的操作
                if (event_flag == 0)
                    continue;
                else
                {
                    event_flag = 0;
                    // 计算滑动距离
                    dx = ex - bx;
                    dy = ey - by;
                    // 判断是滑动还是点击，如果是点击，执行点击操作，然后continue
                    if (abs(dx) < 10 && abs(dy) < 10)
                    {
                        printf("Click at (%d, %d)\n", ex, ey);
                        screen_click();
                        continue;
                    }
                    // 判断滑动方向
                    if (abs(dx) > abs(dy))
                    {
                        if (dx > 0)
                        {
                            printf("Slide Right\n");
                            screen_slide_right(&current_image);
                        }
                        else
                        {
                            printf("Slide Left\n");
                            screen_slide_left(&current_image);
                        }
                    }
                    else
                    {
                        if (dy > 0)
                        {
                            printf("Slide Down\n");
                            screen_slide_down(&current_image);
                        }
                        else
                        {
                            printf("Slide Up\n");
                            screen_slide_up(&current_image);
                        }
                    }
                    current_image = (current_image + num_images) % num_images;
                    printf("Displaying %s\n", image_files[current_image]);
                    // 直接使用image_load和image_display函数
                    
                    Image *img = image_load(image_files[current_image]);
                    if (img)
                    {
                        image_display(img, plcd, var.xres, var.yres, fix.line_length);
                        image_free(img);
                    }
                }
            }
            else if (ev.type == EV_KEY && ev.value == 1)
            {
                // 当前手指正在屏幕上的某一区域
                event_flag = 1;
                bx = -1;
                by = -1;
                ex = -1;
                ey = -1;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_X)
            {
                if (bx == -1)
                    bx = ev.value;
                ex = ev.value;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_Y)
            {
                if (by == -1)
                    by = ev.value;
                ey = ev.value;
            }
        }
    }
    input_close(input_fd);

    /*
        while (1)
    {
        printf("Displaying %s\n", image_files[current_image]);

        // 直接使用image_load和image_display函数
        Image *img = image_load(image_files[current_image]);
        if (img) {
            image_display(img, plcd, var.xres, var.yres, fix.line_length);
            image_free(img);
        }

        //current_image = current_image % num_images;
        //sleep(3);
    }
    */

    lcd_map_release(plcd, var.xres, var.yres);
    lcd_exit(lcd_fd);
    free_image_list(image_files, num_images); // 释放图片列表内存

    return 0;
}
