/*
 *
 * Read and Write PPM6 Files
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ppmFile.h"


void giveUp(char *message){
    printf("Message : %s\n", message);
    exit(121);
}

void checkDimension(int dim){
    if( dim < 1 || dim > 4000)
        giveUp("Dimension exceeding limits");
}

void readPPMHeader(FILE *fp, int *width, int *height){
    char ch;
    int max;

    if(fscanf(fp, "P%c\n", &ch) != 1 || ch != '6')
        giveUp("File is not in PPM6 RAW Format");

    ch = getc(fp);
    while(ch == '#'){
        do{
            ch = getc(fp);
        }while(ch != '\n');
        ch = getc(fp);
    }

    if(!isdigit(ch))
        giveUp("Cannot read header information from PPM");

    ungetc(ch, fp);

    fscanf(fp, "%d%d%d\n", width, height, &max);

    if(max != 255)
        giveUp("Image is not in 24-bit colour");

    checkDimension(*width);
    checkDimension(*height);
}

Image *ImageCreate(int width, int height){
    Image *image = (Image*) malloc(sizeof(Image));

    if(!image)
        giveUp("Error allocating memory for new image");

    image->width = width;
    image->height = height;
    image->data = (unsigned char *) malloc(width * height * 3);

    if(!image->data)
        giveUp("Error allocating memory for image->data");
    
    return image;
}

Image *ImageRead(char *filename){
    int width, height, num, size;

    Image *image = (Image *) malloc(sizeof(Image));

    FILE *fp = fopen(filename, "r");

    if(!image)
        giveUp("Error allocating memory for new image");
    if(!fp)
        giveUp("Error opening file");

    readPPMHeader(fp, &width, &height);

    size = width * height * 3;
    image->data = (unsigned char *) malloc(size);
    image->width = width;
    image->height = height;

    if(!image->data)
        giveUp("Error allocating memory new image");

    num = fread((void *) image->data, 1, (size_t) size, fp);

    if(num != size)
        giveUp("Error reading image data from file");

    fclose(fp);
    return image;
}

void ImageWrite(Image *image, char *filename){
    int num;
    int size = image->width * image->height * 3;

    FILE *fp = fopen(filename, "w");
    if(!fp)
        giveUp("Error opening file handle");

    fprintf(fp, "P6\n%d %d\n%d\n", image->width, image->height, 255);       //Writing PPM6 Header

    num = fwrite((void*) image->data, 1, (size_t) size, fp);
    if(num != size)
        giveUp("Error writing image to file");

    fclose(fp);
}

int ImageWidth (Image *image){
    return image->width;
}

int ImageHeight(Image *image){
    return image->height;
}

void ImageClear(Image *image, unsigned char red, unsigned char green, unsigned char blue){
    int size = image->width * image->height;

    unsigned char *data = image->data;
    for(int i = 0; i < size; i++){
        *data++ = red;
        *data++ = green;
        *data++ = blue;
    }
}

void ImageSetPixel(Image *image, int x, int y, int chan, unsigned char val){
    int offset = (y * image->width + x) * 3 + chan;

    image->data[offset] = val;
}

char ImageGetPixel(Image *image, int x, int y, int chan){
    int offset = (y * image->width + x) * 3 + chan;

    return image->data[offset];
}
