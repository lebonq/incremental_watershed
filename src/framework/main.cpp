//
// Created by lebon on 12/05/2022.
//
#include "dataStructures/graph.h"
#include "imageManager.h"
#include "data.h"
#include "../markers.h"
#include <opencv2/highgui/highgui.hpp>


int main(int argc, char* argv[])
{

    for(int i = 0; i < 1; i++)
    {
        auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);
        //todo declare inside the imageManager
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();

        std::cout << "Add markers 1" << std::endl;
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);

        std::cout << "Show image #1" << std::endl;
        algorithms::showSegmentationMP(testImg,"testImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 2" << std::endl;
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,true);

        std::cout << "Show image #2" << std::endl;
        algorithms::showSegmentationMP(testImg,"testImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 3" << std::endl;
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,true);

        std::cout << "Show image #3" << std::endl;
        algorithms::showSegmentationMP(testImg,"testImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 4" << std::endl;
        testImg.removeMarkers(markers4Cell2,nbMarkers4Cell2,true);

        std::cout << "Show image #4" << std::endl;
        algorithms::showSegmentationMP(testImg,"testImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 5" << std::endl;
        testImg.addMarkers(markers5Cell2,nbMarkers5Cell2,true);

        std::cout << "Show image #5" << std::endl;
        algorithms::showSegmentationMP(testImg,"testImg" + std::to_string(i) + ".png");

    }

    return 0;

}