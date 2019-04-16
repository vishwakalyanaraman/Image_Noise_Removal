import time
from subprocess import check_output

import matplotlib
import matplotlib.pyplot as plt


# Configuration
implementations = ['Sequential Implementation', 'MPI Implementation', 'CUDA Implementation']
build_scripts = ['sequential.sh', 'mpi.sh', 'cuda.sh'] 
binaries = [['./seq_noise_removal'], ['mpiexec', '-np', '5', './MPI_Executable'], ['./imageFilters.x']]
input_filename = 'mpi_in.ppm'
cuda_input_filename = 'cuda_in.ppm'
output_filename = 'output.ppm'
lower_window_size = 3
upper_window_size = 50
window_step_size = 2

# REPL
print('Loading Noise Removal Profiler')
print('Select Implementation\n 1. Sequential\n 2. MPI\n 3. CUDA\n')
implementation = [int(i)-1 for i in input().split()]
print('\nSelect Filter\n A. Mean\n B. Median\n')
filter_code = input()
print('Starting profiler with the binary')

# Build impmlementation
for i in implementation:
    rc = check_output(['sh', build_scripts[i]])
    print('Implementation {} built'.format(implementations[i]))

# Profile implementation
windows = [i for i in range (lower_window_size, upper_window_size, window_step_size)]
time_taken_collection = []
print('Running for window sizes {} to {} in steps of {}'.format(lower_window_size, upper_window_size, window_step_size))

for imp_code in implementation:
    print('\n\nProfiling' , implementations[imp_code])
    time_taken = []
    for i in windows:
        start = 0; end = 0

        if imp_code == 2:
            if filter_code == 'A':
                filter_code = 'mean'
            elif filter_code == 'B':
                filter_code = 'median'
            start = time.time()
            rc = check_output([*binaries[imp_code], str(i), cuda_input_filename, output_filename, filter_code])
            end = time.time()
            
        else :
            start = time.time()
            rc = check_output([*binaries[imp_code], input_filename, output_filename, str(i), filter_code])
            end = time.time()

        print('Windown size = {}, Time = {}'.format(i, end-start))
        time_taken.append(end-start)

    time_taken_collection.append(time_taken)


# Plot
print('Plotting')
imp_code = implementation[0]
ax1 = plt.subplot(311)
plt.plot(windows, time_taken_collection[0])
plt.setp(ax1.get_xticklabels(), fontsize=6)
ax1.set(xlabel='Window Size', ylabel='Time Taken',
       title='')

ax1.grid()

if len(implementation) >= 2:
    print("Plotting 2nd subgraph")  
    imp_code = implementation[1]
    ax2 = plt.subplot(312, sharex=ax1, sharey=ax1)
    plt.plot(windows, time_taken_collection[1])
    # make these tick labels invisible
    plt.setp(ax2.get_xticklabels(), visible=False)
    ax2.set(xlabel='Window Size', ylabel='Time Taken',
       title='')
    ax2.grid()
    ax2.title.set_position([0.1, 1.25])

if len(implementation) == 3:
    print("Plotting 3rd subgraph")
    imp_code = implementation[2]
    ax3 = plt.subplot(313, sharex=ax1,sharey=ax1)
    plt.plot(windows, time_taken_collection[2])
    plt.setp(ax3.get_xticklabels(), visible=False)
    ax3.set(xlabel='Window Size', ylabel='Time Taken',
       title='')
    ax3.grid()

plt.xlim(1, 42)
plt.ylim(0,7)
plt.show()
