//
// Created by sanic on 13.01.2026.
//

#ifndef LABA3_FILTERS_H
#define LABA3_FILTERS_H

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

Image* Crop(Image* img, int width, int height);
Image* Grey(Image* img);
Image* Negro(Image* img);
Image* Sharp(Image* img);
Image* Edge(Image* img, float threshold);
Image* Median(Image* img, int window);
Image* Gaussian_Blur(Image* img, float sigma);
Image* Pixelate(Image* img, int block_size);
Image* Sepia(Image* img, float intensity);
#endif //LABA3_FILTERS_H