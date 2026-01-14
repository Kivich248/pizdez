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
} Image;

typedef struct
{
	float** core;  // матрица ядра
	int size;        // размер ядра (обычно 3, 5, 7...)
	float divisor;   // делитель для нормализации
} Core;


Image Crop(Image* img, int width, int height)
{
	Image* img_return = create_image(width, height);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			img_return->pixels[y][x].r = img->pixels[y][x].r;
			img_return->pixels[y][x].g = img->pixels[y][x].g;
			img_return->pixels[y][x].b = img->pixels[y][x].b;
		}
	}
	return img_return;
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
			float x = 0.299 * r + 0.587 * g + 0.114 * b;
			img->pixels[y][x].r = round(x);
			img->pixels[y][x].g = round(x);
			img->pixels[y][x].b = round(x);
		}
	}
}


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


Image* Sharp(Image* img)
{
	Core* sharp_core = create_sharp_x_core();
	Image img_return = apply_core(img, sharp_core);
	return img_return;
}


Image* Edge(Image* img, float threshold)
{
	int height = img->height;
	int width = img->width;
	img = Grey(img);
	Core* edge_core = create_edge_x_core();
	Image img_return = apply_core(img, edge_core);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int gray = img_return.pixels[y][x].r;
			if (gray - threshold * 255 > 0)
			{
				img->pixels[y][x].r = 255;
				img->pixels[y][x].g = 255;
				img->pixels[y][x].b = 255;
			}
			if (gray - threshold * 255 < 0)
			{
				img->pixels[y][x].r = 0;
				img->pixels[y][x].g = 0;
				img->pixels[y][x].b = 0;
			}
		}
	}
}


