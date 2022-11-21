//
// Created by lebonq on 18/11/22.
//

#include "meyer_ws.h"

meyer_ws::meyer_ws(std::string sName) {
    this->file_name = sName;
    this->img0 = cv::imread(this->file_name, 1);
    if( this->img0.empty() )
    {
        std::cout << "Couldn't open image ";
        return;
    }

    cvtColor(this->img0, this->markerMask, cv::COLOR_BGR2GRAY);
    cvtColor(this->markerMask, this->imgGray, cv::COLOR_GRAY2BGR);

    this->markerMask = cv::Scalar::all(0);

}

void meyer_ws::pre_process(int ws_count) {
    int i, j;

    switch (ws_count) {
        case 0:
            this->markerMask = cv::imread("Add_0.txtmeyer.png",CV_8UC1);
            ws_count++;
            break;

        case 1:
            cv::Mat mar0 = cv::imread("Add_0.txtmeyer.png",CV_8UC1);
            cv::Mat mar1 = cv::imread("Add_1.txtmeyer.png",CV_8UC1);
            this->markerMask = mar0 + mar1;
            break;
    }

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    findContours(this->markerMask, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
    if( contours.empty() )
        return;
    this->markers = cv::Mat(this->markerMask.size(), CV_32S);
    this->markers = cv::Scalar::all(0);
    int idx = 0;
    for( ; idx >= 0; idx = hierarchy[idx][0], this->compCount++ )
        drawContours(this->markers, contours, idx, cv::Scalar::all(this->compCount+1), -1, 8, hierarchy, INT_MAX);
    if( this->compCount == 0 )
        return;
    this->colorTab;
    for( i = 0; i < this->compCount; i++ )
    {
        int b = cv::theRNG().uniform(0, 255);
        int g = cv::theRNG().uniform(0, 255);
        int r = cv::theRNG().uniform(0, 255);
        this->colorTab.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
}

void meyer_ws::ws() {
    watershed( this->img0, this->markers );
}

void meyer_ws::show() {
    int i,j;
    cv::Mat wshed(this->markers.size(), CV_8UC3);
    // paint the watershed image
    for( i = 0; i < this->markers.rows; i++ )
        for( j = 0; j < this->markers.cols; j++ )
        {
            int index = this->markers.at<int>(i,j);
            if( index == -1 )
                wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255);
            else if( index <= 0 || index > this->compCount )
                wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
            else
                wshed.at<cv::Vec3b>(i,j) = colorTab[index - 1];
        }
    wshed = wshed*0.5 + imgGray*0.5;
    cv::namedWindow( "Meyer WS", 1 );
    cv::imshow( "watershed transform", wshed );
    cv::waitKey(0);
}

meyer_ws::~meyer_ws() {
    colorTab.clear();
}

