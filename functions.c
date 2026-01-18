#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filters.h"
#include "functions.h"
#include <ctype.h>
#include <stdbool.h>
#include <tgmath.h>
#include <math.h>

//создание картинки пустой
Image* create_image(int width, int height)
{
	if (width <= 0 || height <= 0)
	{
		return NULL;
	}

	Image* img = malloc(sizeof(Image));
	if (img == NULL)
	{
		return NULL;
	}

	img->width = width;
	img->height = height;

	// Выделяем массив указателей на строки
	img->pixels = malloc(height * sizeof(Pixel*));
	if (img->pixels == NULL)
	{
		free(img);
		return NULL;
	}

	// Выделяем каждую строку отдельно
	for (int y = 0; y < height; y++)
	{
		img->pixels[y] = malloc(width * sizeof(Pixel));
		if (img->pixels[y] == NULL)
		{
			// Освобождаем уже выделенные строки
			for (int i = 0; i < y; i++)
			{
				free(img->pixels[i]);
			}
			free(img->pixels);
			free(img);
			return NULL;
		}
	}
	return img;
}

//вау проработано освобождение памяти(маша настояла)
void destroy_image(Image* img)
{
	if (img != NULL)
	{
		if (img->pixels != NULL)
		{
			for (int y = 0; y < img->height; y++)
			{
				free(img->pixels[y]);  // освобождаем каждую строку
			}
			free(img->pixels);        // освобождаем массив указателей
		}
		free(img);                    // освобождаем саму структуру
	}
}

//учимся читать заголовочные данные и саму картинку with god help
Image* read_bmp(const char* input_file)
{
    if (input_file == NULL)
    {
        return NULL;
    }

    FILE* file = fopen(input_file, "rb");
    if (file == NULL)
    {
        return NULL;
    }

    uint8_t file_header[14];
    size_t bytes_read = fread(file_header, 1, 14, file);
    if (bytes_read != 14)
    {
        fclose(file);
        return NULL;
    }

    if (file_header[0] != 'B' || file_header[1] != 'M')
    {
        fclose(file);
        return NULL;
    }


    uint32_t bfOffBits = *(uint32_t*)(file_header + 10);

    uint8_t dib_header[40];
    bytes_read = fread(dib_header, 1, 40, file);
    if (bytes_read != 40)
    {
        fclose(file);
        return NULL;
    }

    uint32_t biSize = *(uint32_t*)(dib_header + 0);
    if (biSize != 40) //проверка на формат
    {
        fclose(file);
        return NULL;
    }

    int32_t biWidth = *(int32_t*)(dib_header + 4);
    int32_t biHeight = *(int32_t*)(dib_header + 8);
    uint16_t biPlanes = *(uint16_t*)(dib_header + 12);
    uint16_t biBitCount = *(uint16_t*)(dib_header + 14);
    uint32_t biCompression = *(uint32_t*)(dib_header + 16);

    if (biPlanes != 1)
    {
        fclose(file);
        return NULL;
    }
    if (biBitCount != 24)
    {
        fclose(file);
        return NULL;
    }
    if (biCompression != 0)
    {
        fclose(file);
        return NULL;
    }
    if (biHeight <= 0 || biHeight > 10000)
    {
        fclose(file);
        return NULL;
    }
	if (biWidth <= 0 || biWidth > 10000)
	{
		fclose(file);
		return NULL;
	}
    // Проверки на адекватность

    int width = (int)biWidth;
    int height = (int)biHeight;

    Image* img = create_image(width, height);
    if (img == NULL)
    {
        fclose(file);
        return NULL;
    }

    if (fseek(file, bfOffBits, SEEK_SET) != 0)
    {
        destroy_image(img);
        fclose(file);
        return NULL;
    }

    int row_size = (width * 3 + 3) / 4 * 4;
    uint8_t* row_buffer = malloc(row_size);
    if (row_buffer == NULL)
    {
        destroy_image(img);
        fclose(file);
        return NULL;
    }

    for (int y_file = 0; y_file < height; y_file++)
    {
        bytes_read = fread(row_buffer, 1, row_size, file);
        if (bytes_read != (size_t)row_size)
        {
            free(row_buffer);
            destroy_image(img);
            fclose(file);
            return NULL;
        }

        int y_internal = height - 1 - y_file;
        for (int x = 0; x < width; x++)
        {
            img->pixels[y_internal][x].b = row_buffer[x * 3 + 0];
            img->pixels[y_internal][x].g = row_buffer[x * 3 + 1];
            img->pixels[y_internal][x].r = row_buffer[x * 3 + 2];
        }
    }

    free(row_buffer);
    fclose(file);
    return img;
}

