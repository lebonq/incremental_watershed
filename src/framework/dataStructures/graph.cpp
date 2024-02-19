//
// Created by lebonq on 12/05/2022.
//

#include "graph.h"
#include <cstring>

graph::graph(int pNbVertex, int histSize, int pNbEdge) :
        nbVertex_{pNbVertex},
        nbEdge_{pNbEdge},
        histSize_{histSize}, // initialize histSize_
        edges_{new int[pNbEdge]},
        count_{new int[histSize]()}, // use histSize_ instead of 256
        sortedEdges_{new int *[histSize]} { // use histSize_ instead of 256
}


/**
 * Associate a weight to an edge
 * @param pIndex Index of the edge you want to add an weight
 * @param pWeight
 * @return If the operation success
 */
bool graph::setWeight(int pIndex, int pWeight) {
    if (pIndex >= this->nbEdge_) {
        std::cout << "Error to set weight to an edge : Index out of range" << std::endl;
        return false;
    } else {
        if (pWeight >= 0) {
            this->count_[pWeight]++;
        };
        this->edges_[pIndex] = pWeight;
        return true;
    }
}

void graph::init_sortedEdges() {

    //Init all the array at the right size
    for (int i = 0; i < histSize_; i++) { // use histSize_ instead of 256
        this->sortedEdges_[i] = new int[this->count_[i]];
    }

    //Creating an array which will keep track of the index in the sorted edges matrix
    int *count_local = new int[histSize_]; // use histSize_ instead of 256
    memset(count_local, 0, histSize_ * sizeof(int)); // use histSize_ instead of 256

    //Fill the array
    for (int i = 0; i < this->nbEdge_; i++) {
        if (this->edges_[i] >= 0) {//To filter ghost edge
            auto edges = this->edges_[i];
            auto count_lol =  count_local[this->edges_[i]];
            this->sortedEdges_[this->edges_[i]][count_local[this->edges_[i]]] = i;
            count_local[this->edges_[i]]++;
        }
    }
    delete[] count_local;
}

graph::~graph() {
    delete[] this->edges_;
    delete[] this->count_;

    for (int i = 0; i < histSize_; i++) { // use histSize_ instead of 256
        delete[] this->sortedEdges_[i];
    }
    delete[] this->sortedEdges_;
}