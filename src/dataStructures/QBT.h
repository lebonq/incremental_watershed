//
// Created by lebon on 17/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_QBT_H
#define INCREMENTAL_WATERSHED_QBT_H
#include "Q.h"
#include <iostream>
#include "../data.h"

class QBT : public Q {
private:
    int* parent_;// represent the parent of each node
    int size_;
public:


    QBT(int nbVertex);
    int findCannonical(int q) override;
    void makeSet(int q) override;
    int makeUnion(int q1, int q2) override;
    int* getParents(){return this->parent_;};
    int getSize(){return this->size_;};
    void setParent(int x, int y){this->parent_[x] = y;}
    ~QBT();
};


#endif //INCREMENTAL_WATERSHED_QBT_H