//научились писать
int write_bmp(const char* output_file, Image* img)
{
    if (output_file == NULL || img == NULL)
    {
        return 0;
    }
    if (img->width <= 0 || img->height <= 0)
    {
        return 0;
    }
    if (img->pixels == NULL)
    {
        return 0;
    }

    FILE* file = fopen(output_file, "wb");
    if (file == NULL)
    {
        return 0;
    }

    // === 1. Вычисляем параметры ===
    int width = img->width;
    int height = img->height;

    // Размер одной строки в байтах с выравниванием до 4
    int row_size = (width * 3 + 3) / 4 * 4;
    // Размер данных пикселей (все строки)
    size_t size_image = (size_t)row_size * (size_t)height;
    // Общий размер файла: заголовки (54 байта) + данные
    size_t file_size = 54 + size_image;

    // === 2. File Header (14 байт) ===
    uint8_t file_header[14] = {
        'B', 'M',                           // bfType
        0, 0, 0, 0,                         // bfSize (заполним позже)
        0, 0,                               // bfReserved1
        0, 0,                               // bfReserved2
        54, 0, 0, 0                         // bfOffBits = 54
    };
    // Записываем bfSize (байты 2-5)
    file_header[2] = (uint8_t)(file_size & 0xFF);
    file_header[3] = (uint8_t)((file_size >> 8) & 0xFF);
    file_header[4] = (uint8_t)((file_size >> 16) & 0xFF);
    file_header[5] = (uint8_t)((file_size >> 24) & 0xFF);

    // === 3. DIB Header (40 байт) ===
    uint8_t dib_header[40] = {
        40, 0, 0, 0,                        // biSize = 40
        0, 0, 0, 0,                         // biWidth (заполним)
        0, 0, 0, 0,                         // biHeight (заполним)
        1, 0,                               // biPlanes = 1
        24, 0,                              // biBitCount = 24
        0, 0, 0, 0,                         // biCompression = 0
        0, 0, 0, 0,                         // biSizeImage (можно 0)
        0, 0, 0, 0,                         // biXPelsPerMeter
        0, 0, 0, 0,                         // biYPelsPerMeter
        0, 0, 0, 0,                         // biClrUsed
        0, 0, 0, 0                          // biClrImportant
    };
    // Заполняем biWidth (байты 4-7)
    dib_header[4] = (uint8_t)(width & 0xFF);
    dib_header[5] = (uint8_t)((width >> 8) & 0xFF);
    dib_header[6] = (uint8_t)((width >> 16) & 0xFF);
    dib_header[7] = (uint8_t)((width >> 24) & 0xFF);
    // Заполняем biHeight (байты 8-11) - положительное = bottom-up
    dib_header[8] = (uint8_t)(height & 0xFF);
    dib_header[9] = (uint8_t)((height >> 8) & 0xFF);
    dib_header[10] = (uint8_t)((height >> 16) & 0xFF);
    dib_header[11] = (uint8_t)((height >> 24) & 0xFF);

    // === 4. Записываем заголовки ===
    if (fwrite(file_header, 1, 14, file) != 14)
    {
        fclose(file);
        return 0;
    }
    if (fwrite(dib_header, 1, 40, file) != 40)
    {
        fclose(file);
        return 0;
    }

    // === 5. Записываем пиксели (снизу вверх и BGR вместо RGB) ===
    uint8_t* row_buffer = malloc(row_size);
    if (row_buffer == NULL)
    {
        fclose(file);
        return 0;
    }

    for (int y_file = 0; y_file < height; y_file++)
    {
        // y_internal: в BMP первая записанная строка = нижняя
        int y_internal = height - 1 - y_file;

        // Заполняем буфер текущей строкой (конвертируем RGB → BGR)
        for (int x = 0; x < width; x++)
        {
            row_buffer[x * 3 + 0] = img->pixels[y_internal][x].b; // B
            row_buffer[x * 3 + 1] = img->pixels[y_internal][x].g; // G
            row_buffer[x * 3 + 2] = img->pixels[y_internal][x].r; // R
        }
        // Добавляем padding (нули) до row_size
        for (int i = width * 3; i < row_size; i++)
        {
            row_buffer[i] = 0;
        }

        // Записываем строку
        if (fwrite(row_buffer, 1, row_size, file) != (size_t)row_size)
        {
            free(row_buffer);
            fclose(file);
            return 0;
        }
    }

    free(row_buffer);
    fclose(file);
    return 1;
}

