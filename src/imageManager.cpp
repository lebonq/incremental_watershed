//
// Created by lebon on 13/05/2022.
//

#include "imageManager.h"

imageManager::imageManager(std::string path,cv::Mat image)
    : pathFile_{path},
    image_{image},
    height_{image.size().height},
    width_{image.size().width},
    graph_{this->height_*this->width_},
    qbet_{this->height_*this->width_},
    isReady_{false}
{
    //this->pathFile_ = path;
    //this->image_ = cv::imread(this->pathFile_,cv::IMREAD_GRAYSCALE);
    //this->height_ = this->image_.size().height;
    //this->width_ = this->image_.size().width;
    //this->graph_ = graph(this->height_*this->width_);

    //std::cout << (int) img.at<uchar>(1,1) << std::endl;
}

void imageManager::toGraph() {
    int index = 0;
    for(int y = 0; y < this->height_; y++) {
        for (int x = 0; x < this->width_; x++) {
            auto a = (int) this->image_.at<uchar>(y, x);
            if (x + 1 < this->width_) { // Element right
                auto b = (int) this->image_.at<uchar>(y, x + 1);
                this->graph_.setWeight(index, std::abs(a - b));
            } else {//For the ghost edges
                this->graph_.setWeight(index, -1);
            }
            index++; //We go the index of the next edge
            if (y + 1 < this->height_) { //Element below
                auto b = (int) this->image_.at<uchar>(y + 1, x);
                this->graph_.setWeight(index, std::abs(a - b));
            } else {
                this->graph_.setWeight(index, -1);
            }
            index++;
        }
    }
}

void imageManager::init() {
    this->toGraph(); // Convert our image into a graph
    this->graph_.init_sortedEdges(); // Sort the edges of the graph
    algorithms::kruskal(this->graph_,this->qbet_,this->width_); // Apply the kruskal algorithm

    this->isReady_ = true;
}

imageManager::~imageManager() {
    this->image_.release();
}
