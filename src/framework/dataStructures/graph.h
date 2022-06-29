//
// Created by lebonq on 12/05/2022.
//
#ifndef INCREMENTAL_WATERSHED_GRAPH_H
#define INCREMENTAL_WATERSHED_GRAPH_H
#include <iostream>
#include <vector>
#include "../data.h"


class graph {
private:
    int* edges_;
    std::vector<int> mst_; //contain edges in order of the nodes edge 0 correspond to nodes 0 + |V|
    int nbVertex_;


public:
    int* count_;//Number of edges with a value between 0 and 255
    int** sortedEdges_;//index in the vector edges of the edges sorted by value

    explicit graph(int pNbVertex);
    bool setWeight(int pIndex, int pWeight);
    int* getEdges(){return this->edges_;}
    void init_sortedEdges();
    int& getNbVertex(){return this->nbVertex_;}
    std::vector<int>& getMst(){return this->mst_;}
    ~graph();
};


#endif //INCREMENTAL_WATERSHED_GRAPH_H
