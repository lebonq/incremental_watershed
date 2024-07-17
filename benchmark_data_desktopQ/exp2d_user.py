# %%
import pandas as pd
import matplotlib.pyplot as plt
import os
import glob
import numpy as np

import scienceplots

latex_columnwidth_in_cm = 8.5
figure_aspect_ratio = 4 / 6

latex_columnwidth_in_pt = latex_columnwidth_in_cm / 2.54

plt.style.use(['science', 'grid'])  # pip3 install SciencePlots --user

plt.rc('figure', dpi=150, figsize=[latex_columnwidth_in_pt, latex_columnwidth_in_pt * (4 / 6)])
plt.rc('font', size=8, serif='Times')
plt.rc('savefig', pad_inches=0.01, dpi=1000, transparent=True)

root_dir = 'exp1_2D_user'
# %%

images = [ "plant.jpeg","coral.jpg", "tower.jpg", "dwarf.jpg"]

nb_pixel = 1536*2048

n_markers = [59,25, 27, 28]

df_images = []

cpt = 0

for image in images:
    print(image)
    #read data in nano second
    df = pd.DataFrame(columns=["IWS", "NIWS", "OpenCV", "DIFT", "HIGRA", "Init IWS", "Init DIFT"],index=range(0, n_markers[cpt]))
    df["IWS"] = pd.read_csv(root_dir + "/" + image + "_0_time_IW.csv", header=None).transpose()
    df["IWS"] = df["IWS"].astype(float).apply(lambda x: x * nb_pixel)
    #Convert in ms
    df["IWS"] = df["IWS"] / 1e6

    data = pd.read_csv(root_dir + "/" + image + "_0_time_NIW.csv", header=None).transpose()
    df["NIWS"] = data
    df["NIWS"] = df["NIWS"] / 1e6

    data = pd.read_csv(root_dir + "/" + image + "_0_time_opencv.csv", header=None).transpose()
    df["OpenCV"] = data
    df["OpenCV"] = df["OpenCV"] / 1e6

    df["DIFT"] = pd.read_csv(root_dir + "/" + image + "_time_DIFT.csv", header=None).transpose()
    df["DIFT"] = df["DIFT"] / 1e6

    df["HIGRA"] = pd.read_csv(root_dir + "/" + image + "_time_higra.csv", header=None).transpose()
    df["HIGRA"] = df["HIGRA"] / 1e6

    df["Init IWS"] = pd.read_csv(root_dir + "/" + image + "_1_time_init_IW.csv", header=None).transpose()
    df["Init IWS"] = df["Init IWS"] / 1e6

    df["Init DIFT"] = pd.read_csv(root_dir + "/" + image + "_init_time_DIFT.csv", header=None).transpose()
    df["Init DIFT"] = df["Init DIFT"] / 1e6

    df_images.append(df)
    cpt += 1


# %% Figure for plant
df_pixelvisited_plant = pd.read_csv(root_dir + "/graph_plant.csv")["Pixel Visited"]

df_plot = df_images[0]

plt.figure()
plt.plot(df_plot.index, df_plot["IWS"], markersize=4, label='IWS')
plt.plot(df_plot.index, df_plot["NIWS"], markersize=4, label='NIWS')
plt.plot(df_plot.index, df_plot["OpenCV"], markersize=4, linestyle="dotted",label='OpenCV')
plt.plot(df_plot.index, df_plot["HIGRA"], markersize=4, linestyle="dashed",label='HIGRA')
plt.plot(df_plot.index, df_plot["DIFT"], markersize=4, label='DIFT')
plt.grid(True)
plt.legend(fontsize=6,ncol=2,bbox_to_anchor=(.20, 0.4))
plt.title('Time per interaction for plant')
plt.xlabel('\# interaction')
plt.ylabel('Time in ms')
plt.yscale('log')
#plot pixel in plain black line on another axis
plt.twinx()
plt.plot(df_pixelvisited_plant, color='black', label='Pixel visited')
plt.ylabel('Pixel visited in Millions')
plt.grid(False)
plt.legend(fontsize=5,ncol=2,bbox_to_anchor=(.71, 0.39),framealpha=0)
plt.savefig('figures/plant_time_ms.pdf')

plt.show()

#%% Latex tab

#compute mean init time for DF_images
mean_iws_init = 0
mean_dift_init = 0
for df in df_images:
    mean_iws_init += df["Init IWS"].mean()
    mean_dift_init += df["Init DIFT"].mean()

mean_iws_init = mean_iws_init / len(df_images)
mean_dift_init = mean_dift_init / len(df_images)

#compute mean time for each method
mean_iws_full = 0
mean_niws_full = 0
mean_opencv_full = 0
mean_dift_full = 0
mean_higra_full = 0

