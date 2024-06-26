//
// Created by lebonq on 12/05/2022.
//
#ifndef INCREMENTAL_WATERSHED_GRAPH_H
#define INCREMENTAL_WATERSHED_GRAPH_H
#include <iostream>
#include <cstring>
#include <vector>

class graph
{
private:
    int* edges_;
    std::vector<int> mst_; //contain edges in order of the nodes edge 0 correspond to nodes 0 + |V|
    int nbVertex_;
    int nbEdge_;
    int histSize_; // new member variable for histogram size

public:
    int* count_; //Number of edges with a value between 0 and 255
    int** sortedEdges_; //index the edges sorted by value Ie i want to get the first edge of value 10 sortedEdges[10][0]

    explicit graph(int pNbVertex, int histSize, int pNbEdge); // modified constructor
    bool setWeight(int pIndex, int pWeight);
    int* getEdges() { return this->edges_; }
    void init_sortedEdges();
    int& getNbVertex() { return this->nbVertex_; }
    int& getNbEdge() { return this->nbEdge_; }
    const int getHistSize() const{ return this->histSize_; } // new member function for histogram size
    void cleanMemory();
    std::vector<int>& getMst() { return this->mst_; }
    ~graph();
};


#endif //INCREMENTAL_WATERSHED_GRAPH_H
