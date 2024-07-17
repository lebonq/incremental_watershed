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

root_dir = 'exp_2_bigdataset'
# %%

#list images in the directory big_dataset
images = os.listdir("big_dataset")

nmarker = 70

#compute size of every images
nb_pixel_images = []
for image in images:
    print("Load image: ", image)
    cv_img = cv2.imread("big_dataset/" + image)
    #get size
    nb_pixel_images.append(cv_img.shape[0] * cv_img.shape[1])

df_images = []

cpt = 0

for image in images:
    print(image)
    #read data in nano second
    df = pd.DataFrame(columns=["IWS", "NIWS", "OpenCV", "DIFT", "HIGRA", "Init IWS", "Init DIFT"],
                      index=range(0, nmarker))
    df["IWS"] = pd.read_csv(root_dir + "/" + image + "_0_time_IW.csv", header=None).transpose()
    df["IWS"] = df["IWS"].astype(float).apply(lambda x: x * nb_pixel_images[cpt])
    #Convert in s
    df["IWS"] = df["IWS"] / 1e9


    data = pd.read_csv(root_dir + "/" + image + "_0_time_NIW.csv", header=None).transpose()
    df["NIWS"] = data
    df["NIWS"] = df["NIWS"] / 1e9


    data = pd.read_csv(root_dir + "/" + image + "_0_time_opencv.csv", header=None).transpose()
    df["OpenCV"] = data
    df["OpenCV"] = df["OpenCV"] / 1e9



    df["DIFT"] = pd.read_csv(root_dir + "/" + image + "_time_DIFT.csv", header=None).transpose()
    df["DIFT"] = df["DIFT"] / 1e9


    df["HIGRA"] = pd.read_csv(root_dir + "/" + image + "_time_higra.csv", header=None).transpose()
    df["HIGRA"] = df["HIGRA"] / 1e9


    df["Init IWS"] = pd.read_csv(root_dir + "/" + image + "_1_time_init_IW.csv", header=None).transpose()
    df["Init IWS"] = df["Init IWS"] / 1e9


    df["Init DIFT"] = pd.read_csv(root_dir + "/" + image + "_init_time_DIFT.csv", header=None).transpose()
    df["Init DIFT"] = df["Init DIFT"] / 1e9


    df_images.append(df)
    cpt += 1

# %% Figure for all images


#mean all value into one data frame
df_plot = pd.DataFrame(columns=["IWS", "NIWS", "OpenCV", "DIFT", "HIGRA"], index=range(0, nmarker))

df_plot["IWS"] = 0
df_plot["NIWS"] = 0
df_plot["OpenCV"] = 0
df_plot["DIFT"] = 0
df_plot["HIGRA"] = 0

cpt = 0

for df in df_images:
    df_plot["IWS"] += (nb_pixel_images[cpt] * 1) / df["IWS"]
    df_plot["NIWS"] += (nb_pixel_images[cpt] * 1) / df["NIWS"]
    df_plot["OpenCV"] += (nb_pixel_images[cpt] * 1) / df["OpenCV"]
    df_plot["DIFT"] += (nb_pixel_images[cpt] * 1) / df["DIFT"]
    df_plot["HIGRA"] += (nb_pixel_images[cpt] * 1) / df["HIGRA"]
    cpt += 1

df_plot = df_plot / len(df_images)

df_big_corrected = pd.DataFrame()

cpt = 0

for row in range(0, 70, 2):
    df_big_corrected.loc[cpt, "IWS"] = (df_plot.loc[row, "IWS"] + df_plot.loc[row + 1, "IWS"]) / 2
    df_big_corrected.loc[cpt, "NIWS"] = (df_plot.loc[row, "NIWS"] + df_plot.loc[row + 1, "NIWS"]) / 2
    df_big_corrected.loc[cpt, "OpenCV"] = (df_plot.loc[row, "OpenCV"] + df_plot.loc[row + 1, "OpenCV"]) / 2
    df_big_corrected.loc[cpt, "HIGRA"] = (df_plot.loc[row, "HIGRA"] + df_plot.loc[row + 1, "HIGRA"]) / 2
    df_big_corrected.loc[cpt, "DIFT"] = (df_plot.loc[row, "DIFT"] + df_plot.loc[row + 1, "DIFT"]) / 2
    cpt += 2

#convert from pixel per second to giga pixel per second
df_big_corrected = df_big_corrected / 1e6
df_plot = df_big_corrected


plt.figure()
plt.plot(df_plot.index, df_plot["IWS"], label='IWS')
plt.plot(df_plot.index, df_plot["NIWS"], label='NIWS')
plt.plot(df_plot.index, df_plot["OpenCV"],linestyle="dotted",label='OpenCV')
plt.plot(df_plot.index, df_plot["HIGRA"], linestyle='dashed', label='HIGRA')
plt.plot(df_plot.index, df_plot["DIFT"], label='DIFT')
plt.legend(fontsize='small', bbox_to_anchor=(.69, 0.92), loc='upper left')

# plt.title('Throughput per interaction across BIG dataset')

plt.xlabel('\# interaction')
plt.ylabel('MegaPixel per second')
plt.grid(True)
plt.yscale('log')
plt.savefig('figures/big_dataset_time_mgpx.pdf')
plt.show()

#%% Latex tab

df_images_pixsec = []

