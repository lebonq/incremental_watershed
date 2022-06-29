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

    //TODO Temp
    this->indexTemp = new int[this->graph_.getNbVertex()*2]();
    for(int i = 0; i < this->graph_.getNbVertex()*2; i++){
        this->indexTemp[i] = -1;
    }

    this->nbVertex_ = this->graph_.getNbVertex();
    algorithms::kruskal(this->graph_,this->qbet_,this->width_,this->indexTemp); // Apply the kruskal algorithm


    this->segments_ = new int[this->graph_.getNbVertex()](); //There is a ID for each vertex
    this->marks_ = new int[this->qbet_.getQBT().getSize()](); //There is size nodes
    this->sizePart_ = new int[this->graph_.getNbVertex()](); //There is at most |V| segments
    this->ws_ = new bool[this->qbet_.getQBT().getSize()](); //There is size nodes
    this->mstEdit_ = new bool[this->graph_.getMst().size()]; //Size of the MST

    for(int i = 0; i < this->graph_.getMst().size(); i++) { //At start all edges are in the MST
        this->mstEdit_[i] = true;
    }

    this->sizePart_[0] = this->graph_.getNbVertex(); //The first segment is the whole image

    this->isReady_ = true;

    //std::cout << this->graph_.getMst()[this->getEdge(78624+1)] << std::endl;
    //on veut la positon de l'edge 18883 dans le mst
    /*std::cout << this->indexTemp[18883] << std::endl;
    std::cout << this->graph_.getMst()[this->indexTemp[18883]] << std::endl;*/

    /*
     * std::cout << this->graph_.getMst().size() << std::endl;
    std::cout << this->graph_.getEdges()[this->graph_.getMst()[1074645]] << std::endl;
    std::cout << this->mstEdit_[this->indexTemp[(2*417819)-2]] << std::endl:*/
}

void imageManager::addMarkers(int* markers, int nbMarkers) {
    algorithms::addMarker(*this,markers,nbMarkers);
}

void imageManager::removeMarkers(int* markers, int nbMarkers) {
    algorithms::removeMarker(*this,markers,nbMarkers);
}

int imageManager::getEdge(int n){
    if(n < this->nbVertex_){
        std::cout<<"Error : n is not a valid node"<<std::endl;
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
}