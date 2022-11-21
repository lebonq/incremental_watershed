//
// Created by lebon on 12/05/2022.
//
#include "dataStructures/graph.h"
#include "imageManager.h"
#include "meyer_ws.h"
#include "../markers.h"
#include <opencv2/highgui/highgui.hpp>


int main(int argc, char* argv[])
{
    auto meyer = meyer_ws("holiday_data/tower.jpg");
    meyer.pre_process(0);
    meyer.ws();
    meyer.show();

    /*
    for(int i = 0; i < 1; i++)
    {
        auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

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

    for(int i = 0; i< 1; i++){
        auto coinImg = cv::imread("coins.gray.png",cv::IMREAD_GRAYSCALE);
        imageManager coinImgManager = imageManager("coins.gray.png",coinImg);
        coinImgManager.init();

        std::cout << "Add markers 1" << std::endl;
        coinImgManager.addMarkers(markerCoin1,nbmarkerCoin1,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 2" << std::endl;
        coinImgManager.addMarkers(markerCoin2,nbmarkerCoin2,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 3" << std::endl;
        coinImgManager.addMarkers(markerCoin3,nbmarkerCoin3,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 4" << std::endl;
        coinImgManager.addMarkers(markerCoin4,nbmarkerCoin4,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 5" << std::endl;
        coinImgManager.addMarkers(markerCoin5,nbmarkerCoin5,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 6" << std::endl;
        coinImgManager.addMarkers(markerCoin6,nbmarkerCoin6,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 7" << std::endl;
        coinImgManager.addMarkers(markerCoin7,nbmarkerCoin7,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 8" << std::endl;
        coinImgManager.addMarkers(markerCoin8,nbmarkerCoin8,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 9" << std::endl;
        coinImgManager.addMarkers(markerCoin9,nbmarkerCoin9,true);

        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 10" << std::endl;
        coinImgManager.addMarkers(markerCoin10,nbmarkerCoin10,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 11" << std::endl;
        coinImgManager.addMarkers(markerCoin11,nbmarkerCoin11,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 12" << std::endl;
        coinImgManager.addMarkers(markerCoin12,nbmarkerCoin12,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 13" << std::endl;
        coinImgManager.addMarkers(markerCoin13,nbmarkerCoin13,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 14" << std::endl;
        coinImgManager.addMarkers(markerCoin14,nbmarkerCoin14,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Add markers 15" << std::endl;
        coinImgManager.addMarkers(markerCoin15,nbmarkerCoin15,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 15" << std::endl;
        coinImgManager.removeMarkers(markerCoin15,nbmarkerCoin15,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 14" << std::endl;
        coinImgManager.removeMarkers(markerCoin14,nbmarkerCoin14,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 13" << std::endl;
        coinImgManager.removeMarkers(markerCoin13,nbmarkerCoin13,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 12" << std::endl;
        coinImgManager.removeMarkers(markerCoin12,nbmarkerCoin12,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 11" << std::endl;
        coinImgManager.removeMarkers(markerCoin11,nbmarkerCoin11,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 10" << std::endl;
        coinImgManager.removeMarkers(markerCoin10,nbmarkerCoin10,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 9" << std::endl;
        coinImgManager.removeMarkers(markerCoin9,nbmarkerCoin9,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 8" << std::endl;
        coinImgManager.removeMarkers(markerCoin8,nbmarkerCoin8,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 7" << std::endl;
        coinImgManager.removeMarkers(markerCoin7,nbmarkerCoin7,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 6" << std::endl;
        coinImgManager.removeMarkers(markerCoin6,nbmarkerCoin6,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 5" << std::endl;
        coinImgManager.removeMarkers(markerCoin5,nbmarkerCoin5,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 4" << std::endl;
        coinImgManager.removeMarkers(markerCoin4,nbmarkerCoin4,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 3" << std::endl;
        coinImgManager.removeMarkers(markerCoin3,nbmarkerCoin3,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 2" << std::endl;
        coinImgManager.removeMarkers(markerCoin2,nbmarkerCoin2,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");

        std::cout << "Remove markers 1" << std::endl;
        coinImgManager.removeMarkers(markerCoin1,nbmarkerCoin1,true);
        algorithms::showSegmentationMP(coinImgManager,"coinImg" + std::to_string(i) + ".png");
    }*/
}