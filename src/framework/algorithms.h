//
// Created by lebon on 17/05/2022.
//

#ifndef INCREMENTAL_WATERSHED_ALGORITHMS_H
#define INCREMENTAL_WATERSHED_ALGORITHMS_H
#include <vector>
#include <stack>
#include "imageManager.h"
#include "dataStructures/Q.h"
#include "dataStructures/graph.h"
#include <omp.h>
#include <oneapi/tbb.h>

#include "imageManager.fwd.h" //Break circular dependency

class algorithms {
public:
    static void kruskal(graph& G,Q& Q, int w,int* temp);
    static int breadthFirstSearchLabel(imageManager& im, int tag, int p);
    static void splitSegment(imageManager & im, bool* historyVisited,std::vector<int> queueEdges);
    static void mergeSegment(int edge, imageManager & im);
    static void removeMarker(imageManager & im,int* markers,int nbMarkers);
    static void addMarker(imageManager & im,int* markers,int nbMarkers);
    static void showSegmentation(imageManager & im,std::string nameOfImage);
    //Our 3 operations but in MP
    static int breadthFirstSearchLabelMP(imageManager& im, int p,int currentBlock, int xstart,int xend, int ystart, int yend, int* parentTmp,
                                         std::map<int,int>* hVerticesQueue,std::map<int,int>* vVerticesQueue);
    static void splitSegmentMP(imageManager& im, std::vector<int> queueEdges);
    static void addMarkerMP(imageManager & im,int* markers,int nbMarkers);
    static void mergeSegmentMP(imageManager & im, std::vector<int> queueEdges);
    static void removeMarkerMP(imageManager & im,int* markers,int nbMarkers);
    static void showSegmentationMP(imageManager & im,std::string nameOfImage);
};


#endif //INCREMENTAL_WATERSHED_ALGORITHMS_H
