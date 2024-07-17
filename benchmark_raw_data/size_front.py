
# %%
import pandas as pd
import matplotlib.pyplot as plt
import os
import glob
import numpy as np
import cv2
import csv

import scienceplots

latex_columnwidth_in_cm = 8.5
figure_aspect_ratio = 4 / 6

latex_columnwidth_in_pt = latex_columnwidth_in_cm / 2.54

plt.style.use(['science', 'grid'])  # pip3 install SciencePlots --user

plt.rc('figure', dpi=150, figsize=[latex_columnwidth_in_pt, latex_columnwidth_in_pt * (4 / 6)])
plt.rc('font', size=12, serif='Times')
plt.rc('savefig', pad_inches=0.01, dpi=1000, transparent=True)

file_path = "sizefront/size_front_0_patient2.csv"

data = csv.reader(open(file_path))

# Step 2: Read the data from the CSV file
data = list(data)
data = np.array(data).astype(int)


#%%

# Split the line into individual values separated by commas
values = data[0]

min = 0
max = 3000#np.max(values)

# Step 3: Convert the values to integers
values = [int(value) for value in values]

#%%

# Step 5: Plot the histogram
# plt.hist(filtered_values, cumulative=True, bins=50, edgecolor='black', density=True, range=(min, max))

min_all, max_all = np.min(values), np.max(values)
range_ratio = (max_all - min_all) / (max - min)
plt.hist(values, weights=values, cumulative=-1,edgecolor='black', bins=int(round(60 * range_ratio)), density=True,color='green', alpha=0.4)
plt.hist(values, cumulative=-1,edgecolor='black', bins=int(round(60 * range_ratio)), density=True,color='blue', alpha=0.3)
plt.xlim(min, max)

#plt.title('Proportion of exploration in function of level set size')
plt.xlabel('level set size')

plt.ylabel('Proportion in percent')
plt.yticks()
plt.savefig("figures/frontsize.pdf")
plt.show()