for df in df_images:
    mean_iws_full += df["IWS"].mean()
    mean_niws_full += df["NIWS"].mean()
    mean_opencv_full += df["OpenCV"].mean()
    mean_dift_full += df["DIFT"].mean()
    mean_higra_full += df["HIGRA"].mean()

mean_iws_full = mean_iws_full / len(df_images)
mean_niws_full = mean_niws_full / len(df_images)
mean_opencv_full = mean_opencv_full / len(df_images)
mean_dift_full = mean_dift_full / len(df_images)
mean_higra_full = mean_higra_full / len(df_images)

# Copute the max time for each method
max_iws_full = 0
max_niws_full = 0
max_opencv_full = 0
max_dift_full = 0
max_higra_full = 0

for df in df_images:
    max_iws_full = df["IWS"].max()
    max_niws_full =df["NIWS"].max()
    max_opencv_full =  df["OpenCV"].max()
    max_dift_full = df["DIFT"].max()
    max_higra_full =df["HIGRA"].max()

max_iws_full = max_iws_full / len(df_images)
max_niws_full = max_niws_full / len(df_images)
max_opencv_full = max_opencv_full / len(df_images)
max_dift_full = max_dift_full / len(df_images)
max_higra_full = max_higra_full / len(df_images)

#Compute the accumulated time
acc_iws_full = 0
acc_niws_full = 0
acc_dift_full = 0
acc_opencv_full = 0
acc_higra_full = 0

for df in df_images:
    acc_iws_full += df["IWS"].sum()
    acc_niws_full += df["NIWS"].sum()
    acc_dift_full += df["DIFT"].sum()
    acc_opencv_full += df["OpenCV"].sum()
    acc_higra_full += df["HIGRA"].sum()

#do the mean
acc_iws_full = acc_iws_full / len(df_images)
acc_niws_full = acc_niws_full / len(df_images)
acc_dift_full = acc_dift_full / len(df_images)
acc_opencv_full = acc_opencv_full / len(df_images)
acc_higra_full = acc_higra_full / len(df_images)

#Add the init time

acc_iws_full = acc_iws_full + mean_iws_init
acc_niws_full = acc_niws_full + mean_iws_init
acc_dift_full = acc_dift_full + mean_dift_init
acc_opencv_full = acc_opencv_full
acc_higra_full = acc_higra_full

tab_2d_user = pd.DataFrame()

tab_2d_user.columns.name = 'Method'

# Init time
tab_2d_user.loc['IWS', "Init"] = mean_iws_init
tab_2d_user.loc['NIWS', "Init"] = mean_iws_init
tab_2d_user.loc['DIFT', "Init"] = mean_dift_init
tab_2d_user.loc['OpenCV', "Init"] = None
tab_2d_user.loc['HIGRA', "Init"] = None

# Average time
tab_2d_user.loc['IWS', "Average"] = mean_iws_full
tab_2d_user.loc['NIWS', "Average"] = mean_niws_full
tab_2d_user.loc['DIFT', "Average"] = mean_dift_full
tab_2d_user.loc['OpenCV', "Average"] = mean_opencv_full
tab_2d_user.loc['HIGRA', "Average"] = mean_higra_full

# Max time
tab_2d_user.loc['IWS', "Max"] = max_iws_full
tab_2d_user.loc['NIWS', "Max"] = max_niws_full
tab_2d_user.loc['DIFT', "Max"] = max_dift_full
tab_2d_user.loc['OpenCV', "Max"] = max_opencv_full
tab_2d_user.loc['HIGRA', "Max"] = max_higra_full


# Accumulated time Init + Sum
tab_2d_user.loc['IWS', "Accumulated"] = acc_iws_full
tab_2d_user.loc['NIWS', "Accumulated"] = acc_niws_full
tab_2d_user.loc['DIFT', "Accumulated"] = acc_dift_full
tab_2d_user.loc['OpenCV', "Accumulated"] = acc_opencv_full
tab_2d_user.loc['HIGRA', "Accumulated"] = acc_higra_full


tab_2D_user_renderer = tab_2d_user.style.highlight_min(props='underline:--rwrap;')
tab_2D_user_renderer.na_rep = "$\emptyset$"
tab_2D_user_renderer.format('{:.2f}')

tab_2D_user_renderer.set_table_styles([
    {'selector': 'toprule', 'props': ':hline;'},
    {'selector': 'midrule', 'props': ':hline;'},
    {'selector': 'bottomrule', 'props': ':hline;'},
], overwrite=False)

tab_2D_user_renderer.to_latex('table_2d_exp_user.tex', position_float="centering", label="tab:2d_exp",
                              column_format="|l|c|c|c|c|")