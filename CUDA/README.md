## Prerequisites

You will need CUDA 10.1 running on your system in order to implement this application

## Running the Application

First run the makefile using the command

```
make
```

Then execute the program using the command

```
./imageFilters.x R input.ppm output.ppm FILTER
```
where
* input.ppm is the input PPM image
* output.ppm is the output PPM image
* R is an integer >= 3, where R x R is the size of the window used by the filter
* FILTER is the filtering option of choice where "mean" uses the mean filter and "median" uses the
  median filter

For example
```
./imageFilters.x 3 balloon.ppm out.ppm median
```

produces an output PPM image named out.ppm which uses the Median filter as a denoising filter.