//применяем ядро размера n на картинку
Image* apply_core(Image* img, Core* core)
{
	int width = img->width;
	int height = img->height;
	int ksize = core->size;
	int sdvg = ksize / 2;

	if (img == NULL || core == NULL)
	{
		return NULL;
	}

	// Проверяем, что размер ядра нечетный
	if (core->size % 2 == 0)
	{
		return NULL;
	}

	// Проверяем, что ядро не больше изображения
	if (core->size > width || core->size > height)
	{
		return NULL;
	}

	Image* result = create_image(width, height);
	if (result == NULL)
	{
		return NULL;
	}

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f;
			for (int y_с = 0; y_с < ksize; y_с++)
			{
				for (int x_с = 0; x_с < ksize; x_с++)
				{
					int y_mat = y + y_с - sdvg;
					int x_mat = x + x_с - sdvg;

					if (y_mat < 0) y_mat = 0;
					if (y_mat >= height) y_mat = height - 1;
					if (x_mat < 0) x_mat = 0;
					if (x_mat >= width) x_mat = width - 1;

					Pixel pixel = img->pixels[y_mat][x_mat];

					float weight = core->core[y_с][x_с];

					r_sum = r_sum + pixel.r * weight;
					g_sum = g_sum + pixel.g * weight;
					b_sum = b_sum + pixel.b * weight;
				}
			}

			if (core->divisor != 0)
			{
				r_sum = r_sum / core->divisor;
				g_sum = g_sum / core->divisor;
				b_sum = b_sum / core->divisor;
			}

			uint8_t r = (r_sum > 255) ? 255 : ((r_sum < 0) ? 0 : (uint8_t)r_sum);
			uint8_t g = (g_sum > 255) ? 255 : ((g_sum < 0) ? 0 : (uint8_t)g_sum);
			uint8_t b = (b_sum > 255) ? 255 : ((b_sum < 0) ? 0 : (uint8_t)b_sum);

			result->pixels[y][x].r = r;
			result->pixels[y][x].g = g;
			result->pixels[y][x].b = b;
		}
	}
	return result;
}

//создание sharp ядра
Core* create_sharp_x_core()
{
	Core* core = malloc(sizeof(Core));
	if (core == NULL) return NULL;

	core->size = 3;
	core->divisor = 1.0f;

	// Создаем матрицу ядра
	core->core = malloc(core->size * sizeof(float*));
	if (core->core == NULL) {
		free(core);
		return NULL;
	}

	for (int i = 0; i < core->size; i++) {
		core->core[i] = malloc(core->size * sizeof(float));
		if (core->core[i] == NULL) {
			for (int j = 0; j < i; j++) free(core->core[j]);
			free(core->core);
			free(core);
			return NULL;
		}
	}

	// Заполняем значениями оператора Собеля по X
	float sobel_x[3][3] = {
		{0.0f, -1.0f, 0.0f},
		{-1.0f, 5.0f, -1.0f},
		{0.0f, -1.0f, 0.0f}
	};

	for (int i = 0; i < core->size; i++) {
		for (int j = 0; j < core->size; j++) {
			core->core[i][j] = sobel_x[i][j];
		}
	}

	return core;
}

//создание edge ядра
Core* create_edge_x_core()
{
    Core* kernel = malloc(sizeof(Core));
    if (kernel == NULL) return NULL;
    
    kernel->size = 3;
    kernel->divisor = 0.0f;
    
    // Создаем матрицу ядра
    kernel->core = malloc(kernel->size * sizeof(float*));
    if (kernel->core == NULL) {
        free(kernel);
        return NULL;
    }
    
    for (int i = 0; i < kernel->size; i++) {
        kernel->core[i] = malloc(kernel->size * sizeof(float));
        if (kernel->core[i] == NULL) {
            for (int j = 0; j < i; j++) free(kernel->core[j]);
            free(kernel->core);
            free(kernel);
            return NULL;
        }
    }
    
    
    float edge_matrix[3][3] = {
        { 0.0f, -1.0f,  0.0f},
        {-1.0f,  4.0f, -1.0f},
        { 0.0f, -1.0f,  0.0f}
    };
    
    for (int i = 0; i < kernel->size; i++) {
        for (int j = 0; j < kernel->size; j++) {
            kernel->core[i][j] = edge_matrix[i][j];
        }
    }
    
    return kernel;
}

//создание gauss ядра
Core* create_gauss_x_core(float sigma)
{
    Core* kernel = malloc(sizeof(Core));
    if (kernel == NULL) return NULL;

	int p = (int)ceil(sigma);

    kernel->size = 2 * p + 1;
    kernel->divisor = 0.0f;

    
    kernel->core = malloc(kernel->size * sizeof(float*));
    if (kernel->core == NULL) {
        free(kernel);
        return NULL;
    }

    for (int i = 0; i < kernel->size; i++) {
        kernel->core[i] = malloc(kernel->size * sizeof(float));
        if (kernel->core[i] == NULL) {
            for (int j = 0; j < i; j++) free(kernel->core[j]);
            free(kernel->core);
            free(kernel);
            return NULL;
        }
    }

    float sum = 0.0f;
    int half_size = kernel->size / 2;  // 3 для размера 7

    // Заполняем значениями функции Гаусса
    for (int i = -half_size; i <= half_size; i++) {
        for (int j = -half_size; j <= half_size; j++) {
            float value = expf(-(i*i + j*j) / (2.0f * sigma * sigma));
            kernel->core[i + half_size][j + half_size] = value;
            sum += value;
        }
    }

    // Нормализуем
    for (int i = 0; i < kernel->size; i++) {
        for (int j = 0; j < kernel->size; j++) {
            kernel->core[i][j] /= sum;
        }
    }

    return kernel;
}

