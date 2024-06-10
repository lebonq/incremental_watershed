#!/bin/bash

/home/lebonq/.local/share/JetBrains/Toolbox/apps/clion-2/bin/cmake/linux/x64/bin/cmake --build /home/lebonq/CLionProjects/incremental_watershed/cmake-build-release --target incremental_watershed_framework-3d-iws -j 18
# Build line is to update accoridng to your dev environment
num_cpu=(1 2 5 7 10 12 14)

num_elements=(0 25 50 75 100 125 150 175 200 225 250 275 300 325 350 375 400 425 450 475 500 525 550 575 600 625 650 675 700 1000 1250 1500 1750 2000 2250 2500 2750 3000)

for ((j=1; j<20; j++));
do
  for ((i=0; i<${#num_elements[@]}; i++));
  do
    /home/lebonq/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_2/patient_2_png data_3d/patient_2/patient_2_liver_markers 30 2 patient2 ${num_elements[i]} ${j}
  done
done

#/home/lebonq/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_7/patient_7_png data_3d/patient_7/patient_7_liver_markers 30 1 patient7
#/home/lebonq/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_12/patient_12_png data_3d/patient_12/patient_12_liver_markers 30 1 patient12
#/home/lebonq/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_19/patient_19_png data_3d/patient_19/patient_19_liver_markers 30 1 patient19
#/home/lebonq/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_20/patient_20_png data_3d/patient_20/patient_20_liver_markers 30 1 patient20
