#include "touch.h"

//extern struct input_event ev;

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
    while (1)
    {
        if (read(input_fd, &ev, sizeof(ev)) > 0)
        {
            printf("ev_type:%x ev_code:%x ev_var:%d\n", ev.type, ev.code,
                   ev.value);
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
    
}

void screen_slide_up()
{

}
void screen_slide_down()
{

}
void screen_slide_left()
{

}
void screen_slide_right()
{

}
