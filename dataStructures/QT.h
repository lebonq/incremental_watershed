//
// Created by lebon on 20/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_QT_H
#define INCREMENTAL_WATERSHED_QT_H
#include "Q.h"

class QT : public Q {
private:
    int* parent_;// represent the parent of each node
    int* rank_;// represent the rank of each node
    int size_;
public:
    QT(int nbVertex);
    int findCannonical(int q) override;
    void makeSet(int q) override;
    int makeUnion(int q1, int q2) override;
    int getSize(){return this->size_;}
    ~QT();
};


#endif //INCREMENTAL_WATERSHED_QT_H
