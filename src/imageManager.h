//
// Created by lebon on 13/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_IMAGEMANAGER_H
#define INCREMENTAL_WATERSHED_IMAGEMANAGER_H
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "dataStructures/graph.h"
#include "dataStructures/QEBT.h"
#include "algorithms.h"

class imageManager {
private:
    std::string pathFile_; // Path of the image
    int width_; // Width of the image
    int height_; // Height of the image
    cv::Mat image_; // Opencv Data of the image
    graph graph_; // Graph representation of the image
    QEBT qbet_; // Hierarchy

    bool isReady_;
    void toGraph();// Convert image to a graph TODO : Add a way to choose cost function
public:
    imageManager(std::string path,cv::Mat image);
    graph& getGraph() {return this->graph_;} // Getter of the graph
    void init(); // Initialize everything to be ready for add and remove markers
    void addMarkers();
    void removeMarkers();
    ~imageManager();
};

//Pour marks on fait marks[up-nbVertex] ca nous permet de reduire la taille de l'array


#endif //INCREMENTAL_WATERSHED_IMAGEMANAGER_H
