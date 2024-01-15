# load image in val

import os
from copy import deepcopy

import cv2 as cv
import numpy as np
import pandas as pd
import random

np.random.seed(42)

#list all files in dataset_raw
files = os.listdir('dataset_raw')

for file in files:
    if "_im.jpg" in file:
        continue
    img_path = "dataset_raw/" + file.split('.')[0].split('_gt')[0]

    # Load the image
    gt = cv.imread(img_path + '_gt.png')
    gt_plot = deepcopy(gt)
    img = cv.imread(img_path + '_im.jpg')
    img_plot = deepcopy(img)

    name = img_path.split('/')[-1]

    image_data_dir = name + '_im.jpgdata'

    if not os.path.exists(image_data_dir):
        os.makedirs(image_data_dir)

    nb_markers = 35 #so 35*2 = 70 markers

    kernel = cv.getStructuringElement(cv.MORPH_ELLIPSE, (23, 23))
    gt_eroded = cv.erode(gt, kernel, iterations=1)
    gt_dilated = cv.dilate(gt, kernel, iterations=1)


    # Find the indices of all pixels with a value of 255
    indices_object = np.where(gt_eroded == 255)
    len_object = len(indices_object[0])
    indices_background = np.where(gt_dilated == 0)
    len_background = len(indices_background[0])

    random_pos_obj = []
    random_pos_bkg = []

    for i in range(nb_markers):
        random_index = np.random.randint(len_object)

        # Retrieve the corresponding position
        x = indices_object[1][random_index]
        y = indices_object[0][random_index]
        #remove
        # Retrieve the corresponding position
        random_position = (x,y)
        random_pos_obj.append(random_position)

        print("Random position with a pixel value of 255:", random_position)
        #i want to generate a circle on the gt image
        gt_plot = cv.circle(gt_plot, random_position, 10, (0, 255, 0), -1)
        img_plot = cv.circle(img_plot, random_position, 10, (0, 255, 0), -1)

    for i in range(nb_markers):
        random_index = np.random.randint(len_background)

        # Retrieve the corresponding position
        x = indices_background[1][random_index]
        y = indices_background[0][random_index]
        # Retrieve the corresponding position
        random_position = (x, y)
        random_pos_bkg.append(random_position)

        print("Random position with a pixel value of 0:", random_position)
        #i want to generate a circle on the gt image
        gt_plot = cv.circle(gt_plot, random_position, 10, (0, 0, 255), -1)
        img_plot = cv.circle(img_plot, random_position, 10, (0, 0, 255), -1)

    cv.imwrite(name + '_gt.png', gt_plot)
    cv.imwrite(name + '_im.png', img_plot)

    #create black grayscale image
    markers_cumulative = np.zeros((gt.shape[0], gt.shape[1]), dtype=np.uint8)
    markers_tmp = np.zeros((gt.shape[0], gt.shape[1]), dtype=np.uint8)

    idx = 0

    for i in range(nb_markers):
        #object
        random_position = random_pos_obj[i]

        markers_tmp = np.zeros((gt.shape[0], gt.shape[1]), dtype=np.uint8)
        markers_tmp = cv.circle(markers_tmp, random_position, 10, 1, -1)

        indices_tmp = np.where(markers_cumulative == 1)
        markers_tmp[indices_tmp] = 0
        markers_cumulative = cv.add(markers_cumulative, markers_tmp)

        cv.imwrite(name + '_im.jpgdata/step_'+str(idx)+'_add.png', markers_cumulative)

        non_zero_indices = np.nonzero(markers_tmp)
        non_zero_coordinates = non_zero_indices[1] + (non_zero_indices[0] * markers_tmp.shape[1])
        non_zero_coordinates = np.reshape(non_zero_coordinates, (1,len(non_zero_coordinates)))
        np.savetxt(name + '_im.jpgdata/step_'+str(idx)+'_add.txt', non_zero_coordinates, delimiter=',', fmt='%d')
        idx = idx + 1

        ##background
        random_position = random_pos_bkg[i]

        markers_tmp = np.zeros((gt.shape[0], gt.shape[1]), dtype=np.uint8)
        markers_tmp = cv.circle(markers_tmp, random_position, 10, 2, -1)

        indices_tmp = np.where(markers_cumulative == 2)
        markers_tmp[indices_tmp] = 0
        markers_cumulative = cv.add(markers_cumulative, markers_tmp)

        cv.imwrite(name + '_im.jpgdata/step_' + str(idx) + '_add.png', markers_cumulative)

        non_zero_indices = np.nonzero(markers_tmp)
        non_zero_coordinates = non_zero_indices[1] + (non_zero_indices[0] * markers_tmp.shape[1])
        non_zero_coordinates = np.reshape(non_zero_coordinates, (1, len(non_zero_coordinates)))
        np.savetxt(name + '_im.jpgdata/step_' + str(idx) + '_add.txt', non_zero_coordinates, delimiter=',', fmt='%d')
        idx = idx + 1
