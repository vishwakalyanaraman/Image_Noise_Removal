#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
extern "C" {
#include "ppmFile.h"
}

__global__ void mean(int *dW, int *dH, int *dR, unsigned char *dInput, unsigned char *dOutput) {
    int i, j, x, y, chan, temp, num, offset;
    i = blockIdx.x * blockDim.x + threadIdx.x;
    j = blockIdx.y * blockDim.y + threadIdx.y;
	for (chan = 0; chan < 3; chan++) {
		temp = 0;
		num = 0;
		for (y = j - (*dR); y <= j + (*dR); y++) {
	
			for (x = i - (*dR); x <= i + (*dR); x++) {
				if (x >= 0 && x < *dW && y >= 0 && y < *dH) {
					offset = (y * (*dW) + x) * 3 + chan;
					temp += dInput[offset];
					num++;
				}
			}
		}
		temp /= num;
		offset = (j * (*dW) + i) * 3 + chan;
		dOutput[offset] = temp;
	}
}

__global__ void median(int *dW, int *dH, int *dR, unsigned char *dInput, unsigned char *dOutput) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    int chan, offset;
    unsigned char filterVector[9] = {0,0,0,0,0,0,0,0,0};
    if((j==0) || (i==0) || (j==*dH-1) || (i==*dW-1))
        dOutput[i*(*dW)+j] = 0; //Deal with boundry conditions
    else {
        for(chan = 0; chan < 3; chan++){
            int index = 0;
            for (int y = j - (*dR); y <= j + (*dR); y++){
                for (int x = i - (*dR); x <= i + (*dR); x++){
                    offset = (y * (*dW) + x) * 3 + chan;
                    filterVector[index++] = dInput[offset];
                }
            }

            //Sort the filter array. This gives the median of the pixels at filterVector[4]
            for (int m = 0; m < 9; m++) {
                for (int n = m + 1; n < 9; n++) {
                    if (filterVector[m] > filterVector[n]) { 
                        //Swap the variables.
                        unsigned char tmp = filterVector[m];
                        filterVector[m] = filterVector[n];
                        filterVector[n] = tmp;
                    }
                }
            }
            offset = (j * (*dW) + i) * 3 + chan;
            dOutput[offset] = filterVector[4];
        }
    }
}

int main (int argc, char *argv[]) {
    int w, h, r, temp;
    Image *inImage;
    Image *outImage;
    unsigned char* data;
    double time;
    clock_t begin, end;
	unsigned char *dInput;
    unsigned char *dOutput;
    int *dW, *dH, *dR;
    char choice[10];
    
    if (argc != 5) {
		printf("Incorrect input arguments. Should be: <r> <input>.ppm <output>.ppm <choice>\n");
        return 0;
    }
    
	
	r = atoi(argv[1]);
	inImage = ImageRead(argv[2]);
    strcpy(choice, argv[4]);
	w = inImage->width;
	h = inImage->height;
	data = inImage->data;
	printf("Using image: %s, width: %d, height: %d, blur radius: %d\n",argv[2],w,h,r);
    printf("Waiting for GPU ...\n");

    // Grids are based on image size with blocks of 32x32
    dim3 blockD(32, 32);
    dim3 gridD((w + blockD.x - 1) / blockD.x, (h + blockD.y - 1) / blockD.y);
    

    cudaMalloc((void**)&dInput, sizeof(unsigned char*) * w * h * 3);
    cudaMalloc((void**)&dOutput, sizeof(unsigned char*) * w * h * 3);
    cudaMalloc((void**)&dW, sizeof(int*));
    cudaMalloc((void**)&dH, sizeof(int*));
    cudaMalloc((void**)&dR, sizeof(int*));
    

    cudaMemcpy(dInput, data, w * h * 3, cudaMemcpyHostToDevice);
    cudaMemcpy(dW, &w, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dH, &h, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(dR, &r, sizeof(int), cudaMemcpyHostToDevice);
    
	printf("Blurring image ...\n");
    printf("Grid size: %d x %d\n", gridD.x, gridD.y);
	printf("Block size: %d x %d\n", blockD.x, blockD.y);
    printf("Total number of threads: %d\n", gridD.x * gridD.y * blockD.x * blockD.y);
    
    if(!strcmp(choice, "mean")){
        begin = clock();
        
        mean<<<gridD, blockD>>>(dW, dH, dR, dInput, dOutput);

        end = clock();
    }
    else if (!strcmp(choice, "median")){
        begin = clock();
        
        median<<<gridD, blockD>>>(dW, dH, dR, dInput, dOutput);

        end = clock();
    }
    else{
        printf("\n\n~~~~~Incorrect filter choice. Use 'mean' or 'median' as an argument~~~~~\n\n");
        exit(1);
    }
    
    outImage = ImageCreate(w, h); //Create output image
	printf("Processing complete, assembling image ...\n");
    
    temp = w * h * 3;
    cudaMemcpy(outImage->data, dOutput, temp, cudaMemcpyDeviceToHost);

    cudaError_t error = cudaGetLastError();
    if(error != cudaSuccess)
        printf("\n\n````ERROR : %s````\n\n", cudaGetErrorString(error));
    else
        printf("\n\n````NO ERRORS````\n\n");
	
	// Write output image to file
	ImageWrite(outImage, argv[3]);
	printf("Output image created: %s\n", argv[3]);
    
    time = (double)(end-begin) / CLOCKS_PER_SEC;
    printf("Filter execution time: %e s\n", time);
    
	free(inImage->data);
	free(outImage->data);
    cudaFree(dInput);
    cudaFree(dOutput);
    cudaFree(dW);
    cudaFree(dH);
    cudaFree(dR);
    
    return 0;
}
