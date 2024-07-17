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
plt.rc('font', size=11, serif='Times')
plt.rc('savefig', pad_inches=0.01, dpi=1000, transparent=True)

root_dir = 'exp_3_3d'
# %%

volumes = ["patient2", "patient7", "patient12", "patient19", "patient20"]

volumes_size = [512*512*172,512*512*151,512*512*260,512*512*124,512*512*225]

nmarker = 60

df_images_object = []
df_images_background = []

cpt = 0

for image in volumes: #object
    print(image)

    #read data in nano second
    df = pd.DataFrame(columns=["IWS", "IWS_PAR", "Init IWS", "Init IWS_PAR"],index=range(0, 30))

    df["IWS"] = pd.read_csv(root_dir + "/avg_object_time_iws_" + image + "_seq.csv", header=None).transpose()


    df["IWS_PAR"] = pd.read_csv(root_dir + "/avg_object_time_iws_" + image + "_par.csv", header=None).transpose()


    df["ITK"] = 0

    itk_data = 0
    for i in range(0, 5):
        data = pd.read_csv(root_dir + "/itk_avg_object_time_" + str(i) + "_" + image + ".csv", header=None).to_numpy()
        itk_data += data
    itk_data = itk_data / 5
    df["ITK"] = itk_data
    #Convert in ms
    df["ITK"] = df["ITK"] / 1e9

    df_images_object.append(df)
    cpt += 1

cpt = 0
for image in volumes: #background
    print(image)

    df = pd.DataFrame(columns=["IWS", "IWS_PAR", "NIWS","ITK", "Amira", "Init IWS", "Init IWS_PAR"],index=range(0, 30))
    df["IWS"] = pd.read_csv(root_dir + "/avg_background_time_iws_" + image + "_seq.csv", header=None).transpose()


    df["IWS_PAR"] = pd.read_csv(root_dir + "/avg_background_time_iws_" + image + "_par.csv", header=None).transpose()


    itk_data = 0
    for i in range(0, 5):
        data = pd.read_csv(root_dir + "/itk_avg_background_time_" + str(i) + "_" + image + ".csv", header=None).to_numpy()
        itk_data += data
    itk_data = itk_data / 5
    df["ITK"] = itk_data
    #Convert in ms
    df["ITK"] = df["ITK"] / 1e9

    df_images_background.append(df)
    cpt += 1


df_volumes = []

for volume in range(0, len(volumes)):
    df = pd.DataFrame(columns=["IWS", "IWS_PAR", "Amira", "Init IWS", "Init IWS_PAR"], index=range(0, nmarker))

    df["NIWS"] = pd.read_csv(root_dir + "/avg_object_time_niws_" + volumes[volume] + "_seq.csv", header=None).transpose()


    data = pd.read_csv(root_dir + "/benchmark_amira_workstation/" + volumes[volume] + "_execution_times.csv")
    df["Amira"] = data["Execution Time"]


    df["Init IWS"] = pd.read_csv(root_dir + "/init_time_niws_" + image + "_seq.csv", header=None).transpose()


    df["Init IWS_PAR"] = pd.read_csv(root_dir + "/init_time_iws_" + image + "_par.csv", header=None).transpose()


    cpt = 0
    for mark in range(0, 60,2):
        df.loc[mark, "IWS"] = df_images_object[volume].loc[cpt, "IWS"]
        df.loc[mark+1, "IWS"] = df_images_background[volume].loc[cpt, "IWS"]

        df.loc[mark, "IWS_PAR"] = df_images_object[volume].loc[cpt, "IWS_PAR"]
        df.loc[mark+1, "IWS_PAR"] = df_images_background[volume].loc[cpt, "IWS_PAR"]

        df.loc[mark, "ITK"] = df_images_object[volume].loc[cpt, "ITK"]
        df.loc[mark+1, "ITK"] = df_images_background[volume].loc[cpt, "ITK"]

        cpt += 1

    df["IWS"] = df["IWS"].astype(float)
    df["IWS_PAR"] = df["IWS_PAR"].astype(float)

    df_volumes.append(df)

# %%%

full = 0
full_itk = 0
full_l = 0
for df in df_volumes:
    df = df_volumes[0]
    full += df["IWS"].sum()
    full_l +=  df["IWS_PAR"].sum()
    full_itk +=    df["ITK"].sum()

full = full/5
full_l = full_l/5

print(full)
print(full_l)
print(full/full_l)

print("Speedup  iws_par over iws", full/full_l)
print("Speedup  iws over itk", full_itk/full)
print("Speedup  iws_par over itk", full_itk/full_l)

# %% Figure for all images


#mean all value into one data frame
df_plot = pd.DataFrame(columns=["IWS", "NIWS", "IWS_PAR", "Amira", "ITK"], index=range(0, nmarker))

