//
// Created by lebon on 13/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_IMAGEMANAGER_H
#define INCREMENTAL_WATERSHED_IMAGEMANAGER_H
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "graph.h"

class imageManager {
private:
    std::string pathFile_;
    int width_;
    int height_;
    cv::Mat image_;
    graph graph_;
public:
    imageManager(std::string path,cv::Mat image);
    void toGraph();
    graph& getGraph() {return this->graph_;}
    ~imageManager();
};


#endif //INCREMENTAL_WATERSHED_IMAGEMANAGER_H
