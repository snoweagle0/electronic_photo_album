#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

char **scan_images(const char *dir_path, int *num_images)
{
    DIR *dir = opendir(dir_path);
    if (!dir) return NULL;

    char **files = NULL;
    *num_images = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".bmp")) {
            files = realloc(files, (*num_images + 1) * sizeof(char *));
            files[*num_images] = malloc(strlen(dir_path) + strlen(entry->d_name) + 2);
            sprintf(files[*num_images], "%s/%s", dir_path, entry->d_name);
            (*num_images)++;
        }
    }

    closedir(dir);
    return files;
}

void free_image_list(char **image_files, int num_images)
{
    for (int i = 0; i < num_images; i++) {
        free(image_files[i]);
    }
    free(image_files);
}

Image *image_load(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;

    Image *img = malloc(sizeof(Image));
    if (!img) {
        fclose(fp);
        return NULL;
    }

    unsigned char header[54];
    if (fread(header, sizeof(unsigned char), 54, fp) != 54) {
        free(img);
        fclose(fp);
        return NULL;
    }

    // 检查BMP签名
    if (header[0] != 'B' || header[1] != 'M') {
        free(img);
        fclose(fp);
        return NULL;
    }

    img->width = *(int*)&header[18];
    img->height = *(int*)&header[22];
    int bpp = *(short*)&header[28]; // 位深度

    // 只支持24位和32位
    if (bpp != 24 && bpp != 32) {
        printf("Unsupported bit depth: %d\n", bpp);
        free(img);
        fclose(fp);
        return NULL;
    }

    // 计算每行字节数（包含填充）
    int bytesPerLine = ((img->width * bpp + 31) / 32) * 4;
    int dataSize = bytesPerLine * img->height;
    unsigned char *rawData = malloc(dataSize);
    img->data = malloc(img->width * img->height * 3); // 统一转换为24位

    if (!rawData || !img->data) {
        free(rawData);
        free(img->data);
        free(img);
        fclose(fp);
        return NULL;
    }

    // 跳转到像素数据开始位置
    fseek(fp, *(int*)&header[10], SEEK_SET);
    if (fread(rawData, 1, dataSize, fp) != dataSize) {
        free(rawData);
        free(img->data);
        free(img);
        fclose(fp);
        return NULL;
    }

    // 转换像素数据
    int dst = 0;
    for (int y = 0; y < img->height; y++) {
        int srcRow = (img->height - 1 - y) * bytesPerLine;
        for (int x = 0; x < img->width; x++) {
            int src = srcRow + x * (bpp/8);
            img->data[dst++] = rawData[src];     // B
            img->data[dst++] = rawData[src + 1]; // G
            img->data[dst++] = rawData[src + 2]; // R
        }
    }

    free(rawData);
    fclose(fp);
    return img;
}

// 添加双线性插值缩放函数
Image *image_resize(Image *orig, int new_width, int new_height)
{
    if (!orig || new_width <= 0 || new_height <= 0) return NULL;

    Image *scaled = malloc(sizeof(Image));
    if (!scaled) return NULL;

    scaled->width = new_width;
    scaled->height = new_height;
    scaled->data = malloc(new_width * new_height * 3);
    if (!scaled->data) {
        free(scaled);
        return NULL;
    }

    float x_ratio = (float)orig->width / new_width;
    float y_ratio = (float)orig->height / new_height;

    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            float src_x = x * x_ratio;
            float src_y = y * y_ratio;
            int x1 = (int)src_x;
            int y1 = (int)src_y;
            int x2 = (x1 + 1 < orig->width) ? x1 + 1 : x1;
            int y2 = (y1 + 1 < orig->height) ? y1 + 1 : y1;

            float x_diff = src_x - x1;
            float y_diff = src_y - y1;

            for (int c = 0; c < 3; c++) {
                int index = (y * new_width + x) * 3 + c;
                int src_index11 = (y1 * orig->width + x1) * 3 + c;
                int src_index12 = (y1 * orig->width + x2) * 3 + c;
                int src_index21 = (y2 * orig->width + x1) * 3 + c;
                int src_index22 = (y2 * orig->width + x2) * 3 + c;

                float value = (1 - x_diff) * (1 - y_diff) * orig->data[src_index11] +
                            x_diff * (1 - y_diff) * orig->data[src_index12] +
                            (1 - x_diff) * y_diff * orig->data[src_index21] +
                            x_diff * y_diff * orig->data[src_index22];

                scaled->data[index] = (unsigned char)value;
            }
        }
    }

    return scaled;
}

void image_display(Image *img, int *plcd, int xres, int yres, int line_length)
{
    Image *display_img = img;
    Image *scaled_img = NULL;

    // 如果图片尺寸大于屏幕或小于屏幕，进行缩放
    if (img->width != xres || img->height != yres) {
        float scale_x = (float)xres / img->width;
        float scale_y = (float)yres / img->height;
        float scale = scale_x < scale_y ? scale_x : scale_y;

        int new_width = (int)(img->width * scale);
        int new_height = (int)(img->height * scale);

        scaled_img = image_resize(img, new_width, new_height);
        if (scaled_img) {
            display_img = scaled_img;
        }
    }

    // 计算居中显示的起始位置
    int start_x = (xres - display_img->width) / 2;
    int start_y = (yres - display_img->height) / 2;

    // 清空屏幕
    memset(plcd, 0, xres * yres * 4);

    // 显示图片
    for (int y = 0; y < display_img->height; y++) {
        for (int x = 0; x < display_img->width; x++) {
            int src_index = (y * display_img->width + x) * 3;
            unsigned char b = display_img->data[src_index];
            unsigned char g = display_img->data[src_index + 1];
            unsigned char r = display_img->data[src_index + 2];
            int color = (r << 16) | (g << 8) | b;
            plcd[(start_y + y) * (line_length / 4) + (start_x + x)] = color;
        }
    }

    // 释放缩放后的图片内存
    if (scaled_img) {
        image_free(scaled_img);
    }
}

void image_free(Image *img)
{
    if (img) {
        free(img->data);
        free(img);
    }
}
      