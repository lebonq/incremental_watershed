//
// Created by lebonq on 12/05/2022.
//

#include "graph.h"
#include <cstring>

graph::graph(int pNbVertex) :
        nbVertex_{pNbVertex},
        edges_{new int[pNbVertex * 2]},
        count_{new int[256]()},
        sortedEdges_{new int *[256]} {
}

/**
 * Associate a weight to an edge
 * @param pIndex Index of the edge you want to add an weight
 * @param pWeight
 * @return If the operation success
 */
bool graph::setWeight(int pIndex, int pWeight) {
    if (pIndex >= this->nbVertex_ * 2) {
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

/**
 * Allocate memories to store the sorted edges
 */
void graph::init_sortedEdges() {

    //Init all the array at the right size
    for (int i = 0; i < 256; i++) {
        this->sortedEdges_[i] = new int[this->count_[i]];
    }

    //Creating an array which will keep track of the index in the sorted edges matrix
    int *count_local = new int[256];
    memset(count_local, 0, 256 * sizeof(int));

    //Fill the array
    for (int i = 0; i < this->nbVertex_ * 2; i++) {
        if (this->edges_[i] >= 0) {//To filter gosht edge
            //std::cout << "this->sortedEdges_[" << this->edges_[i]<< "][" << count_local[this->edges_[i]] << "] = " << i << std::endl;
            this->sortedEdges_[this->edges_[i]][count_local[this->edges_[i]]] = i;
            count_local[this->edges_[i]]++;
        }
    }
    delete[] count_local;
}

graph::~graph() {
    delete[] this->edges_;
    delete[] this->count_;

    for (int i = 0; i < 256; i++) {
        delete[] this->sortedEdges_[i];
    }
    delete[] this->sortedEdges_;
}