//
// Created by lebonq on 31/01/24.
//

#ifndef ALGORITHMS_3D_H
#define ALGORITHMS_3D_H
#include <vector>
#include "dataStructures/Q.h"
#include "dataStructures/graph.h"
#include <omp.h>
#include <fstream>
#include <iostream>
#include "volumeManager.fwd.h"
#include "volumeManager.h"


class algorithms3D {
public:
    static void kruskal(graph& G,Q& Q, int w, int h,int* temp);
    static int breadthFirstSearchLabel(volumeManager& vol, int tag, int p);
    void algorithms3D::splitSegment(volumeManager &vol, bool *historyVisited,
                              std::vector<int> queueEdges);
    void algorithms3D::mergeSegment(int edge, volumeManager &vol);
};



#endif //ALGORITHMS_3D_H
