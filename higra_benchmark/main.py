import timeit

import numpy as np
import gc
from skimage import io as imageio
import matplotlib.pyplot as plt
import cv2
import higra as hg
import time

try:
    from utils import * # imshow, locate_resource, get_sed_model_file
except: # we are probably running from the cloud, try to fetch utils functions from URL
    import urllib.request as request; exec(request.urlopen('https://github.com/higra/Higra-Notebooks/raw/master/utils.py').read(), globals())

name_image = ["plant.jpeg","coral.jpg","tower.jpg","dwarf.jpg"]
n_marker = [59,25,27,28]

for j in range(1,len(name_image)-2):
    print(name_image[j])
    name_image_markers = name_image[j] + "data/step_"
    image = imageio.imread(name_image[j])
    image = image.astype(np.float32)/255
    gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    # discard the color channel
    size = gray_image.shape[:2]

    graph = hg.get_4_adjacency_graph(size)
    edge_weights = hg.weight_graph(graph, gray_image, hg.WeightFunction.L1)

    for i in range(0,n_marker[j]+1):
        try:
            label = imageio.imread(name_image_markers + str(i) +"_remove.png")
        except:
            label = imageio.imread(name_image_markers + str(i) + "_add.png")

        #time labelisation_seeded_watershed 10 times
        time_average = []
        gc.disable()
        for t in range(0,20):
            start = time.time()
            labelisation = hg.labelisation_seeded_watershed(graph, edge_weights, label)
            end = time.time()
            time_average.append((end-start)*1000)

        gc.enable()

        print(str(np.mean(time_average)))

        labelisation = labelisation*122
        labelisation = labelisation.astype(np.uint8)
        #imageio.imsave(name_image + "history/step_"+str(i)+"_labelisation.png",labelisation)