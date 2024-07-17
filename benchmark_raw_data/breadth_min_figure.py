# %%
import pandas as pd
import matplotlib.pyplot as plt
import os
import glob
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

import scienceplots

latex_columnwidth_in_cm = 8.5
figure_aspect_ratio = 4 / 6

latex_columnwidth_in_pt = latex_columnwidth_in_cm / 2.54

plt.style.use(['science', 'grid'])  # pip3 install SciencePlots --user

plt.rc('figure', dpi=150, figsize=[latex_columnwidth_in_pt, latex_columnwidth_in_pt * (4 / 6)])
plt.rc('font', size=8, serif='Times')
plt.rc('savefig', pad_inches=0.01, dpi=1000, transparent=True)

root_dir = 'min_breadth_perf_analisys'
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

cpus = [1, 2, 3, 4, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 48]

num_cpu = len(cpus)

foreground_file_root = 'avg_object_time_iws_patient2_'
background_file_root = 'avg_background_time_iws_patient2_'

curves_f = []
curves_b = []
average_f = []
average_b = []
average_both = []

# Iterate over every CSV file in root_dir
for cpu in range(0, num_cpu):
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
        curves_f[cpu].append(df_foreground.to_numpy())
        curves_b[cpu ].append(df_background.to_numpy())

        # Compute the average
        average_f[cpu].append(np.sum(curves_f[cpu ][-1]))
        average_b[cpu].append(np.sum(curves_b[cpu ][-1]))

    average_both.append(np.add(average_f[cpu], average_b[cpu]))

# best_arg = range[np.argmin(average_both)]
# speedup = average_both/average_both.min()

# %%

# Plot the curve
start = 0
stop = 500#(len(range_param))

cpu_range = [1,2,5,11,37,48]

#find idx of lowest value in average_both
# get the idx of the value of cpu_range
cpu_range = [cpus.index(cpu) for cpu in cpu_range]
print(cpu_range)

# plt.plot(range[start:stop], average_b[start:stop], label='Background', marker='x',markersize=4)
# plt.plot(range[start:stop], average_f[start:stop], label='Foreground', marker='o',markersize=4)
for cpu in cpu_range:
    label_plot = str(cpus[cpu]) + ' Proc'
    plt.plot(range_param, average_both[cpu], label=label_plot, markersize=2)
plt.legend(fontsize=5.5,ncol=3,bbox_to_anchor=(0.20, 0.64))
# plt.title("Execution time over MIN\_BREADTH")

plt.xlabel('MIN\_BREADTH')
plt.ylabel('Accumulated Time (s)')
# plt.yscale('log')
plt.savefig('figures/min_breadth_perf_analysis.pdf')
plt.show()

# %%
baseline_seq = pd.read_csv(os.path.join(root_dir, 'baseline_seq_patient2.csv'), header=None)

# Find the minimum execution time for the 1 CPU configuration
min_time_1_cpu = baseline_seq.to_numpy().sum()

# min_time_1_cpu = (min(average_both[0]))

# Calculate speedup for each CPU configuration
speedup = [min_time_1_cpu / min_time for min_time in average_both]

max = []

for speed in speedup:
    max.append(np.max(speed).astype(float))


# Plot the speedup
plt.figure()
plt.plot(cpus, max, marker='o', markersize=2, label='Speedup')
plt.axhline(y=1, color='r', linestyle='--', label='Baseline (sequential)')
plt.legend()
#plt.title('Speedup with Different Number of threads')
plt.xlabel('Number of threads')
# plt.ylabel('Speedup')
plt.grid(True)
plt.savefig('figures/speedup_thread_num.pdf')
plt.show()

#%%
latex_columnwidth_in_cm = 8.5*2
figure_aspect_ratio = 4 / 6

latex_columnwidth_in_pt = latex_columnwidth_in_cm / 2.54

plt.style.use(['science', 'grid'])  # pip3 install SciencePlots --user

plt.rc('figure', dpi=150, figsize=[latex_columnwidth_in_pt, latex_columnwidth_in_pt * (4 / 6)])
plt.rc('font', size=8, serif='Times')
plt.rc('savefig', pad_inches=0.01, dpi=1000, transparent=True)

cpu_range = cpus

#one fo eveyr 2 value in cpus
cpu_range = cpus[::2]
cpu_range.append(11)
cpu_range.remove(13)

# Indices of CPUs of interest
cpu_range_indices = [cpus.index(cpu) for cpu in cpu_range]

# Create a meshgrid for the range parameters and CPU indices
X, Y = np.meshgrid(range_param, cpu_range_indices)

# Extract the corresponding data for the Z-axis
Z = np.array([average_both[idx] for idx in cpu_range_indices])

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Create the surface plot
surf = ax.plot_surface(X, Y, Z, cmap='viridis')

# Add labels
ax.set_xlabel('MIN_BREADTH')
ax.set_ylabel('Nombre de threads')
ax.invert_yaxis()  # Invert the y-axis to match the CPU li
ax.set_zlabel('Accumulated Time (s)')
ax.set_yticks(cpu_range_indices)
ax.set_yticklabels([cpus[idx] for idx in cpu_range_indices])

# Add a color bar which maps values to colors
fig.colorbar(surf, shrink=0.5, aspect=5)

# Show the plot
plt.show()
