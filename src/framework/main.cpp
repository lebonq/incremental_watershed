//
// Created by lebon on 12/05/2022.
//
#include "dataStructures/graph.h"
#include "imageManager.h"
#include "meyer_ws.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "../matplotlibcpp.h"
#include <fstream>
#include <filesystem>
#include <opencv2/core/hal/interface.h>

namespace plt = matplotlibcpp;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    std::vector<double> time_meyer;
    std::vector<double> time_IFT;
    std::vector<double> time_IW;
    std::vector<double> time_init_IW;
    std::vector<double> time_NIW;

    const int nb_bench = 1;
    const int nb_images = 4;

    std::vector<std::string> images_name;

    std::string directoryPath = "data/";

    for (const auto &entry: fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            if (entry.path().extension().string() == ".jpg" || entry.path().extension().string() == ".jpeg") {
                images_name.push_back(entry.path().filename().string());
            }
        }
    }

    //sort images name
    std::sort(images_name.begin(), images_name.end());

    for (auto image: images_name){
        std::cout << image << std::endl;
    }

    int idx_bench = 0;

    for (auto image: images_name){
        if(idx_bench <= 0){
            idx_bench++;
            continue;
        }
        std::string image_path = "data/" + image;

        //=============================== Compute number of pixel======================================================

        cv::Mat img_4px = cv::imread(image_path, cv::IMREAD_COLOR);
        double nb_pixel = img_4px.rows * img_4px.cols * 1.0;
        std::cout << "Number of pixel: " << nb_pixel << std::endl;

        /*

        //===================== Generate data for DIFT ========================================
        std::cout << "Generate data for DIFT benchmark" << std::endl;
        auto img_gray_pgm = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
        imwrite(image_path + ".pgm", img_gray_pgm);
        cv::Mat gradient_pgm;
        cv::Mat grad_x, grad_y;
        cv::Mat abs_grad_x, abs_grad_y;
        Sobel(img_gray_pgm, grad_x, CV_16S, 1, 0);
        Sobel(img_gray_pgm, grad_y, CV_16S, 0, 1);

        convertScaleAbs(grad_x, abs_grad_x);
        convertScaleAbs(grad_y, abs_grad_y);
        addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradient_pgm);
        imwrite(image_path + ".gradient.pgm", gradient_pgm);

        for (int i = 0; i < nb_images; ++i) {
            std::string name = image_path + "data/step_";
            algorithms::get_DIFT_seed_from_image(name, i);
        }

        //=========================== OPENCV IMPLEMENTATION ========================================
        std::cout << "Benchmarking OpenCV implementation" << std::endl;
        for (int i = 0; i < nb_images; ++i) {
            double t_mean = 0;
            for (int j = 0; j < nb_bench; ++j) {
                auto meyer = meyer_ws(image_path);
                meyer.pre_process(i);
                double t = (double) cv::getTickCount();
                meyer.ws();
                t = (double) cv::getTickCount() - t;
                t_mean += t / cv::getTickFrequency();
                //meyer.show();
            }
            printf("Set #%d : %g ns/px\n", i, (t_mean * 1e9 / nb_bench)/nb_pixel);
            printf("Set #%d : %g ms\n", i, (t_mean * 1000. / nb_bench));
            time_meyer.push_back((t_mean *1e9 / nb_bench)/nb_pixel);
        }*/

        auto testim = cv::imread(image_path, cv::IMREAD_GRAYSCALE);

        std::cout << "Computing  for " << image << std::endl;

        //=========================== IWS IMPLEMENTATION ========================================
        std::cout << "Benchmarking IWS implementation" << std::endl;

        double t_mean = 0;
        double init_mean = 0;
        bool remove;
        double t;
        for (int i = 0; i < nb_images; ++i) {
            t_mean = 0;
            for (int j = 0; j < nb_bench; ++j) {
                imageManager testImg = imageManager(image_path, testim);
                t = (double) cv::getTickCount();
                testImg.init();
                t = (double) (cv::getTickCount() - t);
                init_mean += t / cv::getTickFrequency();

                if (i > 0) {
                    std::string name = image_path + "data/step_";
                    name.append(std::to_string(i - 1));
                    std::vector<int> values_img;
                    algorithms::get_tab_from_image(name, values_img);
                    testImg.addMarkers(values_img.data(), values_img.size());
                }

                std::vector<int> values;
                std::string name = image_path + "data/step_";
                name.append(std::to_string(i));
                remove = algorithms::get_vector_from_txt(name, values);

                if (remove == true) {
                    t = (double) cv::getTickCount();
                    testImg.removeMarkers(values.data(), values.size());
                    t = (double) cv::getTickCount() - t;
                } else {
                    t = (double) cv::getTickCount();
                    testImg.addMarkers(values.data(), values.size());
                    t = (double) cv::getTickCount() - t;
                }
                t_mean += t / cv::getTickFrequency();
            }
            time_IW.push_back((t_mean *1e9/ nb_bench)/nb_pixel);
                            //We compute en record the init time just 1 time
            printf("Init : %g ns/px\n", (init_mean * 1e9 / nb_bench) / nb_pixel);
            time_init_IW.push_back((init_mean * 1e9 / nb_bench) / nb_pixel);
            printf("Init : %g ms\n", i, (init_mean *1000. / nb_bench));
            if (remove) {
                printf("Remove set #%d : %g ns/px\n", i, (t_mean*1e9 / nb_bench)/nb_pixel);
                printf("Remove set #%d : %g ms\n", i, (t_mean *1000./ nb_bench));
            } else {
                printf("Add set #%d : %g ns/px\n", i, (t_mean *1e9 / nb_bench)/nb_pixel);
                printf("Add set #%d : %g ms\n", i, (t_mean *1000. / nb_bench));
            }

        }



        /*
        //=========================== NIWS IMPLEMENTATION ========================================
        std::cout << "Benchmarking NIWS implementation" << std::endl;

        t_mean = 0;
        init_mean = 0;
        remove;
        t;
        for (int i = 0; i < nb_images; ++i) {
            t_mean = 0;
            for (int j = 0; j < nb_bench; ++j) {
                imageManager testImg = imageManager(image_path, testim);
                t = (double) cv::getTickCount();
                testImg.init();
                t = (double) cv::getTickCount() - t;

                std::vector<int> values;
                std::string name = image_path + "data/step_";
                name.append(std::to_string(i));
                algorithms::get_tab_from_image(name, values);

                t = (double) cv::getTickCount();
                testImg.addMarkers(values.data(), values.size());
                t = (double) cv::getTickCount() - t;
                t_mean += t / cv::getTickFrequency();
                //algorithms::showSegmentation(testImg,"testImg.png");
            }
            time_NIW.push_back((t_mean *1e9/ nb_bench)/nb_pixel);
            printf("Set #%d : %g ns/px\n", i, (t_mean * 1e9 / nb_bench)/nb_pixel);
            printf("Set #%d : %g ms\n", i, (t_mean *1000. / nb_bench));

        }

        algorithms::vector_to_csv(time_meyer, std::to_string(idx_bench) + "_time_meyer.csv");
        time_meyer.clear();
        algorithms::vector_to_csv(time_IW, std::to_string(idx_bench) + "_time_IW.csv");
        time_IW.clear();
        algorithms::vector_to_csv(time_NIW, std::to_string(idx_bench) + "_time_NIW.csv");
        time_NIW.clear();
*/
        idx_bench++;

        //plt::named_plot("IWS", time_IW);
        //plt::named_plot("NIWS", time_NIW);
        //plt::named_plot("OpenCV", time_meyer);
        //plt::legend();
        //plt::title("Average computational time in ms");
        //plt::show();

        //Illustration of every step/interaction

        for (int img_n = 0; img_n < nb_images; img_n++) {
            auto imgGray = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
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
                img_backend.addMarkers(values_img.data(), values_img.size());
            }

            std::vector<int> values;
            std::string name = image_path + "data/step_";
            name.append(std::to_string(img_n));
            bool remove = algorithms::get_vector_from_txt(name, values);

            if (remove == true) {
                img_backend.removeMarkers(values.data(), values.size());
            } else {
                img_backend.addMarkers(values.data(), values.size());
            }

            cv::Mat marker_image;

            try {
                marker_image = cv::imread(image_path + "data/step_" + std::to_string((img_n)) + "_add.png", CV_8UC1);
                if (marker_image.empty()) {
                    marker_image = cv::imread(image_path + "data/step_" + std::to_string((img_n)) + "_remove.png",
                                              CV_8UC1);
                }
            } catch (cv::Exception e) { ;
                try {
                    marker_image = cv::imread(image_path + "data/step_" + std::to_string((img_n)) + "_remove.png",
                                              CV_8UC1);
                } catch (cv::Exception e) {
                    std::cout << e.msg << std::endl;
                    std::exit(e.code);
                }
            }


            int cpt = 0, seed, color;
            for (int i = 0; i < marker_image.rows; i++)
            {
                for (int j = 0; j < marker_image.cols; j++) {
                    color = (int) marker_image.at<unsigned char>(i, j);
                    if (color != 0) {
                        color_tab[img_backend.segments_[cpt]] = color;
                    }
                    cpt++;
                }
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
            for (auto contour: contours) {
                cv::drawContours(imgColor, contours, -1, cv::Scalar(0, 0, 0, 0), 8);
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
            imgColor.copyTo(cpy_over);

            cpy_over = wshed * 0.5 + imgColor * 0.5;

            imwrite(image_path + "history_test/seg_step" + std::to_string(img_n) + ".png",wshed);
            imwrite(image_path + "illustration/marker_step" + std::to_string(img_n) + ".jpg", imgColor);

            free(color_tab);
        }
        exit(42);
    }

    algorithms::vector_to_csv(time_init_IW, "time_init_IW.csv");
    time_init_IW.clear();
}