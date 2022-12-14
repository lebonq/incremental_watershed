//
// Created by lebonq on 18/11/22.
//

#include "meyer_ws.h"

meyer_ws::meyer_ws(std::string sName) {
    this->file_name = sName;
    this->img0 = cv::imread(this->file_name, 1);
    if (this->img0.empty()) {
        std::cout << "Couldn't open image ";
        return;
    }

    cvtColor(this->img0, this->markerMask, cv::COLOR_BGR2GRAY);
    cvtColor(this->markerMask, this->imgGray, cv::COLOR_GRAY2BGR);

    this->markerMask = cv::Scalar::all(0);

}

void meyer_ws::pre_process(int id) {
    int i, j;

    try {
        this->markerMask = cv::imread(this->file_name + "data/step_" + std::to_string(id) + "_add.png", CV_8UC1);
        if (this->markerMask.empty()) {
            this->markerMask = cv::imread(this->file_name + +"data/step_" + std::to_string(id) + "_remove.png",
                                          CV_8UC1);
        }
    } catch (cv::Exception e) {
        std::cout << "not add for step " << std::to_string(id) << std::endl;
        try {
            this->markerMask = cv::imread(this->file_name + +"data/step_" + std::to_string(id) + "_remove.png",
                                          CV_8UC1);
        } catch (cv::Exception ee) {
            std::cout << ee.msg << std::endl;
            std::exit(ee.code);
        }
    }

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    findContours(this->markerMask, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty())
        return;
    this->markers = cv::Mat(this->markerMask.size(), CV_32S);
    this->markers = cv::Scalar::all(0);
    int idx = 0;
    for (; idx >= 0; idx = hierarchy[idx][0], this->compCount++)
        drawContours(this->markers, contours, idx, cv::Scalar::all(this->compCount + 1), -1, 8, hierarchy, INT_MAX);
    if (this->compCount == 0)
        return;
    this->colorTab;
    for (i = 0; i < this->compCount; i++) {
        int b = cv::theRNG().uniform(0, 255);
        int g = cv::theRNG().uniform(0, 255);
        int r = cv::theRNG().uniform(0, 255);
        this->colorTab.push_back(cv::Vec3b((uchar) b, (uchar) g, (uchar) r));
    }

}

/**
 * Ref https://github.com/opencv/opencv/blob/4.x/modules/imgproc/src/segmentation.cpp line 88
 */
void meyer_ws::ws() {
    watershed(this->img0, this->markers);
}

void meyer_ws::show() {
    int i, j;
    cv::Mat wshed(this->markers.size(), CV_8UC3);
    // paint the watershed image
    //There is a BETTER way to show segments by markers ID instead of a double doubled for loop but it work just fine
    for (i = 0; i < this->markers.rows; i++)
        for (j = 0; j < this->markers.cols; j++) {
            int index = this->markers.at<int>(i, j);
            int index_color = this->markerMask.at<uchar>(i, j);
            if (index_color == 1) {
                this->colorTab[index - 1] = cv::Vec3b((uchar) 0, (uchar) 255, (uchar) 0);
            }
            if (index_color == 2) {
                this->colorTab[index - 1] = cv::Vec3b((uchar) 0, (uchar) 0, (uchar) 255);
            }
        }

    for (i = 0; i < this->markers.rows; i++)
        for (j = 0; j < this->markers.cols; j++) {
            int index = this->markers.at<int>(i, j);
            if (index == -1)
                wshed.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
            else if (index <= 0 || index > this->compCount)
                wshed.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            else
                wshed.at<cv::Vec3b>(i, j) = colorTab[index - 1];
        }

    wshed = wshed * 0.5 + imgGray * 0.5;
    cv::namedWindow("Meyer WS", 1);
    cv::imshow("watershed transform", wshed);
    cv::waitKey(0);
}

meyer_ws::~meyer_ws() {
    colorTab.clear();
}

