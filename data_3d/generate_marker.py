import pydicom
import matplotlib.pyplot as plt
import numpy as np
import cv2 as cv

def generate_random_position(volume):
    rand_x = np.random.randint(0, volume[0].shape[0])
    rand_y = np.random.randint(0, volume[0].shape[1])
    rand_z = np.random.randint(0, len(volume))
    return rand_x, rand_y, rand_z

def get_1D(x, y, z, volume):
    return x + y * volume[0].shape[0] + z * volume[0].shape[0] * volume[0].shape[1]

path = "patient_1_liver/image_"
nb_images = 260

nb_batch = 30
nb_marker = 10

volume = []
volume_marked = [[]]

rand_pos_add3D = np.zeros((nb_batch, nb_marker, 3), dtype=int)

rand_pos_add1D = np.zeros((nb_batch, nb_marker, 121), dtype=int)

for i in range(0, nb_images):
    #open dicom with opencv
    ds = pydicom.dcmread(path + str(i))
    volume.append(ds.pixel_array*128)
    marker = np.zeros((ds.pixel_array.shape[0], ds.pixel_array.shape[1]), dtype=np.uint8)
    volume_marked.append(marker)

#dicom to png using opencv
for i in range(0, nb_images):
    #0 is 0 and 1 is 255 but above one is 125
    volume[i] = cv.normalize(volume[i], None, 0, 255, cv.NORM_MINMAX)
    #erode
    kernel = np.ones((5,5), np.uint8)
    #seen as uint
    volume[i] = volume[i].astype(np.uint8)

    volume[i] = cv.erode(volume[i], kernel, iterations=1)
    #cv.imwrite(path + str(i) + ".png", volume[i])


for set in range(nb_batch):
    for id in range(nb_marker):

        rand_x, rand_y, rand_z = generate_random_position(volume)

        while volume[rand_z][rand_x, rand_y] != 255:
            rand_x, rand_y, rand_z = generate_random_position(volume)
            if volume[rand_z][rand_x, rand_y] == 255:
                cv.circle(volume_marked[rand_z], (rand_x,rand_y), 10, (255, 255, 255), -1)
                #save as an image
                cv.imwrite("marker_" + str(set) + "_" + str(id) + "_" + str(rand_z) + ".png", volume_marked[rand_z])
                print(rand_x, rand_y, rand_z)
                print("found")
                rand_pos_add3D[set][id] = (rand_x, rand_y, rand_z)

#compute 1D coordinates for every marker in every batch
for set in range(nb_batch):
    for id in range(nb_marker):
        x, y, z = rand_pos_add3D[set][id]
        rand_pos_add1D[set][id] = np.zeros(121, dtype=int)
        cpt = 0
        #make a 2D circle around the marker of radius 5
        for i in range(-5, 6):
            for j in range(-5, 6):
                if x + i >= 0 and x + i < volume[0].shape[0] and y + j >= 0 and y + j < volume[0].shape[1]:
                    rand_pos_add1D[set][id][cpt] = get_1D(x + i, y + j, z, volume)
                    cpt += 1

#save 1D in a text file
for set in range(nb_batch):
    file = open("marker_" + str(set) + ".txt", "w")
    for id in range(nb_marker):
        file.write(str(rand_pos_add1D[set][id]) + "\n")
    file.close()