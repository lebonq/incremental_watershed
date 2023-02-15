//
// Created by lebonq on 18/11/22.
//

#ifndef INCREMENTAL_WATERSHED_MEYER_WS_H
#define INCREMENTAL_WATERSHED_MEYER_WS_H

#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <cstdio>
#include <iostream>

class meyer_ws {
private:
    std::string file_name;
    cv::Mat img0;
    cv::Mat imgGray;
    cv::Mat markerMask;
    std::vector<cv::Vec3b> colorTab;
    cv::Mat markers;
    int compCount = 0;

public:
    meyer_ws(std::string);
    ~meyer_ws();
    void pre_process(int ws_count);
    void ws();
    void show();

};


#endif //INCREMENTAL_WATERSHED_MEYER_WS_H
