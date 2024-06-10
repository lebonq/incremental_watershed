import os
import cv2
from scipy.ndimage import zoom
from tifffile import imwrite, TiffWriter

width = 512
height = 512

names = ["patient_2","patient_7","patient_12","patient_19","patient_20"]
depth = [172, 151, 260, 124, 225]

import numpy as np

def from1Dto3D(p, w, h):
    z = p // (w * h)
    y = (p - z * w * h) // w
    x = p - z * w * h - y * w
    return x, y,z

# Load the marker file (assuming each marker ID is on a separate line)
for i in range(0, len(names)):

    volume = np.zeros((depth[i],width, height), dtype=np.uint8)

    step_files = 0

    for step in range(0, 30):
        marker_file_background = names[i] + "/" + names[i] + "_liver_markers/" + "markers_background_" + str(step) + ".txt"
        marker_file_object = names[i] + "/" + names[i] + "_liver_markers/" + "markers_object_" + str(step) + ".txt"
        with open(marker_file_background, 'r') as f:
            markers_background = [int(line.strip()) for line in f.readlines()]

        with open(marker_file_object, 'r') as f:
            markers_object = [int(line.strip()) for line in f.readlines()]

        # Convert the 1D markers to 3D coordinates
        markers_background_3D = []
        for marker in markers_background:
            markers_background_3D.append(from1Dto3D(marker, width, height))

        markers_object_3D = []
        for marker in markers_object:
            markers_object_3D.append(from1Dto3D(marker, width, height))

        path_object = "modified_slices/" + names[i] + "_markers"
        path_background = "modified_slices/" + names[i] + "_markers"

        if not os.path.exists(path_object):
            os.makedirs(path_object)

        if not os.path.exists(path_background):
            os.makedirs(path_background)

        for marker in markers_object_3D:
            x, y, z = marker
            volume[z,x,y] = 2

        """for j in range(depth[i]):
            new_filename = f'{path_object}/slice_{j:03d}.png'
            cv2.imwrite(new_filename, volume[:, :, j])"""

        with TiffWriter(path_object+"/step_"+str(step_files) +"_volume.tif") as tif:
            tif.write(volume.astype(np.int8))

        step_files += 1

        print(f"Saved modified slices with object for {names[i]}")

        for marker in markers_background_3D:
            x, y, z = marker
            volume[z,x,y] = 1

        """for j in range(depth[i]):
            new_filename = f'{path_background}/slice_{j:03d}.png'
            cv2.imwrite(new_filename, volume[:, :, j])"""

        with TiffWriter(path_background+"/step_"+str(step_files) +"_volume.tif") as tif:
            tif.write(volume.astype(np.int8))

        step_files += 1


        print(f"Saved modified slices with background for {names[i]}")
