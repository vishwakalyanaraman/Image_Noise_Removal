#ifndef INCLUDED_A1_H
#define INCLUDED_A1_H

typedef struct{
    int width, height;
} Dimension;

typedef struct{
    unsigned char r, g, b;
} RGB;

//Used in readwritePPM.c
RGB *readPPM(char *file, int *width, int *height, int *max);
void writePPM(char *file, int *width, int *height, int max, const RGB *image);

//used in processImage.c
RGB *processImage(int width, int height, RGB *image, int windowSize, char *filterType);
void quickSort(unsigned char *a, int l, int r);
unsigned char partition(unsigned char *a, int l, int r);

unsigned char mean(unsigned char *a, int size);

#endif
