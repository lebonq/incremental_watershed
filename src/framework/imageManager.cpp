//
// Created by lebon on 13/05/2022.
//

#include "imageManager.h"

imageManager::imageManager(std::string path, cv::Mat image)
        : pathFile_{path},
          image_{image},
          height_{image.size().height},
          width_{image.size().width},
          graph_{this->height_ * this->width_},
          qbet_{this->height_ * this->width_},
          partitionMP_{this->height_ * this->width_} {}

void imageManager::toGraph() {
    int index = 0;
    for (int y = 0; y < this->height_; y++) {
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

    //TODO Temp
    this->map_graph_mst = new int[this->graph_.getNbVertex() * 2]();
    for (int i = 0; i < this->graph_.getNbVertex() * 2; i++) {
        this->map_graph_mst[i] = -1;
    }

    this->nbVertex_ = this->graph_.getNbVertex();
    algorithms::kruskal(this->graph_, this->qbet_, this->width_, this->map_graph_mst); // Apply the kruskal algorithm

    this->segments_ = new int[this->graph_.getNbVertex()](); //There is a ID for each vertex
    this->marks_ = new int[this->qbet_.getQBT().getSize()](); //There is size nodes
    this->sizePart_ = new int[this->graph_.getNbVertex()](); //There is at most |V| segments
    this->ws_ = new bool[this->qbet_.getQBT().getSize()](); //There is size nodes
    this->mstEdit_ = new bool[this->graph_.getMst().size()]; //Size of the MST

    for (int i = 0; i < this->graph_.getMst().size(); i++) { //At start all edges are in the MST
        this->mstEdit_[i] = true;
    }

    this->sizePart_[0] = this->graph_.getNbVertex(); //The first segment is the whole image

}

void imageManager::addMarkers(int *markers, int nbMarkers) {
    algorithms::addMarker(*this, markers, nbMarkers);
}

void imageManager::removeMarkers(int *markers, int nbMarkers) {
    algorithms::removeMarker(*this, markers, nbMarkers);
}

int imageManager::getEdge(int n) {
    if (n < this->nbVertex_) {
        std::cout << "Error : n is not a valid node" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return n - this->nbVertex_;
}

imageManager::~imageManager() {
    this->image_.release();

    delete[] this->segments_;
    delete[] this->marks_;
    delete[] this->sizePart_;
    delete[] this->ws_;
    delete[] this->mstEdit_;
    delete[] this->map_graph_mst;
}

bool imageManager::isInMst(int edge) {
    auto mst_edge = map_graph_mst[edge];
    if(mst_edge == -1)
    {
        return false;
    }
    else
    {
        return this->mstEdit_[mst_edge];
    }
}