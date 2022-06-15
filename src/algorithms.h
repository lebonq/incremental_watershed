//
// Created by lebon on 17/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_ALGORITHMS_H
#define INCREMENTAL_WATERSHED_ALGORITHMS_H
#include <vector>
#include "imageManager.h"
#include "dataStructures/Q.h"
#include "dataStructures/graph.h"

#include "imageManager.fwd.h" //Break circular dependency

class algorithms {
public:
    static void kruskal(graph& G,Q& Q, int w,int* temp);
    static int breadthFirstSearchLabel(imageManager& im, int tag, int p);
    static void splitSegment(imageManager & im, bool* historyVisited,std::vector<int> queueEdges);
    static void mergeSegment(int p1, int p2, imageManager & im);
    static void removeMarker(imageManager & im,int* markers,int nbMarkers);
    static void addMarker(imageManager & im,int* markers,int nbMarkers);
};


#endif //INCREMENTAL_WATERSHED_ALGORITHMS_H
