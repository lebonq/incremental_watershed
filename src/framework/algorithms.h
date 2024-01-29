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
#include <atomic>
#include <omp.h>
#include <fstream>
#include <iostream>
#include "imageManager.fwd.h" //Break circular dependency

class algorithms {
public:
    static void kruskal(graph& G,Q& Q, int w,int* temp);
    static int breadthFirstSearchLabel(imageManager& im, int tag, int p);
    static void breadthFirstSearchLabel_optimised(imageManager& im, int* p, int* old_tag, bool* re_init, bool* re_init_c,std::condition_variable& finish, std::mutex& finish_m, bool* finish_b);
    static void splitSegment(imageManager & im, bool* historyVisited,std::vector<int> queueEdges);
    static void splitSegment_optimised(imageManager & im, bool* historyVisited,std::vector<int> queueEdges);
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
