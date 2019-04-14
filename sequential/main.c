#include <stdlib.h>
#include "a1.h"
#include <stdio.h>
int main(int argc, char** argv){
	
	// Image and Filtered Image
	RGB *image, *filteredImage;
    
	int global_width, global_height;
	int width, height, max;
	
	int dest, source;
	int offset;

	int windowSize = atoi(argv[3]);
	int tag = 0;
    // Read the image
    image = readPPM(argv[1], &global_width, &global_height, &max);

	// Process image    argv[4] decides Mean or Median Filtering	
	filteredImage = processImage(global_width, global_height, image, windowSize, argv[4]);	
    
    //Write the noise-removed image to file.
    writePPM(argv[2], global_width, global_height, max, filteredImage);
    // Free image file
	free(image);
    
	return(0);
}
