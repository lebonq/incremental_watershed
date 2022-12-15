//
// Created by lebon on 12/05/2022.
//
#include "dataStructures/graph.h"
#include "imageManager.h"
#include "meyer_ws.h"
#include "../markers.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "../matplotlibcpp.h"
#include <fstream>

namespace plt = matplotlibcpp;

int handleError(int status, const char *func_name,
                const char *err_msg, const char *file_name,
                int line, void *userdata) {
    //Do nothing -- will suppress console output
    return 0;   //Return value is not used
}

int main(int argc, char *argv[]) {
    cv::redirectError(handleError);//avoid printing opencv error
    //cv::redirectError(nullptr);//restor printing opencv error

    std::vector<double> time_meyer;
    std::vector<double> time_IW;

    const int nb_bench = 10;

    for (int i = 0; i < 73; ++i) {
        double t_mean = 0;
        for (int j = 0; j < nb_bench; ++j) {
            auto meyer = meyer_ws("holiday_data/plant.jpeg");
            meyer.pre_process(i);
            double t = (double) cv::getTickCount();
            meyer.ws();
            t = (double) cv::getTickCount() - t;
            t_mean += t * 1000. / cv::getTickFrequency();
            //meyer.show();
        }
        printf("%g\n", i, t_mean / nb_bench);
        time_meyer.push_back(t_mean/nb_bench);
    }

    auto testim = cv::imread("holiday_data/plant.jpeg",cv::IMREAD_GRAYSCALE);


    for (int i = 0; i < 73; ++i) {
        double t_mean = 0;
        bool remove;
        for (int j = 0; j < nb_bench; ++j) {
            imageManager testImg = imageManager("holiday_data/plant.jpeg",testim);
            testImg.init();

            if(i > 0){
                std::string name = "holiday_data/plant.jpegdata/step_";
                name.append(std::to_string(i-1));
                std::vector<int> values_img;
                algorithms::get_tab_from_image(name,values_img);
                testImg.addMarkers(values_img.data(),values_img.size(),false);
            }

            std::vector<int> values;
            std::string name = "holiday_data/plant.jpegdata/step_";
            name.append(std::to_string(i));
            remove = algorithms::get_vector_from_txt(name,values);

            double t;
            if(remove == true){
                t = (double) cv::getTickCount();
                testImg.removeMarkers(values.data(),values.size(),false);
                t = (double) cv::getTickCount() - t;
            }
            else{
                t = (double) cv::getTickCount();
                testImg.addMarkers(values.data(),values.size(),false);
                t = (double) cv::getTickCount() - t;
            }
            t_mean+= t * 1000. / cv::getTickFrequency();

        }
        time_IW.push_back(t_mean/nb_bench);
        //algorithms::showSegmentation(testImg,"testImg.png");
        if(remove){
            printf("%g\n", i, t_mean/nb_bench);
        }
        else{
            printf("%g\n", i, t_mean/nb_bench);
        }

    }

    plt::named_plot("IWS", time_IW);
    plt::named_plot("OpenCV WS", time_meyer);
    plt::title("Average computational time in ms");
    plt::show();


    /*//Store txt marker
    std::vector<int> values;
    std::vector<int> values_img;
    int nbmarkers = 0;

    algorithms::get_tab_from_image("holiday_data/coral.jpgdata/step_3",values_img);
    bool remove = algorithms::get_vector_from_txt("holiday_data/coral.jpgdata/step_4",values);

    auto testim = cv::imread("holiday_data/coral.jpg",cv::IMREAD_GRAYSCALE);

    imageManager testImg = imageManager("holiday_data/coral.jpg",testim);
    testImg.init();

    testImg.addMarkers(values_img.data(),values_img.size(),false);
    algorithms::showSegmentation(testImg,"testImg.png");
    if(remove == true){
        testImg.removeMarkers(values.data(),values.size(),false);
    }
    else{
        testImg.addMarkers(values.data(),values.size(),false);
    }*/
    //algorithms::showSegmentation(testImg,"testImg.png");

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