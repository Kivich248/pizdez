#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filters.h"
#include "functions.h"

// Пиксель: RGB, каждый компонент — uint8_t (0..255)
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Pixel;

// Изображение
typedef struct {
	int width;   // количество столбцов (x)
	int height;  // количество строк (y)
	Pixel** pixels;  // pixels[y][x] — доступ к пикселю
} Image;


int main(int argc, char *argv[])
{
	const char *nazv = argv[0];
	const char *input_file = argv[1];
	const char *output_file = argv[2];


	Image* img = read_bmp(input_file);


}