//эмммм я хз это писала маша но она уже спит и я уже хочу спать а не ебашить эти комменты
int** create_matrix(int n) {
    int **matrix = (int**)malloc(n * sizeof(int*));
    if (matrix == NULL) {
        printf("Ошибка выделения памяти!\n");
        return NULL;
    }
    
    for (int i = 0; i < n; i++) {
        matrix[i] = (int*)malloc(n * sizeof(int));
        if (matrix[i] == NULL) {
            printf("Ошибка выделения памяти для строки %d!\n", i);
            // Освобождаем уже выделенную память
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }
    
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = 0;
        }
    }
    return matrix;
}


//аналогично
void sort_matrix(int **matrix, int n) {
    if (matrix == NULL || n <= 0) return;
    
    int total = n * n;
    
    // Шаг 1: Копируем матрицу в линейный массив
    int *linear = (int*)malloc(total * sizeof(int));
    if (linear == NULL) return;
    
    int index = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            linear[index++] = matrix[i][j];
        }
    }
    
    // Шаг 2: Сортируем линейный массив
    for (int i = 0; i < total - 1; i++) {
        for (int j = 0; j < total - 1 - i; j++) {
            if (linear[j] > linear[j + 1]) {
                int temp = linear[j];
                linear[j] = linear[j + 1];
                linear[j + 1] = temp;
            }
        }
    }
    
    // Шаг 3: Копируем отсортированные значения обратно в матрицу
    index = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = linear[index++];
        }
    }
    
    free(linear);
    
}

//число ли(и флоат и интежер)
bool is_valid_number(const char *str)
{
	if (!str || *str == '\0')
	{
		return false; // Пустая строка не является числом
	}

	int i = 0;
	bool has_decimal_point = false;

	// Обработка знака
	if (str[i] == '-' || str[i] == '+') i++;

	// Если после знака сразу точка — строка всё равно может быть валидной ("+.5")
	if (str[i] == '.')
	{
		has_decimal_point = true;
		i++;
	}

	bool has_digits = false;

	while (str[i] != '\0')
	{
		if (isdigit(str[i]))
		{
			has_digits = true;
		} else if (str[i] == '.')
		{
			if (has_decimal_point)
			{
				return false; // Вторая точка недопустима
			}
			has_decimal_point = true;
		} else
		{
			return false; // Недопустимый символ
		}
		i++;
	}
	return has_digits; // Должна быть хотя бы одна цифра
}


//строка в флоат
float string_to_float(const char *str)
{
	if (!str || !is_valid_number(str)) { // Предполагается, что is_valid_number объявлена выше
		return 0.0f; // Возвращаем 0.0f, если строка не валидна
	}

	int i = 0;
	float result = 0.0f;
	float fraction = 0.1f;
	bool negative = false;

	// Обработка знака
	if (str[i] == '-' || str[i] == '+') {
		if (str[i] == '-') {
			negative = true;
		}
		i++;
	}

	// Обработка целой части
	while (str[i] != '\0' && isdigit(str[i])) {
		result = result * 10.0f + (float)(str[i] - '0');
		i++;
	}

	// Если есть точка — обрабатываем дробную часть
	if (str[i] == '.') {
		i++; // Переходим к первому символу после точки
		while (str[i] != '\0' && isdigit(str[i])) {
			result += (float)(str[i] - '0') * fraction;
			fraction *= 0.1f;
			i++;
		}
	}

	return negative ? -result : result;
}


//инт ли
int is_valid_integer(const char *str) {         //реально интеджер ввели?
    if (str == NULL || *str == '\0') {
        return 0;
    }
    
    char *endptr;
    long val = strtol(str, &endptr, 10);
    
    // Проверяем, что преобразовано полностью и нет дробной части
    if (*endptr != '\0' || endptr == str) {
        return 0;
    }
    
    // Проверяем диапазон для int
    if (val < INT_MIN || val > INT_MAX) {
        return 0;
    }
    
    // Дополнительная проверка на точку/запятую
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.' || str[i] == ',') {
            return 0;
        }
    }
    
    return 1;
}