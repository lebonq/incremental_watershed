//
// Created by lebon on 17/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_Q_H
#define INCREMENTAL_WATERSHED_Q_H


class Q {
private:
public:
    virtual int findCannonical(int) = 0;
    virtual void makeSet(int) = 0;
    virtual int makeUnion(int, int) = 0;
};


#endif //INCREMENTAL_WATERSHED_Q_H
