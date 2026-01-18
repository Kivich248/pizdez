#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filters.h"
#include "functions.h"
#include <string.h>
//.\cmake-build-debug\laba3.exe mountains.bmp res.bmp
// -crop число число, целые положительные
// -gs
// -neg
// -sharp
// -edge число, float любые, имеет смысл от 0 до 1, но можно любые
// -med число, int положительные, тесты с 9 уже долгие, дальше на СВОй страх и риск
// -blur число, float полож, размер ядра считается как 2*[sigma*3] + 1, поэтому больше 10 не советую применять
// -pixelate число
// -sepia можно с и без числа
int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("ты идиот?\n");

        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    Image* img = read_bmp(input_file);
    if (img == NULL)
    {
        printf("печально\n");
        return 1;
    }

    if (argc == 3)
    {
        int result = write_bmp(output_file, img);
        if (!result)
        {
            printf("грустно'\n", output_file);
            destroy_image(img);
            return 1;
        }
        destroy_image(img);
        return 0;
    }

	int width = img->width;
	int height = img->height;

	Image* img_got = create_image(width, height);
    int i = 3;
    while (i < argc)
    {
        if (strcmp(argv[i], "-crop") == 0 && argc > i+1 && is_valid_integer(argv[i+1]) && is_valid_integer(argv[i+2]))
        {
            int w = atoi(argv[i+1]);
            int h = atoi(argv[i+2]);
            img_got = Crop(img, w, h);
            i = i + 3;
            continue;
        }
        if (strcmp(argv[i], "-gs") == 0 )
        {
            img_got = Grey(img);
            i = i + 1;
            continue;
        }
        if (strcmp(argv[i], "-neg") == 0)
        {
            img_got = Negro(img);
            i = i + 1;
            continue;
        }
        if (strcmp(argv[i], "-sharp") == 0)
        {
            img_got = Sharp(img);
            i = i + 1;
            continue;
        }
        if (strcmp(argv[i], "-edge") == 0 && argc > i && is_valid_number(argv[i+1]))
        {
            float threshold = string_to_float(argv[i+1]);
            img_got = Edge(img, threshold);
            i = i + 2;
            continue;
        }
        if (strcmp(argv[i], "-med") == 0 && argc > i && is_valid_integer(argv[i+1]))
        {
            int window = atoi(argv[i+1]);
            img_got = Median(img, window);
            i = i + 2;
            continue;
        }
        if (strcmp(argv[i], "-blur") == 0 && argc > i && is_valid_number(argv[i+1]))
        {
            float sigma = string_to_float(argv[i+1]);
            img_got = Gaussian_Blur(img, sigma);
            i = i + 2;
            continue;
        }
        if (strcmp(argv[i], "-pixelate") == 0 && argc > i && is_valid_integer(argv[i+1]))
        {
            int block_size = atoi(argv[i+1]);
            if (block_size < 2)
            {
                printf("Размер блока для -pixelate должен быть >= 2\n");
                return 1;
            }
            img_got = Pixelate(img, block_size);
            i = i + 2;
            continue;
        }
        if (strcmp(argv[i], "-sepia") == 0)
        {
            float intensity = 1.0f; // Значение по умолчанию
            if (argc > i+1 && is_valid_number(argv[i+1]))
            {
                intensity = string_to_float(argv[i+1]);
                i = i + 2;
            }
            else
            {
                i = i + 1;
            }

            // Проверяем корректность интенсивности
            if (intensity < 0.0f || intensity > 1.0f)
            {
                printf("Интенсивность сепии должна быть от 0.0 до 1.0\n");
                return 1;
            }

            img_got = Sepia(img, intensity);
            continue;
        }
        printf("введи правильную команду\n");
        return 1;
    }
    write_bmp(argv[2], img_got);
    destroy_image(img);
    return 0;
}