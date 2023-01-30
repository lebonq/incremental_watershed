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

    const int nb_bench = 50;
    const int nb_images = 29;
    const std::string image_path = "holiday_data/dwarf.jpg";
/*
    // OPENCV IMPLEMENTATION
    for (int i = 0; i < nb_images; ++i) {
        double t_mean = 0;
        for (int j = 0; j < nb_bench; ++j) {
            auto meyer = meyer_ws(image_path);
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

    auto testim = cv::imread(image_path,cv::IMREAD_GRAYSCALE);

    double t_mean = 0;
    bool remove;
    double t;
    for (int i = 0; i < nb_images; ++i) {
        t_mean = 0;
        for (int j = 0; j < nb_bench; ++j) {
            imageManager testImg = imageManager(image_path,testim);
            t = (double) cv::getTickCount();
            testImg.init();
            t = (double) cv::getTickCount() - t;

            //For first marker add we count the time to init the image
            if(i == 0){
               std::cout << "Init" << t * 1000. / cv::getTickFrequency() << std::endl;
            }

            if(i > 0){
                std::string name = image_path + "data/step_";
                name.append(std::to_string(i-1));
                std::vector<int> values_img;
                algorithms::get_tab_from_image(name,values_img);
                testImg.addMarkers(values_img.data(),values_img.size(),false);
            }

            std::vector<int> values;
            std::string name = image_path +  "data/step_";
            name.append(std::to_string(i));
            remove = algorithms::get_vector_from_txt(name,values);

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

    algorithms::vector_to_csv(time_meyer,"time_meyer.csv");
    algorithms::vector_to_csv(time_IW,"time_IW.csv");

    plt::named_plot("IWS", time_IW);
    plt::named_plot("OpenCV WS", time_meyer);
    plt::title("Average computational time in ms");
    plt::show()*/

    //Illustration



    for(int img_n =0; img_n < nb_images;img_n++ ){
        auto imgGray = cv::imread(image_path,cv::IMREAD_GRAYSCALE);
        auto imgColor = cv::imread(image_path);
        int *color_tab;
        cv::Mat markerMask_rzs, markerMask_prev_rzs;
        imageManager img_backend = imageManager(image_path, imgGray);
        img_backend.init();
        color_tab = (int *) malloc(sizeof(int) * img_backend.getGraph().getNbVertex());

        if (img_n > 0) {
            std::string name = image_path + "data/step_";
            name.append(std::to_string(img_n - 1));
            std::vector<int> values_img;
            algorithms::get_tab_from_image(name, values_img);
            img_backend.addMarkers(values_img.data(), values_img.size(), false);
        }

        std::vector<int> values;
        std::string name = image_path + "data/step_";
        name.append(std::to_string(img_n));
        bool remove = algorithms::get_vector_from_txt(name, values);

        if (remove == true) {
            img_backend.removeMarkers(values.data(), values.size(), false);
        } else {
            img_backend.addMarkers(values.data(), values.size(), false);
        }

        cv::Mat marker_image;

        try {
            marker_image = cv::imread(image_path + "data/step_" +std::to_string((img_n))+ "_add.png", CV_8UC1);
            if (marker_image.empty()) {
                marker_image = cv::imread(image_path + "data/step_" +std::to_string((img_n))+ "_remove.png",
                                          CV_8UC1);
            }
        } catch (cv::Exception e) { ;
            try {
                marker_image = cv::imread(image_path + "data/step_" +std::to_string((img_n))+ "_remove.png",
                                          CV_8UC1);
            } catch (cv::Exception e) {
                std::cout << e.msg << std::endl;
                std::exit(e.code);
            }
        }


        int cpt = 0, seed, color;
        for (int i = 0; i < marker_image.rows; i++)
            for (int j = 0; j < marker_image.cols; j++) {
                color = (int) marker_image.at<unsigned char>(i, j);
                if (color != 0) {
                    color_tab[img_backend.segments_[cpt]] = color;
                }
                cpt++;
            }

        cv::Mat wshed(marker_image.size(), CV_8UC3);
        cpt = 0;
        for (int y = 0; y < img_backend.getHeight(); y++) {
            for (int x = 0; x < img_backend.getWidth(); x++) {
                seed = color_tab[img_backend.segments_[cpt]];
                if (seed == 1) {
                    wshed.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 255, 0);
                } else {
                    wshed.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
                }
                cpt++;
            }
        }

        cv::Mat cont(wshed.size(), CV_8UC4);
        cont = cv::Scalar(255, 255, 255, 0);

        cv::Mat binary;
        cv::cvtColor(wshed, binary, cv::COLOR_BGR2GRAY);
        cv::threshold(binary, binary, 128, 255, cv::THRESH_BINARY);
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        for (auto contour : contours) {
            cv::drawContours(imgColor, contours, -1, cv::Scalar(0, 0, 0,255), 8);
        }


      cv::Mat imgRGBA(marker_image.size(), CV_8UC4);
      for (int i = 0; i < marker_image.rows; i++) {
        for (int j = 0; j < marker_image.cols; j++) {
          int value = marker_image.at<uchar>(i, j);
          if (value == 1) {
            imgColor.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 255, 0);
          } else if (value == 2) {
            imgColor.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
          } else {

          }
        }
      }

        //Create an overlay over th WSHED transform
        cv::Mat cpy_over;
        //overlay.copyTo(cpy_over);

        //cpy_over = wshed * 0.5 + overlay * 0.5;

        //imwrite(filename + "history/seg_step" + std::to_string(nb_change) + ".png",wshed);
        imwrite(image_path + "illustration/marker_step" + std::to_string(img_n) + ".jpg", imgColor);

        free(color_tab);
    }
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