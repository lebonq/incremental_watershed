//
// Created by lebon on 07/07/2022.
//

#include "partition.h"
partition::partition(int nbVertex):
    parent_{new int[nbVertex]}, //TODO init to 0
    lock_{new bool[nbVertex]()},
    size_{nbVertex} //really necessary ??
    {
        std::fill_n(this->getParents(), nbVertex, -2);
}

int partition::findCanonical(int q) {
    while(this->parent_[q] >= 0){
        q = this->parent_[q];
    }
    return q;
}

int partition::findCanonicalPathCompression(int q) {
    int r = q;
    int tmp = 0;
    while(this->parent_[r] >= 0){
        r = this->parent_[r];
    }
    while(this->parent_[q] >= 0){
        tmp = q;
        q = this->parent_[q];
        this->parent_[tmp] = r;
    }
    return r;
}

partition::~partition() {
    delete[] parent_;
    delete[] lock_;
}

