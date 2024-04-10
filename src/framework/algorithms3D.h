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


class algorithms3D
{
public:
    static int from3Dto1D(int x, int y, int z, int w, int h);
    static std::tuple<int,int,int> from1Dto3D(int p, int w, int h, int d);
    static void kruskal(graph& G, Q& Q, int w, int h, int* temp);
    static int breadthFirstSearchLabel(volumeManager& vol, int tag, int p);
    static void splitSegment(volumeManager& vol, std::vector<bool>& historyVisited,
                      std::vector<int>& queueEdges);
    static std::tuple<int, int> edge_to_vertices(int edge, int w, int h);
    static void init_dmap(volumeManager* vol);
    static void deinit_dmap();
    static void splitSegment_par(volumeManager& vol, std::vector<bool>& historyVisited, std::vector<int>& queueEdges);
    static void mergeSegment(int edge, volumeManager& vol);
    static void removeMarker(volumeManager& vol, std::vector<int>& markers, int nbMarkers);
    static void addMarker(volumeManager& vol, std::vector<int>& markers);
};


#endif //ALGORITHMS_3D_H
