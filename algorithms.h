//
// Created by lebon on 17/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_ALGORITHMS_H
#define INCREMENTAL_WATERSHED_ALGORITHMS_H
#include "dataStructures/Q.h"
#include "graph.h"

class algorithms {
public:
    static void kruskal(graph& G,Q& Q, int w , int h);
};


#endif //INCREMENTAL_WATERSHED_ALGORITHMS_H
