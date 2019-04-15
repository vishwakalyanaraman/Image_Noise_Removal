# Image Noise Removal

This program implements two types of filters commonly used for removing noise in an image. The two filters here are a Mean Filter and a Median Filter. These filters work by calculating the Mean or Median of surrounding pixels in an image and uses that data to produce a noise free output. The program works on the principle of parallelism wherein multiple processes perform these calculations on respective image pixels.

## Prerequisites

You will need MPI running on your system in order to implement this application

## Running the Application

First run the makefile using the command

```
make
```

Then execute the program using the command

```
mpirun -np p ./MPI_Executable input.ppm output.ppm N F
```
where
* p is the number of processes
* input.ppm is the input PPM image
* output.ppm is the output PPM image
* N is an integer >= 3, where N x N is the size of the window of the filter
* F is the filtering method of choice, where A is the Mean filter and M is the Median filter

For example
```
mpirun -np 10 ./MPI_Executable balloon.ppm out.ppm 5 A
```

produces an output PPM image named out.ppm which uses the Mean filter as a denoising filter.
