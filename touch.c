#include "touch.h"

// extern struct input_event ev;


int ev_open()
{
    int input_fd = open("/dev/input/event6", O_RDONLY);
    if (input_fd == -1)
    {
        perror("oen event_file failed");
        return -1;
    }
    printf("input_fd open \n");
    fflush(stdout);
    return input_fd;
}

void ev_test(int input_fd)
{
    int bx, by; // 触摸起始点
    int ex, ey; // 触摸中止点
    int dx, dy; // 变化
    int event_flag=0;
    while (1)
    {
        if (read(input_fd, &ev, sizeof(ev)) > 0)
        {
            //printf("ev_type:%x ev_code:%x ev_var:%d\n", ev.type, ev.code,ev.value);
            //判断输入做了什么操作
            if (ev.type == EV_KEY && ev.value == 0)
            {
                // 手指不在屏幕上，有可能是没有操作或者是操作结束的表示
                // 没有操作也可以看作操作结束，相当于进行了一个什么都不做的操作
                if(event_flag==0)
                continue;
                else {
                    event_flag=0;
                    // 计算滑动距离
                    dx = ex - bx;
                    dy = ey - by;
                    //判断是滑动还是点击，如果是点击，执行点击操作，然后continue
                    if (abs(dx) < 10 && abs(dy) < 10) {
                        printf("Click at (%d, %d)\n", ex, ey);
                        screen_click();
                        continue;
                    }
                    // 判断滑动方向
                    if (abs(dx) > abs(dy)) {
                        if (dx > 0) {
                            printf("Slide Right\n");
                            screen_slide_right();
                        } else {
                            printf("Slide Left\n");
                            screen_slide_left();
                        }
                    } else {
                        if (dy > 0) {
                            printf("Slide Down\n");
                            screen_slide_down();
                        } else {
                            printf("Slide Up\n");
                            screen_slide_up();
                        }
                    }
                }
            }
            else if (ev.type == EV_KEY && ev.value == 1)
            {
                // 当前手指正在屏幕上的某一区域
                event_flag=1;
                bx=-1;
                by=-1;
                ex=-1;
                ey=-1;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_X)
            {
                if(bx==-1)
                bx=ev.value;
                ex = ev.value;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_Y)
            {
                if(by==-1)
                by=ev.value;
                ey = ev.value;
            }
        }
    }
    input_close(input_fd);
}

void input_close(int input_fd)
{
    close(input_fd);
}

void screen_click()
{
    return;
}

void screen_slide_up()
{
    printf("slide up\n");
    return;
}
void screen_slide_down()
{
    printf("slide down\n");
    return;
}
void screen_slide_left()
{
    printf("slide left\n");
    return;
}
void screen_slide_right()
{
    printf("slide right\n");
    return;
}
