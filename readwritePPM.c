#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "a1.h"

void writePPM(char *file, int width, int height, int max, const RGB *image){
    
    FILE *fp;
    fp = fopen(file, "w");

    fprintf(fp, "P3\n"); //Header of file used for verification purposes in readPPM
    fprintf(fp, "%d %d\n%d\n", width, height, max);

    int size = width*height;

    //Write the PPM Image
    for(int i = 0; i < size; i++){
        const RGB *p = image + i;
        fprintf(fp, "%d %d %d", p->r, p->g, p->b);
    }
}

RGB *readPPM(char *file, int *width, int *height, int *max){

    FILE *fp;
    char c;
    int n;
    char b[100];
    int red, green, blue;

    fp = fopen(file, "r");
    n = fscanf(fp, "%[^\n] ", b); //Regex : matches a string of all characters != \n

    if(b[0] != 'P' || b[1] != '3'){
        printf("Not a PPM File!\n");
        exit(0);
    }

    printf("Valid PPM File\n");
    n = fscanf(fp, "%c", &c);
    
    while(c == '#'){
        n = fscanf(fp, "%[^\n] ", b);
        printf("%s\n", b);
        n = fscanf(fp, "%c", &c);
    }

    ungetc(c, fp);      //Takes a char and puts in back into input string
    n = fscanf(fp, "%d %d %d", width, height, max);
    assert(n==3);       //Assuming n = 3 for the rest of the code below, else return error

    printf("%d x %d image", *width, *height);

    int size = (*width) * (*height);

    RGB *image = (RGB*) malloc(sizeof(RGB) * size);
    assert(image);

    for(int i = 0; i < size; i++){
        n = fscanf(fp, "%d %d %d", &red, &green, &blue);
        assert(n==3);

        image[i].r = red;
        image[i].g = green;
        image[i].b = blue;
    }

    return image;
}

