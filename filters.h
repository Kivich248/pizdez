#ifndef LABA3_FILTERS_H
#define LABA3_FILTERS_H
#include "functions.h"
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