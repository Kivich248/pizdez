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
// -edge число, float от 0 до 1
// -med число, int положительные, тесты с 9 уже долгие, дальше на СВОй страх и риск
// -blur число, float полож, размер ядра считается как 2*[sigma*3] + 1, поэтому больше 10 не советую применять
// -pixelate число, int >= 2
// -sepia можно с и без числа, с числом float от 0 до 1
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf(".\\cmake-build-debug\\laba3.exe mountains.bmp res.bmp - путь до файла лабы + пример инпута и оутпута\n");
        printf("Функции с описанием:\n");
        printf("-crop int int, положительные. Отрезает кусок от верхнего левого угла изображения;\n");
        printf("-gs. Делает изображение серым через классическую формулу.\n");
        printf("-neg. Фильтр негатива.\n");
        printf("-sharp. Применение матрицы с 5 в центре и с -1 по сторонам от 5. Обостряет картинку.\n");
        printf("-edge float, число от 0 до 1. Выделяет границы через перепад цвета.\n");
        printf("-med int. Размывает каждый пиксель равномерно.\n");
        printf("-blur float. Размывает пиксель по динамическому ядру с помощью формулы.\n");
        printf("-pixelate int, число >= 2. Условно ухудшаем качество изображения, шакаля его.\n");
        printf("-sepia float, число от 0 до 1. Можно с числом или без. По умолчанию 1. Коричневит фото.\n");
        return 1;
    }

    const char *input_file = argv[1];               //оп оп завезли название файлов
    const char *output_file = argv[2];

    Image* img = read_bmp(input_file);              //оп оп прочитали и проработали ошибку
    if (img == NULL)
    {
        printf("Файл инпута не был прочитан, read_bmp\n");
        return 1;
    }

    //3 аргумента = отсутствие фильтров
    if (argc == 3)
    {
        int result = write_bmp(output_file, img);
        if (!result)
        {
            printf("Как....'\n");
            destroy_image(img);
            return 1;
        }
        destroy_image(img);
        return 0;
    }

	int width = img->width;                         //удобно
	int height = img->height;

	Image* img_got = create_image(width, height);   //пустое изображение для реза
    int i = 3;
    //в цикле проверяем разные условия совпадение с названием функций, если она требует число, требуем проверки существования след элемента и то, число ли оно
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
            if (threshold < 0.0f || threshold > 1.0f)
            {
                printf("Threshold в edge должен быть от 0.0 до 1.0\n");
                return 1;
            }
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
        printf("Введи правильную команду. Аргумент номер %i\n", i);
        return 1;
    }
    write_bmp(argv[2], img_got);
    destroy_image(img);
    return 0;
}