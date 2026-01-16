#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "filters.h"
#include "functions.h"
#include <ctype.h>
#include <string.h>
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
    if (argc < 3) {
        printf("ты идиот?\n");
        
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    
    Image* img = read_bmp(input_file);
    if (img == NULL) {
        printf("печально\n");
        return 1;
    }

    
    if (argc == 3) {
       
        int result = write_bmp(output_file, img);
        if (!result) {
            printf("грустно'\n", output_file);
            destroy_image(img);
            return 1;
        }
        destroy_image(img);
        return 0;
    }
	int width = img->width;
	int height = img->height;
    
    
	Image* img_got = create_image(width, height);
    int i = 3;
    while (i < argc) {
       
        if (strcmp(argv[i], "-grey") == 0 ) {
            img_got = Grey(img);
        }
        else if (strcmp(argv[i], "-negro") == 0) {
            img_got = Negro(img);
        }
        else if (strcmp(argv[i], "-sharp") == 0) {
            img_got = Sharp(img);
        }
        else if (strcmp(argv[i], "-median") == 0 && argc > i+1 && isdigit(argv[i+1])) {
            int window = atoi(argv[i+1]);
            img_got = Median(img, window);
        }
        else if (strcmp(argv[i], "-crop") == 0 && argc > i+2 && isdigit(argv[i+1]) && isdigit(argv[i+2])) {
            int w = atoi(argv[i+1]);
            int h = atoi(argv[i+2]);
            img_got = Crop(img, w, h);
            
        }else {
			printf("введи правильную команду\n");
			return 1;
		}
    

    
    write_bmp(argv[2], img);
    
 
    destroy_image(img);
    
    
}

    return 0;
} //допиши едж и гаусс, хз что с эджем делать, потому что атой  этом случае не работает...