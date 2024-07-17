# %%
import pandas as pd
import matplotlib.pyplot as plt
import os
import glob
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
from matplotlib import colors
import matplotlib as mpl
import seaborn as sns

import scienceplots

latex_columnwidth_in_cm = 8.5
figure_aspect_ratio = 4 / 6

latex_columnwidth_in_pt = latex_columnwidth_in_cm / 2.54

plt.style.use(['science', 'grid'])  # pip3 install SciencePlots --user

plt.rc('figure', dpi=150, figsize=[latex_columnwidth_in_pt, latex_columnwidth_in_pt * (4 / 6)])
plt.rc('font', size=10, serif='Times')
plt.rc('savefig', pad_inches=0.01, dpi=1000, transparent=True)

root_dir = 'min_breadth_ccl'
# %%

max_range = 3000
range_param = [0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,225,230,235,240,245,250,255,260,265,270,275,280,285,290,295,300,325,350,375,400,425,450,475,500,550,600,650,700,750,800,850,900,950,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,2000,2100,2200,2300,2400,2500,2600,2700,2800,2900,3000]
range_param.sort()

cpus = [1,2,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39,41,43,45,47,48]

num_cpu = len(cpus)

foreground_file_root = 'avg_object_time_iws_patient2_'
background_file_root = 'avg_background_time_iws_patient2_'
ccl_file_root =  'avg_ccl_times_iws_patient2_'

curves_f = []
curves_b = []
curves_ccl = []
average_f = []
average_b = []
average_ccl = []
average_both = []

# Iterate over every CSV file in root_dir
for cpu in range(0, num_cpu):
    curves_b.append([])
    curves_f.append([])
    curves_ccl.append([])
    average_f.append([])
    average_b.append([])
    average_ccl.append([])

    print("idx CPU: ", cpu)
    print("Number cpu: ", cpus[cpu])

    for param in range_param:
        # Load the CSV file
        df_foreground = pd.read_csv(
            os.path.join(root_dir, foreground_file_root + str(param) + '_' + str(cpus[cpu]) + '.csv'), header=None)
        df_background = pd.read_csv(
            os.path.join(root_dir, background_file_root + str(param) + '_' + str(cpus[cpu]) + '.csv'), header=None)
        df_ccl = pd.read_csv(os.path.join(root_dir, ccl_file_root + str(param) + '_' + str(cpus[cpu]) + '.csv'), header=None)
        # Append the curve to the list
        curves_f[cpu].append(df_foreground.to_numpy())
        curves_b[cpu].append(df_background.to_numpy())
        curves_ccl[cpu].append(df_ccl.to_numpy())

        # Compute the average
        average_f[cpu].append(np.sum(curves_f[cpu ][-1]))
        average_b[cpu].append(np.sum(curves_b[cpu ][-1]))
        average_ccl[cpu].append(np.sum(curves_ccl[cpu][-1]))



    # average_both.append(np.add(average_f[cpu], average_b[cpu]))
    average_both.append(average_ccl[cpu])

#devide average_both by 1e9

average_both = np.array(average_both) / 1e9


# best_arg = range[np.argmin(average_both)]
# speedup = average_both/average_both.min()

# %%

# Plot the curve
start = 0
stop = 500#(len(range_param))

cpu_range = cpus #[1,2,5,11,37,48]

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
plt.savefig('figures/min_breadth_perf_ccl.pdf')
plt.show()

# %%
plt.rc('font', size=11, serif='Times')
baseline_seq = pd.read_csv(os.path.join(root_dir, 'baseline_seq_patient2.csv'), header=None)
baseline_seq = baseline_seq / 1e9

# Find the minimum execution time for the 1 CPU configuration
min_time_1_cpu = 48#baseline_seq.to_numpy().sum()

# min_time_1_cpu = (min(average_both[0]))

# Calculate speedup for each CPU configuration
speedup = [min_time_1_cpu / min_time for min_time in average_both]

max = []

for speed in speedup:
    max.append(np.max(speed).astype(float))

print("Max speedup with {} thread : {}".format(cpus[np.argmax(max)], np.max(max)))

# Plot the speedup
plt.figure()
plt.plot(cpus, max, marker='o', markersize=2, label='Speedup')
plt.axhline(y=1, color='r', linestyle='--', label='Baseline (sequential)')
plt.legend()
plt.ylim(0,4)
#plt.title('Speedup with Different Number of threads')
plt.xlabel('Number of threads')
# plt.ylabel('Speedup')
plt.grid(True)
plt.savefig('figures/speedup_thread_num_ccl.pdf')
plt.show()

