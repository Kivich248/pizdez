#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPHeader;
#pragma pack(pop)

void create_gradient_bmp(const char* filename, int width, int height) {
    FILE* file = fopen(filename, "wb");
    if (!file) return;
    
    int row_size = (width * 3 + 3) / 4 * 4;
    int image_size = row_size * height;
    
    BMPHeader header = {
        .bfType = 0x4D42, // 'BM'
        .bfSize = sizeof(BMPHeader) + image_size,
        .bfReserved1 = 0,
        .bfReserved2 = 0,
        .bfOffBits = sizeof(BMPHeader),
        .biSize = 40,
        .biWidth = width,
        .biHeight = height,
        .biPlanes = 1,
        .biBitCount = 24,
        .biCompression = 0,
        .biSizeImage = image_size,
        .biXPelsPerMeter = 2835,
        .biYPelsPerMeter = 2835,
        .biClrUsed = 0,
        .biClrImportant = 0
    };
    
    fwrite(&header, sizeof(BMPHeader), 1, file);
    
    uint8_t* row = malloc(row_size);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Горизонтальный градиент RGB
            uint8_t r = (255 * x) / width;
            uint8_t g = (255 * y) / height;
            uint8_t b = 128;
            
            row[x * 3 + 0] = b;     // B
            row[x * 3 + 1] = g;     // G
            row[x * 3 + 2] = r;     // R
        }
        // Заполнение до выравнивания
        for (int i = width * 3; i < row_size; i++) {
            row[i] = 0;
        }
        fwrite(row, 1, row_size, file);
    }
    
    free(row);
    fclose(file);
    printf("Created: %s (%dx%d)\n", filename, width, height);
}

void create_checkerboard_bmp(const char* filename, int width, int height, int cell_size) {
    FILE* file = fopen(filename, "wb");
    if (!file) return;
    
    int row_size = (width * 3 + 3) / 4 * 4;
    int image_size = row_size * height;
    
    BMPHeader header = {
        .bfType = 0x4D42,
        .bfSize = sizeof(BMPHeader) + image_size,
        .bfOffBits = sizeof(BMPHeader),
        .biSize = 40,
        .biWidth = width,
        .biHeight = height,
        .biPlanes = 1,
        .biBitCount = 24,
        .biCompression = 0,
        .biSizeImage = image_size,
        .biXPelsPerMeter = 2835,
        .biYPelsPerMeter = 2835,
        .biClrUsed = 0,
        .biClrImportant = 0
    };
    
    fwrite(&header, sizeof(BMPHeader), 1, file);
    
    uint8_t* row = malloc(row_size);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int cell_x = x / cell_size;
            int cell_y = y / cell_size;
            
            uint8_t color = ((cell_x + cell_y) % 2) ? 255 : 0;
            
            row[x * 3 + 0] = color; // B
            row[x * 3 + 1] = color; // G
            row[x * 3 + 2] = color; // R
        }
        for (int i = width * 3; i < row_size; i++) {
            row[i] = 0;
        }
        fwrite(row, 1, row_size, file);
    }
    
    free(row);
    fclose(file);
    printf("Created: %s (checkerboard %dx%d)\n", filename, width, height);
}

void create_color_bars_bmp(const char* filename, int width, int height) {
    FILE* file = fopen(filename, "wb");
    if (!file) return;
    
    int row_size = (width * 3 + 3) / 4 * 4;
    int image_size = row_size * height;
    
    BMPHeader header = {
        .bfType = 0x4D42,
        .bfSize = sizeof(BMPHeader) + image_size,
        .bfOffBits = sizeof(BMPHeader),
        .biSize = 40,
        .biWidth = width,
        .biHeight = height,
        .biPlanes = 1,
        .biBitCount = 24,
        .biCompression = 0,
        .biSizeImage = image_size,
        .biXPelsPerMeter = 2835,
        .biYPelsPerMeter = 2835,
        .biClrUsed = 0,
        .biClrImportant = 0
    };
    
    fwrite(&header, sizeof(BMPHeader), 1, file);
    
    uint8_t* row = malloc(row_size);
    int bar_width = width / 8;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int bar = x / bar_width;
            
            uint8_t r = 0, g = 0, b = 0;
            
            switch(bar % 8) {
                case 0: r = 255; g = 255; b = 255; break; // Белый
                case 1: r = 255; g = 255; b = 0;   break; // Желтый
                case 2: r = 0;   g = 255; b = 255; break; // Голубой
                case 3: r = 0;   g = 255; b = 0;   break; // Зеленый
                case 4: r = 255; g = 0;   b = 255; break; // Пурпурный
                case 5: r = 255; g = 0;   b = 0;   break; // Красный
                case 6: r = 0;   g = 0;   b = 255; break; // Синий
                case 7: r = 0;   g = 0;   b = 0;   break; // Черный
            }
            
            row[x * 3 + 0] = b;
            row[x * 3 + 1] = g;
            row[x * 3 + 2] = r;
        }
        for (int i = width * 3; i < row_size; i++) {
            row[i] = 0;
        }
        fwrite(row, 1, row_size, file);
    }
    
    free(row);
    fclose(file);
    printf("Created: %s (color bars)\n", filename);
}

int main() {
    srand(time(NULL));
    
    // 1. Простой градиент
    create_gradient_bmp("test_gradient.bmp", 640, 480);
    
    // 2. Шахматная доска (хорошо для проверки фильтров)
    create_checkerboard_bmp("test_checkerboard.bmp", 512, 512, 32);
    
    // 3. Цветные полосы
    create_color_bars_bmp("test_colorbars.bmp", 640, 100);
    
    return 0;
}