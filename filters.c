#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filters.h"
#include "functions.h"
#include <tgmath.h>

Image* Crop(Image* img, int width, int height)
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
			float gray_value = 0.299f * r + 0.587f * g + 0.114f * b; 
            uint8_t gray = (uint8_t)roundf(gray_value);
            img->pixels[y][x].r = gray;
            img->pixels[y][x].g = gray;
            img->pixels[y][x].b = gray;
		}
	}
	return img;
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
	return img;
}


Image* Sharp(Image* img)
{
	Core* sharp_core = create_sharp_x_core();
	Image* img_return = apply_core(img, sharp_core);
	return img_return;
}


Image* Edge(Image* img, float threshold)
{
	int height = img->height;
	int width = img->width;
	img = Grey(img);
	Core* edge_core = create_edge_x_core();
	Image* img_return = apply_core(img, edge_core);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int gray = img_return->pixels[y][x].r;
			if (gray - threshold * 255 > 0)
			{
				img_return->pixels[y][x].r = 255;
				img_return->pixels[y][x].g = 255;
				img_return->pixels[y][x].b = 255;
			}
			if (gray - threshold * 255 < 0)
			{
				img_return->pixels[y][x].r = 0;
				img_return->pixels[y][x].g = 0;
				img_return->pixels[y][x].b = 0;
			}
		}
	}
	return img_return;
}


Image* Median(Image* img, int window) {				//медианный фильтр
	if (window % 2 == 0 || window <= 0) return NULL;
	if (img == NULL) return NULL;
	int width = img->width;
	int height = img->height;
	int sdvg = window / 2;
	if (window > width || window > height)
	{
		return NULL;
	}


	Image* result = create_image(width, height);

	int **mat_r = create_matrix(window);		//создаем вспомогательные матрицы для цветов
	int **mat_g = create_matrix(window);
	int **mat_b = create_matrix(window);


	for (int y = 0; y < height; y++)			//проходимся по изображению
	{
		for (int x = 0; x < width; x++)
		{
			for (int i = 0; i < window; i++ ) {		//накладываем матрицу на изображение
				for (int j = 0; j < window; j++ ) {
				int x_m = x + j - sdvg;
				int y_m = y + i - sdvg;


				if (x_m < 0) {						//обработка границ
					x_m  = -x_m - 1;	
				}
				if (y_m < 0) {
					y_m  = -y_m - 1;	
				}
				if (x_m >= width) {
					x_m = 2*width - x_m;		
				}
				if (y_m >= height) {
					y_m = 2*height - y_m - 1;		
				}
				Pixel pixel = img->pixels[y_m][x_m]; //переносим значения в матрицу
				mat_r[i][j] = pixel.r;
				mat_g[i][j] = pixel.g;
				mat_b[i][j] = pixel.b;
			}
			}

			sort_matrix(mat_r, window);				//ищем медианный элемент
			sort_matrix(mat_g, window);
			sort_matrix(mat_b, window);

			uint8_t r = mat_r[sdvg][sdvg];
			uint8_t g = mat_g[sdvg][sdvg];
			uint8_t b = mat_b[sdvg][sdvg];
			


			result->pixels[y][x].r = r;
			result->pixels[y][x].g = g;
			result->pixels[y][x].b = b;
			

		}
	}
	for (int i = 0; i < window; i++) {
    free(mat_r[i]);
    free(mat_g[i]);
    free(mat_b[i]);
}
free(mat_r);
free(mat_g);
free(mat_b);
	return result;
}


Image* Gaussian_Blur(Image* img, float sigma)
{
	Core* gauss_core = create_gauss_x_core(sigma);
	Image* img_return = apply_core(img, gauss_core);
	return img_return;
}





// Фильтр точечной мозаики
Image* Pixelate(Image* img, int block_size)
{
    if (img == NULL || block_size <= 1) return NULL;
    
    int width = img->width;
    int height = img->height;
    
    Image* result = create_image(width, height);
    if (result == NULL) return NULL;
    
    // Проходим по блокам размером block_size x block_size
    for (int block_y = 0; block_y < height; block_y += block_size)
    {
        for (int block_x = 0; block_x < width; block_x += block_size)
        {
            // Определяем границы текущего блока
            int block_end_y = (block_y + block_size < height) ? 
                              block_y + block_size : height;
            int block_end_x = (block_x + block_size < width) ? 
                              block_x + block_size : width;
            
            // Считаем средний цвет в блоке
            unsigned long long sum_r = 0, sum_g = 0, sum_b = 0;
            int pixel_count = 0;
            
            for (int y = block_y; y < block_end_y; y++)
            {
                for (int x = block_x; x < block_end_x; x++)
                {
                    sum_r += img->pixels[y][x].r;
                    sum_g += img->pixels[y][x].g;
                    sum_b += img->pixels[y][x].b;
                    pixel_count++;
                }
            }
            
            // Вычисляем средние значения
            uint8_t avg_r = (uint8_t)(sum_r / pixel_count);
            uint8_t avg_g = (uint8_t)(sum_g / pixel_count);
            uint8_t avg_b = (uint8_t)(sum_b / pixel_count);
            
            // Заполняем блок средним цветом
            for (int y = block_y; y < block_end_y; y++)
            {
                for (int x = block_x; x < block_end_x; x++)
                {
                    result->pixels[y][x].r = avg_r;
                    result->pixels[y][x].g = avg_g;
                    result->pixels[y][x].b = avg_b;
                }
            }
        }
    }
    
    return result;
}

// Фильтр сепии с регулируемой интенсивностью
Image* Sepia(Image* img, float intensity)
{
    if (img == NULL) return NULL;
    
    // Ограничиваем интенсивность от 0.0 до 1.0
    if (intensity < 0.0f) intensity = 0.0f;
    if (intensity > 1.0f) intensity = 1.0f;
    
    int width = img->width;
    int height = img->height;
    
    // Создаем копию изображения для результата
    Image* result = create_image(width, height);
    if (result == NULL) return NULL;
    
    // Стандартные коэффициенты для сепии
    const float sepia_r = 0.393f;
    const float sepia_g = 0.769f;
    const float sepia_b = 0.189f;
    
    const float sepia_r2 = 0.349f;
    const float sepia_g2 = 0.686f;
    const float sepia_b2 = 0.168f;
    
    const float sepia_r3 = 0.272f;
    const float sepia_g3 = 0.534f;
    const float sepia_b3 = 0.131f;
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t r = img->pixels[y][x].r;
            uint8_t g = img->pixels[y][x].g;
            uint8_t b = img->pixels[y][x].b;
            
            // Вычисляем оттенок сепии
            float new_r = (r * sepia_r + g * sepia_g + b * sepia_b);
            float new_g = (r * sepia_r2 + g * sepia_g2 + b * sepia_b2);
            float new_b = (r * sepia_r3 + g * sepia_g3 + b * sepia_b3);
            
            // Интерполируем между оригиналом и сепией в зависимости от интенсивности
            new_r = r * (1.0f - intensity) + new_r * intensity;
            new_g = g * (1.0f - intensity) + new_g * intensity;
            new_b = b * (1.0f - intensity) + new_b * intensity;
            
            // Ограничиваем значения и предотвращаем переполнение
            if (new_r > 255.0f) new_r = 255.0f;
            if (new_g > 255.0f) new_g = 255.0f;
            if (new_b > 255.0f) new_b = 255.0f;
            
            result->pixels[y][x].r = (uint8_t)new_r;
            result->pixels[y][x].g = (uint8_t)new_g;
            result->pixels[y][x].b = (uint8_t)new_b;
        }
    }
    
    return result;
}