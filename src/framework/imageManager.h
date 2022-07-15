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
#include "dataStructures/partition.h"

class imageManager {
private:
    //Data structures for "classic" watershed cuts
    std::string pathFile_; // Path of the image
    int width_; // Width of the image
    int height_; // Height of the image
    cv::Mat image_; // Opencv Data of the image
    graph graph_; // Graph representation of the image
    QEBT qbet_; // Hierarchy
    void toGraph();// Convert image to a graph TODO : Add a way to choose cost function

    int nbVertex_; // Number of vertex in the graph for better performance in getEdge(n)
public:
    //Data structures needed for add/remove not private because they exist to be modified
    int* segments_; //Associate to vertex index a segment ID
    int* marks_; //Keep track
    int* sizePart_;
    bool* ws_;
    bool* mstEdit_;

    int tagCount_ = 1;
    std::vector<int> tag_;

    int* indexTemp;//tab de correcpondence

    //For MP
    partition partitionMP_;

    imageManager(std::string path,cv::Mat image);
    graph& getGraph() {return this->graph_;} // Getter of the graph
    QEBT& getHierarchy() {return this->qbet_;} // Getter of the hierarchy
    void init(); // Initialize everything to be ready for add and remove markers
    void addMarkers(int* markers, int nbMarkers);
    void removeMarkers(int* markers, int nbMarkers);
    int getEdge(int n);
    int getWidth(){return this->width_;};
    int getHeight(){return this->height_;};
    ~imageManager();
};

#endif //INCREMENTAL_WATERSHED_IMAGEMANAGER_H
