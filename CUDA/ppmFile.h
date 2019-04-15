#ifndef PPM_H
#define PPM_H

#include <sys/types.h>

typedef struct image{
    int width;
    int height;
    unsigned char *data;
}Image;

Image *ImageCreate(int width, int height);

Image *ImageRead(char *filename);
void ImageWrite(Image *image, char *filename);

int ImageWidth(Image *image);
int ImageHeight(Image *image);

void ImageClear(Image *image, unsigned char red, unsigned char green, unsigned char blue);
void ImageSetPixel(Image *image, int x, int y, int chan, unsigned char val);
char ImageGetPixel(Image *image, int x, int y, int chan);

#endif
