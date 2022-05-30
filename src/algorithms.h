//
// Created by lebon on 17/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_ALGORITHMS_H
#define INCREMENTAL_WATERSHED_ALGORITHMS_H
#include <vector>
#include "dataStructures/Q.h"
#include "dataStructures/graph.h"

class algorithms {
public:
    static void kruskal(graph& G,Q& Q, int w);
    static void breadthFirstSearchLabel();
    static void fusionSegment();
    static void splitSegment();
    static void removeMarker();
    static void addMarker();
};


#endif //INCREMENTAL_WATERSHED_ALGORITHMS_H