cpt = 0
for image in df_images:
    df = pd.DataFrame(columns=["IWS", "NIWS", "OpenCV", "DIFT", "HIGRA"], index=range(0, nmarker))
    df["IWS"] = (nb_pixel_images[cpt] * 1) / image["IWS"]
    df["NIWS"] = (nb_pixel_images[cpt] * 1) / image["NIWS"]
    df["OpenCV"] = (nb_pixel_images[cpt] * 1) / image["OpenCV"]
    df["DIFT"] = (nb_pixel_images[cpt] * 1) / image["DIFT"]
    df["HIGRA"] = (nb_pixel_images[cpt] * 1) / image["HIGRA"]
    df["Init IWS"] = (nb_pixel_images[cpt] * 1) / image["Init IWS"]
    df["Init DIFT"] = (nb_pixel_images[cpt] * 1) / image["Init DIFT"]
    df_images_pixsec.append(df)
    cpt += 1

#compute mean init time for DF_images
mean_iws_init = 0
mean_dift_init = 0

cpt = 0

for df in df_images_pixsec:
    mean_iws_init += df["Init IWS"].mean()
    mean_dift_init += df["Init DIFT"].mean()
    cpt = cpt + 1

mean_iws_init = mean_iws_init / len(df_images)
mean_dift_init = mean_dift_init / len(df_images)

#compute mean time for each method
mean_iws_full = 0
mean_niws_full = 0
mean_opencv_full = 0
mean_dift_full = 0
mean_higra_full = 0

cpt = 0
for df in df_images_pixsec:
    mean_iws_full += df["IWS"].mean()
    mean_niws_full += df["NIWS"].mean()
    mean_opencv_full += df["OpenCV"].mean()
    mean_dift_full +=df["DIFT"].mean()
    mean_higra_full += df["HIGRA"].mean()
    cpt += 1

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

cpt = 0

for df in df_images_pixsec:
    max_iws_full +=  df["IWS"].min()
    max_niws_full += df["NIWS"].min()
    max_opencv_full += df["OpenCV"].min()
    max_dift_full += df["DIFT"].min()
    max_higra_full += df["HIGRA"].min()
    cpt += 1

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

cpt = 0

for df in df_images:
    acc_iws_full += (nb_pixel_images[cpt] * 1) /( df["IWS"].sum() + df["Init IWS"].mean())
    acc_niws_full += (nb_pixel_images[cpt] * 1) / (df["NIWS"].sum() + df["Init IWS"].mean())
    acc_dift_full += (nb_pixel_images[cpt] * 1) / (df["DIFT"].sum()+ df["Init DIFT"].mean())
    acc_opencv_full += (nb_pixel_images[cpt] * 1) / df["OpenCV"].sum()
    acc_higra_full += (nb_pixel_images[cpt] * 1) / df["HIGRA"].sum()
    cpt += 1

#do the mean
acc_iws_full = acc_iws_full / len(df_images)
acc_niws_full = acc_niws_full / len(df_images)
acc_dift_full = acc_dift_full / len(df_images)
acc_opencv_full = acc_opencv_full / len(df_images)
acc_higra_full = acc_higra_full / len(df_images)

tab_2d_user = pd.DataFrame()

tab_2d_user.columns.name = 'Method'

# Init time
tab_2d_user.loc['IWS', "Init"] = mean_iws_init / 1e6
tab_2d_user.loc['NIWS', "Init"] = mean_iws_init / 1e6
tab_2d_user.loc['DIFT', "Init"] = mean_dift_init / 1e6
tab_2d_user.loc['OpenCV', "Init"] = None
tab_2d_user.loc['HIGRA', "Init"] = None

# Average time
tab_2d_user.loc['IWS', "Average"] = mean_iws_full / 1e6
tab_2d_user.loc['NIWS', "Average"] = mean_niws_full / 1e6
tab_2d_user.loc['DIFT', "Average"] = mean_dift_full / 1e6
tab_2d_user.loc['OpenCV', "Average"] = mean_opencv_full / 1e6
tab_2d_user.loc['HIGRA', "Average"] = mean_higra_full / 1e6

# Max time
tab_2d_user.loc['IWS', "Max"] = max_iws_full / 1e6
tab_2d_user.loc['NIWS', "Max"] = max_niws_full / 1e6
tab_2d_user.loc['DIFT', "Max"] = max_dift_full / 1e6
tab_2d_user.loc['OpenCV', "Max"] = max_opencv_full / 1e6
tab_2d_user.loc['HIGRA', "Max"] = max_higra_full / 1e6

# Accumulated time Init + Sum
tab_2d_user.loc['IWS', "Accumulated"] = acc_iws_full/ 1e6
tab_2d_user.loc['NIWS', "Accumulated"] = acc_niws_full/ 1e6
tab_2d_user.loc['DIFT', "Accumulated"] = acc_dift_full/ 1e6
tab_2d_user.loc['OpenCV', "Accumulated"] = acc_opencv_full/ 1e6
tab_2d_user.loc['HIGRA', "Accumulated"] = acc_higra_full/ 1e6

tab_2D_user_renderer = tab_2d_user.style.highlight_max(props='underline:--rwrap;')
tab_2D_user_renderer.na_rep = "$\emptyset$"
tab_2D_user_renderer.format('{:.2f}')

tab_2D_user_renderer.set_table_styles([
    {'selector': 'toprule', 'props': ':hline;'},
    {'selector': 'midrule', 'props': ':hline;'},
    {'selector': 'bottomrule', 'props': ':hline;'},
], overwrite=False)

tab_2D_user_renderer.to_latex('table_2d_exp_big.tex', position_float="centering", label="tab:2d_exp_big",
                              column_format="|l|c|c|c|c|")
