# Image Noise Removal

One Paragraph of project description goes here

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
* N is an integer >= 3, where N x N is the size of the window of the image
* F is the filtering method of choice, where MEAN is the Mean filter and MEDIAN is the Median filter

For example
```
mpirun -np 10 ./MPI_Executable balloon.ppm out.ppm 5 MEAN
```

produces an output PPM image named out.ppm which uses the Mean filter as a denoising filter.
