//
// Created by lebon on 17/05/2022.
//

#include "QBT.h"

QBT::QBT(int nbVertex):
    parent_{new int[nbVertex*2 -1]},
    size_{0}
{
}

int QBT::findCannonical(int q) {
    while(this->parent_[q] >= 0){
        q = this->parent_[q];
    }
    return q;
}

void QBT::makeSet(int q) {
    this->parent_[q] = -1;
    this->size_++;
}

int QBT::makeUnion(int q1, int q2) {
    this->parent_[q1] = this->size_;
    this->parent_[q2] = this->size_;
    this->makeSet(this->size_);
    return (this->size_) - 1;
}

QBT::~QBT() {
    delete[] this->parent_;
}
