# %%
import pandas as pd
import matplotlib.pyplot as plt
import os
import glob
import numpy as np

import scienceplots

latex_columnwidth_in_cm = 20.5
figure_aspect_ratio = 4 / 6

latex_columnwidth_in_pt = latex_columnwidth_in_cm / 2.54

plt.style.use(['science', 'grid'])  # pip3 install SciencePlots --user

plt.rc('figure', dpi=150, figsize=[latex_columnwidth_in_pt, latex_columnwidth_in_pt * (4 / 6)])
plt.rc('font', size=8, serif='Times')
plt.rc('savefig', pad_inches=0.01, dpi=1000, transparent=True)

root_dir = '/home/lebonqu/CLionProjects/incremental_watershed/data_3d/patient_2/patient_2_liver_markers'
# %%
step = 250
max_range = 4000
range_param = np.arange(0, max_range + 1, step)

range_param = np.append(range, np.array([1, 5, 10, 15, 60, 20, 73, 126, 180, 233]))

range_param = [0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350, 375, 400, 425, 450, 475, 500, 525,
               550, 575, 600, 625, 650, 675, 700, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000]
range_param = [0, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 1000, 1500, 1750, 2000, 2500,
               3000]
range_param = [0, 50, 100, 150, 200, 250, 300, 350, 400, 500, 600, 700, 1000, 1500, 1750, 2000, 2500, 3000]
range_param.sort()

cpus = [1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 48]

num_cpu = len(cpus)

foreground_file_root = 'avg_object_time_iws_patient2_'
background_file_root = 'avg_background_time_iws_patient2_'

curves_f = []
curves_b = []
average_f = []
average_b = []
average_both = []

# Iterate over every CSV file in root_dir
for cpu in range(1, num_cpu):
    curves_b.append([])
    curves_f.append([])
    average_f.append([])
    average_b.append([])

    print("idx CPU: ", cpu)
    print("Number cpu: ", cpus[cpu])

    for param in range_param:
        # Load the CSV file
        df_foreground = pd.read_csv(
            os.path.join(root_dir, foreground_file_root + str(param) + '_' + str(cpus[cpu]) + '.csv'), header=None)
        df_background = pd.read_csv(
            os.path.join(root_dir, background_file_root + str(param) + '_' + str(cpus[cpu]) + '.csv'), header=None)

        # Append the curve to the list
        curves_f[cpu - 1].append(df_foreground[cpu - 1].to_numpy())
        curves_b[cpu - 1].append(df_background[cpu - 1].to_numpy())

        # Compute the average
        average_f[cpu - 1].append(np.sum(curves_f[cpu - 1][-1]))
        average_b[cpu - 1].append(np.sum(curves_b[cpu - 1][-1]))

    average_both.append(np.add(average_f[cpu - 1], average_b[cpu - 1]))

# best_arg = range[np.argmin(average_both)]
# speedup = average_both/average_both.min()

# %%

# Plot the curve
start = 0
stop = (len(range_param))

cpu_range = [1,2,9,11,13,15,37]

#find idx of lowest value in average_both
# get the idx of the value of cpu_range
cpu_range = [cpus.index(cpu) for cpu in cpu_range]
print(cpu_range)

# plt.plot(range[start:stop], average_b[start:stop], label='Background', marker='x',markersize=4)
# plt.plot(range[start:stop], average_f[start:stop], label='Foreground', marker='o',markersize=4)
for cpu in cpu_range:
    label_plot = str(cpus[cpu]) + ' CPU'
    plt.plot(range_param, average_both[cpu], label=label_plot, marker='o', markersize=2)
plt.legend()
plt.title('Average execution time for foreground and background given Hparam')
plt.xlabel('front_size')
plt.ylabel('Time (s)')
plt.yscale('log')
plt.show()

# %%

# Plot the curve
start = 0
stop = (len(range_param))

cpu_range = [1,2,9,11,13,15,37]

#find idx of lowest value in average_both
# get the idx of the value of cpu_range
cpu_range = [cpus.index(cpu) for cpu in cpu_range]
print(cpu_range)

# plt.plot(range[start:stop], average_b[start:stop], label='Background', marker='x',markersize=4)
# plt.plot(range[start:stop], average_f[start:stop], label='Foreground', marker='o',markersize=4)
for cpu in cpu_range:
    label_plot = str(cpus[cpu]) + ' CPU'
    plt.plot(range_param, average_both[cpu], label=label_plot, marker='o', markersize=2)
plt.legend()
plt.title('Average execution time for foreground and background given Hparam')
plt.xlabel('front_size')
plt.ylabel('Time (s)')
plt.yscale('log')
plt.show()

# %%
# Find the minimum execution time for the 1 CPU configuration
min_time_1_cpu = min(average_both[0])

# Calculate speedup for each CPU configuration
speedup = [min_time_1_cpu / min_time for min_time in average_both]

max =[]

for speed in speedup:
    max.append(np.max(speed).astype(float))


# Plot the speedup
plt.figure()
plt.plot(cpus, max, marker='o', markersize=4, label='Speedup')
plt.axhline(y=1, color='r', linestyle='--', label='Baseline (1 CPU)')
plt.legend()
plt.title('Speedup with Different Number of CPUs')
plt.xlabel('Number of CPUs')
plt.ylabel('Speedup')
plt.grid(True)
plt.show()
