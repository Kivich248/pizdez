#ifndef LABA3_FUNCTIONS_H
#define LABA3_FUNCTIONS_H

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
	Pixel* data;     // указатель на единый блок памяти (для освобождения)
} Image;

Image* create_image(int width, int height);
void destroy_image(Image* img);
Image* read_bmp(const char* input_file);
int write_bmp(const char* output_file, Image* img);

#endif //LABA3_FUNCTIONS_H