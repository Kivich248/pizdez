#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filters.h"
#include "functions.h"

#include <math.h>

// Пиксель: RGB, каждый компонент — uint8_t (0..255)
typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Pixel;

// Изображение
typedef struct
{
	int width;   // количество столбцов (x)
	int height;  // количество строк (y)
	Pixel** pixels;  // pixels[y][x] — доступ к пикселю
	Pixel* data;     // указатель на единый блок памяти (для освобождения)
} Image;

typedef struct
{
	float** core;  // матрица ядра
	int size;        // размер ядра (обычно 3, 5, 7...)
	float divisor;   // делитель для нормализации
} Core;

Image* Negro(Image* img)
{
	int width = img->width;
    int height = img->height;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t r = img->pixels[y][x].r;
            uint8_t g = img->pixels[y][x].g;
            uint8_t b = img->pixels[y][x].b;
            img->pixels[y][x].r = 255 - r;
            img->pixels[y][x].g= 255 - g;
            img->pixels[y][x].b = 255 - b;
        }
    }
}

Image* Grey(Image* img)
{
    int width = img->width;
    int height = img->height;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t r = img->pixels[y][x].r;
            uint8_t g = img->pixels[y][x].g;
            uint8_t b = img->pixels[y][x].b;
            float x = 0,299 * r + 0,587 * g + 0,114 * b;
            img->pixels[y][x].r = round(x);
            img->pixels[y][x].g = round(x);
            img->pixels[y][x].b = round(x);
        }
    }
}