df_plot["IWS"] = 0
df_plot["NIWS"] = 0
df_plot["IWS_PAR"] = 0
df_plot["Amira"] = 0
df_plot["ITK"] = 0

cpt = 0

for df in df_volumes:
    df_plot["IWS"] +=volumes_size[cpt] / df["IWS"]
    df_plot["NIWS"] +=volumes_size[cpt] /  df["NIWS"]
    df_plot["IWS_PAR"] += volumes_size[cpt] / df["IWS_PAR"]
    df_plot["Amira"] += volumes_size[cpt] / df["Amira"]
    df_plot["ITK"] +=volumes_size[cpt] /  df["ITK"]
    cpt += 1

df_plot = df_plot / len(df_volumes)

df_big_corrected = pd.DataFrame()

cpt = 0

#Smooth the curve
for row in range(0, nmarker,2):
    df_big_corrected.loc[cpt, "IWS"] = (df_plot.loc[row, "IWS"] + df_plot.loc[row + 1, "IWS"])/2
    df_big_corrected.loc[cpt, "NIWS"] = (df_plot.loc[row, "NIWS"] + df_plot.loc[row + 1, "NIWS"])/2
    df_big_corrected.loc[cpt, "IWS_PAR"] = (df_plot.loc[row, "IWS_PAR"] + df_plot.loc[row + 1, "IWS_PAR"])/2
    df_big_corrected.loc[cpt, "Amira"] = (df_plot.loc[row, "Amira"] + df_plot.loc[row + 1, "Amira"])/2
    df_big_corrected.loc[cpt, "ITK"] = (df_plot.loc[row, "ITK"] + df_plot.loc[row + 1, "ITK"])/2

    cpt += 2

df_plot = df_big_corrected / 1e6

plt.figure()
plt.plot(df_plot.index, df_plot["IWS"], markersize=4, label='IWS')
plt.plot(df_plot.index, df_plot["NIWS"], markersize=4, label='NIWS')
plt.plot(df_plot.index, df_plot["IWS_PAR"], markersize=4, label='IWS_PAR')
plt.plot(df_plot.index, df_plot["Amira"], markersize=4, label='Amira')
plt.plot(df_plot.index, df_plot["ITK"], markersize=4, label='ITK')
plt.legend(fontsize=7, bbox_to_anchor=(1.04, 0.63), loc='upper left',ncol=1)

# plt.title('Throughput per interactions across 3D volumes')

plt.xlabel('\# interaction')
plt.ylabel('MegaPixel per second')
plt.yscale('log')
plt.grid(True)
plt.savefig('figures/3d_exp_time_mgpx.pdf')
plt.show()


print("avergae across all iteration for ITK :", df["ITK"].mean())
print("avergae across all iteration for IWS :", df["IWS"].mean())
print("avergae across all iteration for IWS_PAR :", df["IWS_PAR"].mean())
print("avergae across all iteration for AMIRA :", df["Amira"].mean())
print("avergae across all iteration for NIWS :", df["NIWS"].mean())

#%% Latex tab

df_images_pixsec = []

cpt = 0
for image in df_volumes:
    df = pd.DataFrame(columns=["IWS", "NIWS", "IWS_PAR", "Amria", "ITK"], index=range(0, nmarker))
    df["IWS"] = (volumes_size[cpt] * 1) / image["IWS"]
    df["NIWS"] = (volumes_size[cpt] * 1) / image["NIWS"]
    df["IWS_PAR"] = (volumes_size[cpt] * 1) / image["IWS_PAR"]
    df["Amira"] = (volumes_size[cpt] * 1) / image["Amira"]
    df["ITK"] = (volumes_size[cpt] * 1) / image["ITK"]
    df["Init IWS"] = (volumes_size[cpt] * 1) / image["Init IWS"]
    df["Init IWS_PAR"] = (volumes_size[cpt] * 1) / image["Init IWS_PAR"]
    df_images_pixsec.append(df)
    cpt += 1

#compute mean init time for DF_images
mean_iws_init = 0
mean_iws_par_init = 0

for df in df_images_pixsec:
    mean_iws_init += df["Init IWS"].mean()
    mean_iws_par_init += df["Init IWS_PAR"].mean()

mean_iws_init = mean_iws_init / len(df_volumes)
mean_iws_par_init = mean_iws_par_init / len(df_volumes)

#compute mean time for each method
mean_iws_full = 0
mean_niws_full = 0
mean_iws_par_full = 0
mean_amira_full = 0
mean_itk_full = 0

for df in df_images_pixsec:
    mean_iws_full += df["IWS"].mean()
    mean_niws_full += df["NIWS"].mean()
    mean_iws_par_full += df["IWS_PAR"].mean()
    mean_amira_full += df["Amira"].mean()
    mean_itk_full += df["ITK"].mean()

