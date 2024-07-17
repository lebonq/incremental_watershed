import numpy as np
from PIL import Image
import os
import SimpleITK as sitk
import time
import tqdm
import argparse

parser = argparse.ArgumentParser(description='Name of the patient')
parser.add_argument('patient', type=str, help='Name of the patient')

args = parser.parse_args()

print("Patient: ", args.patient)

def from1Dto3D(p, w, h):
    z = p // (w * h)
    y = (p - z * w * h) // w
    x = p - z * w * h - y * w
    return x, y, z

patients = ['patient_2', 'patient_7', 'patient_12', 'patient_19', 'patient_20']

patient = args.patient

for bench in tqdm.tqdm(range(0, 5),"Bench"):
    time_obj = []
    time_back = []

    # Get a sorted list of all PNG files in the directory
    dir_path = patient + "/" + patient + "_png/"
    file_list = sorted([os.path.join(dir_path, file) for file in os.listdir(dir_path) if file.endswith('.png')])

    # Initialize an empty list to hold the image data
    volume = []

    # Loop through each file in the list
    for file in file_list:
        # Open the image file
        img = Image.open(file)
        # Convert the image data to a numpy array and append it to the list
        volume.append(np.array(img))

    # Convert the list of 2D arrays into a 3D array
    volume = np.stack(volume, axis=0)

    # Convert the numpy array to a SimpleITK image
    input_image = sitk.GetImageFromArray(volume)
    feature_img = sitk.GradientMagnitude(input_image)

    # Read the marker image
    min_img = sitk.RegionalMinima(feature_img, backgroundValue=0, foregroundValue=1.0, fullyConnected=False,
                                  flatIsMinima=True)
    marker_img = sitk.ConnectedComponent(min_img)
    marker_img *= 0  # Marker set to 0

    marker_1D_obj = np.array([])
    marker_1D_bkg = np.array([])

    for cpt in range(0, 30):
        # list txt file to get the marker
        marker_1D_obj_l = np.loadtxt(patient + "/" + patient + "_liver_markers/markers_object_"     + str(cpt) + ".txt")
        marker_1D_bkg_l = np.loadtxt(patient + "/" + patient + "_liver_markers/markers_background_" + str(cpt) + ".txt")
        marker_1D_obj = np.concatenate((marker_1D_obj, marker_1D_obj_l))
        marker_1D_bkg = np.concatenate((marker_1D_bkg, marker_1D_bkg_l))

        for marker in marker_1D_obj:
            marker = int(marker)
            x, y, z = from1Dto3D(marker, volume.shape[1], volume.shape[2])
            idx = feature_img.TransformPhysicalPointToIndex((y, x, z))
            marker_img[idx] = 2

        # Apply the watershed transform
        start_obj = time.time_ns()
        watershed_image = sitk.MorphologicalWatershedFromMarkers(input_image, marker_img)
        end_obj = time.time_ns()

        print("Time for watershed object marker: ", end_obj - start_obj)
        time_obj.append(end_obj - start_obj)

        for marker in marker_1D_bkg:
            marker = int(marker)
            x, y, z = from1Dto3D(marker, volume.shape[1], volume.shape[2])
            idx = feature_img.TransformPhysicalPointToIndex((y, x, z))
            marker_img[idx] = 1

        # Apply the watershed transform
        start_back = time.time_ns()
        watershed_image = sitk.MorphologicalWatershedFromMarkers(input_image, marker_img)
        end_back = time.time_ns()

        print("Time for watershed background marker: ", end_back - start_back)
        time_back.append(end_back - start_back)

        # Write the output image
        #sitk.WriteImage(watershed_image, "watershed_image_"+patient+"_" + str(cpt) + ".nii")

    np.savetxt("itk_avg_object_time_"     + str(bench) + "_" + patient + ".csv", np.array(time_obj ))
    np.savetxt("itk_avg_background_time_" + str(bench) + "_" + patient + ".csv", np.array(time_back))
