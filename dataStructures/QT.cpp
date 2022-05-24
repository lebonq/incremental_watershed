//
// Created by lebon on 20/05/2022.
//

#include "QT.h"

QT::QT(int nbVertex):
    parent_{new int[nbVertex*2 -1]},
    rank_{new int[nbVertex*2 -1]},
    size_{0}
{}

int QT::findCannonical(int q) {
    int r = q;
    int tmp = 0;
    while(this->parent_[r] >= 0){
        r = this->parent_[r];
    }
    while(this->parent_[q] >= 0){
        tmp = q; //TODO : check if it's necessary
        q = this->parent_[q];
        this->parent_[tmp] = r;
    }
    return r;
}

void QT::makeSet(int q) {
    this->parent_[q] = -1;
    this->rank_[q] = 0;
    this->size_++;
}

int QT::makeUnion(int q1, int q2) {
    if(this->rank_[q1] > this->rank_[q2]){
        int tmp = q1;
        q1 = q2;
        q2 = tmp;
    }
    if(this->rank_[q1] == this->rank_[q2]){
        this->rank_[q2]++;
    }
    this->parent_[q1] = q2;
    return q2;
}

QT::~QT() {
    delete[] this->parent_;
    delete[] this->rank_;

}