mean_iws_full = mean_iws_full / len(df_volumes)
mean_niws_full = mean_niws_full / len(df_volumes)
mean_iws_par_full = mean_iws_par_full / len(df_volumes)
mean_amira_full = mean_amira_full / len(df_volumes)
mean_itk_full = mean_itk_full / len(df_volumes)

# Copute the max time for each method
max_iws_full = 0
max_niws_full = 0
max_iws_par_full = 0
max_amira_full = 0
max_itk_full = 0

for df in df_images_pixsec:
    max_iws_full += df["IWS"].min()
    max_niws_full += df["NIWS"].min()
    max_iws_par_full += df["IWS_PAR"].min()
    max_amira_full +=  df["Amira"].min()
    max_itk_full +=  df["ITK"].min()

max_iws_full = max_iws_full / len(df_volumes)
max_niws_full = max_niws_full / len(df_volumes)
max_iws_par_full = max_iws_par_full / len(df_volumes)
max_amira_full = max_amira_full / len(df_volumes)
max_itk_full = max_itk_full / len(df_volumes)


#Compute the accumulated time
acc_iws_full = 0
acc_niws_full = 0
acc_iws_par_full = 0
acc_amira_full = 0
acc_itk_full = 0

cpt = 0

for df in df_volumes:
    acc_iws_full += volumes_size[cpt] / (df["IWS"].sum() + df["Init IWS"].sum())
    acc_niws_full +=volumes_size[cpt] / ( df["NIWS"].sum() + df["Init IWS"].sum())
    acc_iws_par_full += volumes_size[cpt] / ( df["IWS_PAR"].sum() + df["Init IWS_PAR"].sum())
    acc_amira_full += volumes_size[cpt] / ( df["Amira"].sum())
    acc_itk_full +=volumes_size[cpt] / (  df["ITK"].sum())

#do the mean
acc_iws_full = acc_iws_full / len(df_volumes)
acc_niws_full = acc_niws_full / len(df_volumes)
acc_iws_par_full = acc_iws_par_full / len(df_volumes)
acc_amira_full = acc_amira_full / len(df_volumes)
acc_itk_full = acc_itk_full / len(df_volumes)

#Add the init time


tab_2d_user = pd.DataFrame()

tab_2d_user.columns.name = 'Method'

# Init time
tab_2d_user.loc['IWS', "Init"] = mean_iws_init / 1e6
tab_2d_user.loc['NIWS', "Init"] = mean_iws_init / 1e6
tab_2d_user.loc['IWS_PAR', "Init"] = mean_iws_par_init / 1e6
tab_2d_user.loc['Amira', "Init"] = None
tab_2d_user.loc['ITK', "Init"] = None

# Average time
tab_2d_user.loc['IWS', "Average"] = mean_iws_full  / 1e6
tab_2d_user.loc['NIWS', "Average"] = mean_niws_full  / 1e6
tab_2d_user.loc['IWS_PAR', "Average"] = mean_iws_par_full  / 1e6
tab_2d_user.loc['Amira', "Average"] = mean_amira_full  / 1e6
tab_2d_user.loc['ITK', "Average"] = mean_itk_full   / 1e6

# Max time
tab_2d_user.loc['IWS', "Max"] = max_iws_full / 1e6
tab_2d_user.loc['NIWS', "Max"] = max_niws_full / 1e6
tab_2d_user.loc['IWS_PAR', "Max"] = max_iws_par_full / 1e6
tab_2d_user.loc['Amira', "Max"] = max_amira_full / 1e6
tab_2d_user.loc['ITK', "Max"] = max_itk_full / 1e6


# Accumulated time Init + Sum
tab_2d_user.loc['IWS', "Accumulated"] = acc_iws_full / 1e6
tab_2d_user.loc['NIWS', "Accumulated"] = acc_niws_full / 1e6
tab_2d_user.loc['IWS_PAR', "Accumulated"] = acc_iws_par_full / 1e6
tab_2d_user.loc['Amira', "Accumulated"] = acc_amira_full / 1e6
tab_2d_user.loc['ITK', "Accumulated"] = acc_itk_full / 1e6

tab_2D_user_renderer = tab_2d_user.style.highlight_max(props='underline:--rwrap;')
tab_2D_user_renderer.na_rep = "$\emptyset$"
tab_2D_user_renderer.format('{:.2f}')

tab_2D_user_renderer.set_table_styles([
    {'selector': 'toprule', 'props': ':hline;'},
    {'selector': 'midrule', 'props': ':hline;'},
    {'selector': 'bottomrule', 'props': ':hline;'},
], overwrite=False)

tab_2D_user_renderer.to_latex('table_3d_exp.tex', position_float="centering", label="tab:3d_exp",
                              column_format="|l|c|c|c|c|")