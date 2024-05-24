


names = ["patient_2","patient_7","patient_12","patient_19","patient_20"]
sizes = [172, 151, 260, 124, 225]

import numpy as np

# Load the marker file (assuming each marker ID is on a separate line)
for i in range(0, len(names)):
    marker_file = names[i] + "/" + names[i] + "_liver_markers/" + "markers_background_0.txt"
    with open(marker_file, 'r') as f:
        markers = [int(line.strip()) for line in f.readlines()]

