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

root_dir = 'ccl_times'
# %%

avg_object_time_iws = []
avg_background_time_iws = []
avg_time_iws = []
ccl_times_iws = []

avg_object_time_iws_par = []
avg_background_time_iws_par = []
avg_time_iws_par = []
ccl_times_iws_par = []

avg_time_niws = []
ccl_times_niws = []

volumes = ["patient2", "patient7", "patient12", "patient19", "patient20"]

volumes_size = [512*512*172,512*512*151,512*512*260,512*512*124,512*512*225]

nmarker = 60

cpt_volumes = 0

df_volumes = []

for volume in volumes:
    avg_object_time_iws.append(pd.read_csv(f'{root_dir}/avg_object_time_iws_{volume}.csv', header=None).transpose())
    avg_background_time_iws.append(pd.read_csv(f'{root_dir}/avg_background_time_iws_{volume}.csv', header=None).transpose())
    ccl_times_iws.append(pd.read_csv(f'{root_dir}/avg_ccl_times_iws_{volume}.csv', header=None).transpose())

    avg_object_time_iws_par.append(pd.read_csv(f'{root_dir}/avg_object_time_iws_{volume}_par.csv', header=None).transpose())
    avg_background_time_iws_par.append(pd.read_csv(f'{root_dir}/avg_background_time_iws_{volume}_par.csv', header=None).transpose())
    ccl_times_iws_par.append(pd.read_csv(f'{root_dir}/avg_ccl_times_iws_{volume}_par.csv', header=None).transpose())

    avg_time_niws.append(pd.read_csv(f'{root_dir}/avg_object_time_niws_{volume}_seq.csv', header=None).transpose())
    ccl_times_niws.append(pd.read_csv(f'{root_dir}/avg_ccl_times_niws_{volume}_seq.csv', header=None).transpose())

    df = pd.DataFrame(columns=["IWS", "IWS_PAR"], index=range(0, nmarker))

    cpt = 0
    for mark in range(0, 60,2):
        df.loc[mark, "IWS"] = avg_object_time_iws[cpt_volumes][0].loc[cpt]
        df.loc[mark+1, "IWS"] = avg_background_time_iws[cpt_volumes][0].loc[cpt]

        df.loc[mark, "IWS_PAR"] = avg_object_time_iws_par[cpt_volumes][0].loc[cpt]
        df.loc[mark+1, "IWS_PAR"] = avg_background_time_iws_par[cpt_volumes][0].loc[cpt]
        cpt += 1

    df["IWS"] = df["IWS"].astype(float) * 1e9
    df["IWS_PAR"] = df["IWS_PAR"].astype(float)* 1e9

    df["NIWS"] = avg_time_niws[cpt_volumes][0]* 1e9
    df["IWS_CCL"] = ccl_times_iws[cpt_volumes][0]
    df["IWS_PAR_CCL"] = ccl_times_iws_par[cpt_volumes][0]
    df["NIWS_CCL"] = ccl_times_niws[cpt_volumes][0]

    cpt_volumes += 1
    df_volumes.append(df)


#%%

# Concatenate DataFrames along the rows
df_concat_iws_ccl = pd.concat([df_volumes[0]["IWS_CCL"], df_volumes[1]["IWS_CCL"], df_volumes[2]["IWS_CCL"], df_volumes[3]["IWS_CCL"], df_volumes[4]["IWS_CCL"]], axis=1)
df_concat_iws_par_ccl = pd.concat([df_volumes[0]["IWS_PAR_CCL"], df_volumes[1]["IWS_PAR_CCL"], df_volumes[2]["IWS_PAR_CCL"], df_volumes[3]["IWS_PAR_CCL"], df_volumes[4]["IWS_PAR_CCL"]], axis=1)

#Do the row on mean for df_concat_iws_ccl
iws_ccl_mean = df_concat_iws_ccl.mean(axis=1)
iws_par_ccl_mean = df_concat_iws_par_ccl.mean(axis=1)

#Mean all interactions
iws_ccl_mean = iws_ccl_mean.mean()
iws_par_ccl_mean = iws_par_ccl_mean.mean()

#speed up
print("Speed up IWS: ", iws_ccl_mean/iws_par_ccl_mean)
