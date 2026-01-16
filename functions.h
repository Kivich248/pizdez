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

typedef struct
{
	float** core;  // матрица ядра
	int size;        // размер ядра (обычно 3, 5, 7...)
	float divisor;   // делитель для нормализации
} Core;

Image* create_image(int width, int height);
void destroy_image(Image* img);
Image* read_bmp(const char* input_file);
int write_bmp(const char* output_file, Image* img);
Image apply_core(Image* img, Core* core);
Core* create_sharp_x_core();
Core* create_edge_x_core();
Core* create_gauss_x_core(float sigma);
int** create_matrix(n);
void sort_matrix(int **matrix, int n);

#endif //LABA3_FUNCTIONS_H