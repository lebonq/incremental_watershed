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
#include <thread>
#include <condition_variable>
#include <semaphore>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <ctype.h>
#include <semaphore.h>
#include "parDMap.h"
#include <random>
#include <atomic>
#include <omp.h>
#include <fstream>
#include <iostream>
#include "imageManager.fwd.h" //Break circular dependency

class algorithms {
public:
    static void kruskal(graph& G,Q& Q, int w,int* temp);
    static int breadthFirstSearchLabel(imageManager& im, int tag, int p);
    static void breadthFirstSearchLabel_v2(imageManager& im, int* p, int* buffer, int* n,std::binary_semaphore& finish, std::binary_semaphore& start, int id);
    static int breadthFirstSearchLabel_v2_seq(imageManager& im, int* p, int* buffer, int* n);
    static void splitSegment(imageManager & im, bool* historyVisited,std::vector<int> queueEdges);
    static void splitSegment_optimised_v2(imageManager & im, bool* historyVisited,std::vector<int> queueEdges);
    static void splitSegment_v3(imageManager& im, bool* historyVisited, std::vector<int> queueEdges);
    static std::tuple<int,int> edge_to_vertices(const int edge,const int w);
    static void mergeSegment(int edge, imageManager & im);
    static void removeMarker(imageManager & im,int* markers,int nbMarkers);
    static void addMarker(imageManager & im,int* markers,int nbMarkers);
    static void showSegmentation(imageManager & im,std::string nameOfImage);
    static bool get_vector_from_txt(const std::string& file_path, std::vector<int> &vector);
    static void get_tab_from_image(const std::string &file_path, std::vector<int> &values);
    static void vector_to_csv(std::vector<double>& vector,std::string file_path);
    static void get_DIFT_seed_from_image(const std::string &file_path, const int id);

};


#endif //INCREMENTAL_WATERSHED_ALGORITHMS_H