#%%
latex_columnwidth_in_cm = 8.5*2
figure_aspect_ratio = 4 / 6

latex_columnwidth_in_pt = latex_columnwidth_in_cm / 2.54

plt.style.use(['science', 'grid'])  # pip3 install SciencePlots --user

plt.rc('figure', dpi=150, figsize=[latex_columnwidth_in_pt, latex_columnwidth_in_pt * (4 / 6)])
plt.rc('font', size=8, serif='Times')
plt.rc('savefig', pad_inches=0.01, dpi=1000, transparent=True)
mpl.rcParams['grid.color'] = '#909090'

cpu_range = cpus
# Indices of CPUs of interest
cpu_range_indices = [cpus.index(cpu) for cpu in cpu_range]

# Create a meshgrid for the range parameters and CPU indices
X, Y = np.meshgrid(range_param, cpu_range_indices)

# Extract the corresponding data for the Z-axis

z = np.array([average_both[idx] for idx in cpu_range_indices])

z_max = np.zeros_like(z[:-1, :-1])
for i in range(z.shape[0] - 1):
    for j in range(z.shape[1] - 1):
        z_max[i, j] = np.max(z[i:i+2, j:j+2])

norm = colors.PowerNorm(gamma=0.22)
cmap = plt.cm.coolwarm

colors_m =  cmap(norm(z_max))

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Create the surface plot
surf = ax.plot_surface(X, Y, z,facecolors=colors_m, cmap='coolwarm',antialiased=False)
# Add labels
ax.set_xlabel('MIN_BREADTH',labelpad=-3.5)
ax.set_ylabel('Nb of threads',labelpad=-3.5)
ax.invert_yaxis()  # Invert the y-axis to match the CPU li
ax.set_zlabel('CCL Time (s)')
ax.set_yticks(cpu_range_indices)
ax.set_yticklabels([cpus[idx] for idx in cpu_range_indices])
ax.tick_params(axis='x', which='major', pad=-3)
ax.tick_params(axis='y', which='major', pad=-3)

ax.set_xlim(0,3000)


#one of every 10 line on the grid
# Reduce the number of grid lines
from matplotlib.ticker import MaxNLocator

ax.xaxis.set_major_locator(MaxNLocator(integer=True, prune='both', nbins=5))
ax.yaxis.set_major_locator(MaxNLocator(integer=True, prune='both', nbins=5))
ax.zaxis.set_major_locator(MaxNLocator(integer=True, prune='both', nbins=5))



#show colormap from colors_m
from matplotlib.cm import ScalarMappable
mappable = ScalarMappable(norm=norm, cmap=cmap)
mappable.set_array(z_max)
cbar = fig.colorbar(mappable, ax=ax, shrink=0.75, aspect=10)
cbar.set_ticks([13,15,20,30,50,40,60,70,80])
cbar.set_label('CCL Time (s)',size=10)
plt.savefig('figures/heatmap_ccl_3d_nul.pdf')

# Show the plot
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

# Indices of CPUs of interest
cpu_range_indices = [cpus.index(cpu) for cpu in cpu_range]

tick = 1

ax = sns.heatmap(average_both[:,::tick],cmap="coolwarm", norm=colors.PowerNorm(gamma=0.25),xticklabels=range_param[::tick], yticklabels=cpu_range)
scale = ax.get_xticks()[::3]
scale = np.append(scale,98.5)
ax.set_xticks(scale)

#add more number label to colorbar
cbar = ax.collections[0].colorbar
cbar.set_ticks([80,60,13,14,20,15,30,25,40,50])
#add label to color map
cbar = ax.collections[0].colorbar
cbar.set_label('CCL Time (s)',size=12)
plt.xlabel('MIN_BREADTH',size=12)
plt.ylabel('Nb of threads',size=12)
plt.xticks(rotation=90)
plt.yticks(rotation=0)
plt.savefig('figures/heatmap_ccl.pdf')
plt.show()

#%%

#find index of min value in average_both
min_idx = np.unravel_index(np.argmin(average_both, axis=None), average_both.shape)
print("Minimum value: ", average_both[min_idx])
print("Index of minimum value: ", min_idx)
print("Number of threads: ", cpus[min_idx[0]])
print("MIN_BREADTH: ", range_param[min_idx[1]])

baseline_seq = pd.read_csv(os.path.join(root_dir, 'baseline_seq_patient2.csv'), header=None)
baseline_seq = baseline_seq / 1e9

# Find the minimum execution time for the 1 CPU configuration
min_time_1_cpu = baseline_seq.to_numpy().sum()
print("Time for seq ", min_time_1_cpu)
