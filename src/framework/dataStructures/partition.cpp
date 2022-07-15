//
// Created by lebon on 07/07/2022.
//

#include "partition.h"
partition::partition(int nbVertex):
    parent_{new int[nbVertex]}, //TODO init to 0
    lock_{new bool[nbVertex]()},
    size_{nbVertex} //really necessary ??
    {
        //here -2 cause we brute force
        for(int i = 0; i <nbVertex;++i){
            this->parent_[i] = -2;
        }

}

int partition::findCanonical(int q) {
    while(this->parent_[q] >= 0){
        q = this->parent_[q];
    }
    return q;
}

partition::~partition() {
    delete[] parent_;
    delete[] lock_;
}

