//
// Created by lebon on 07/07/2022.
//

#ifndef INCREMENTAL_WATERSHED_PARTITION_H
#define INCREMENTAL_WATERSHED_PARTITION_H


class partition {
private:
    int* parent_;// represent the parent of each node
    bool* lock_;
    int size_;
public:
    partition(int nbVertex);
    int findCanonical(int q);
    int* getParents(){return this->parent_;};
    ~partition();
};


#endif //INCREMENTAL_WATERSHED_PARTITION_H
