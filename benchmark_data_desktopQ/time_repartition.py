# %%
import pandas as pd
import matplotlib.pyplot as plt
import os
import glob
import numpy as np
import cv2

import scienceplots

latex_columnwidth_in_cm = 8.5
figure_aspect_ratio = 4 / 6

latex_columnwidth_in_pt = latex_columnwidth_in_cm / 2.54

plt.style.use(['science', 'grid'])  # pip3 install SciencePlots --user

plt.rc('figure', dpi=150, figsize=[latex_columnwidth_in_pt, latex_columnwidth_in_pt * (4 / 6)])
plt.rc('font', size=8, serif='Times')
plt.rc('savefig', pad_inches=0.01, dpi=1000, transparent=True)

root_dir = 'par_detailled'
# %%
nb_thread = 11

times = pd.DataFrame(columns=["ALL", "SEQ", "PAR"], index=range(0, 60))
times_wo_sync = pd.DataFrame()

times["ALL"] = pd.read_csv(f'{root_dir}/all_time_iws_patient2_250_11_bench_0.csv', header=None).transpose()
times["SEQ"] = pd.read_csv(f'{root_dir}/seq_times_iws_patient2_250_11_bench_0.csv', header=None).transpose()
times["PAR"] = pd.read_csv(f'{root_dir}/par_times_iws_patient2_250_11_bench_0.csv', header=None).transpose()

for i in range(0, nb_thread):
    times_wo_sync[f"T{i}"] = pd.read_csv(f'{root_dir}/real_thread_times_iws_patient2_250_11_bench_0_thread_{i}.csv', header=None).transpose()