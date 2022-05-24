//
// Created by lebon on 23/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_QEBT_H
#define INCREMENTAL_WATERSHED_QEBT_H
#include "dataStructures/Q.h"
#include "dataStructures/QT.h"
#include "dataStructures/QBT.h"

class QEBT : public Q{
private:
    QBT qbt; //TODO Do we need a shared pointer here?
    QT qt;
    int* root_;

public:
    QEBT(int nbVertex);
    int findCannonical(int q) override;
    void makeSet(int q) override;
    int makeUnion(int q1, int q2) override;
    ~QEBT();
};


#endif //INCREMENTAL_WATERSHED_QEBT_H
