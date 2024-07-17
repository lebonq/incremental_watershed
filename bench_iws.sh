#!/bin/bash

/home/lebonqu/.local/share/JetBrains/Toolbox/apps/clion/bin/cmake/linux/x64/bin/cmake --build /home/lebonqu/CLionProjects/incremental_watershed/cmake-build-release --target incremental_watershed_framework-3d-iws -j 46
# Build line is to update accoridng to your dev environment
#num_cpu=(1 2 3 5 7 9 11 13 15 17 19 21 23 25 27 29 31 33 35 37 39 41 43 45 47 48)
##
#num_elements=(0 5 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95 100 105 110 115 120 125 130 135 140 145 150 155 160 165 170 175 180 185 190 195 200 205 210 215 220 225 230 235 240 245 250 255 260 265 270 275 280 285 290 295 300 325 350 375 400 425 450 475 500 550 600 650 700 750 800 850 900 950 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100 2200 2300 2400 2500 2600 2700 2800 2900 3000)
#
#for ((i=0; i<${#num_elements[@]}; i++));
#do
#  for ((j=0; j<${#num_cpu[@]}; j+=1));
#  do
#    /home/lebonqu/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_2/patient_2_png data_3d/patient_2/patient_2_liver_markers 30 3 patient2 ${num_elements[i]} ${num_cpu[j]}
#  done
#done
/home/lebonqu/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_2/patient_2_png data_3d/patient_2/patient_2_liver_markers 30 5 patient2 165 11
/home/lebonqu/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_7/patient_7_png data_3d/patient_7/patient_7_liver_markers 30 5 patient7 165 11
/home/lebonqu/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_12/patient_12_png data_3d/patient_12/patient_12_liver_markers 30 5 patient12 165 11
/home/lebonqu/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_19/patient_19_png data_3d/patient_19/patient_19_liver_markers 30 5 patient19 165 11
/home/lebonqu/CLionProjects/incremental_watershed/cmake-build-release/src/framework/incremental_watershed_framework-3d-iws data_3d/patient_20/patient_20_png data_3d/patient_20/patient_20_liver_markers 30 5 patient20 165 11
