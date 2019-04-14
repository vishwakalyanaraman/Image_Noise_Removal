# Sequential Implementation

This is a sequential implentation of Mean and Median filters. This code is used to benchmark to measure the optimization achieved with CUDA and MPI.

## Running the Application

First run the makefile using the command

```bash
make
```

Then execute the program using the command

```bash
./seq_noise_removal input.ppm output.ppm N F
```
where
* input.ppm is the input PPM image
* output.ppm is the output PPM image
* N is an integer >= 3, where N x N is the size of the window of the image
* F is the filtering method of choice, where A is the Mean filter and M is the Median filter

For example
```bash
./seq_noise_removal balloon.ppm out.ppm 5 A
```

produces an output PPM image named out.ppm which uses the Mean filter as a denoising filter.
