//
// Created by lebonq on 29/01/24.
//

#ifndef VOLUMEMANAGER_H
#define VOLUMEMANAGER_H

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include <vector>
#include <filesystem>
#include "dataStructures/graph.h"
#include "algorithms.h"
#include "dataStructures/QEBT.h"
#include "libs/natural_sort.h"

class volumeManager {
private:
    std::vector<DicomImage*> volume_;
    int width_;
    int height_;
    int depth_;
    std::unique_ptr<graph> graph_;
    std::unique_ptr<QEBT> hierarchy_;
    std::unique_ptr<std::vector<int>>  map_graph_mst_;

public:
    volumeManager() = default;
    DicomImage* loadDicomFile(const std::string& filename);
    void loadVolume(const std::string& folder_name);
    void createGraph();
    void buildHierarchy();
    ~volumeManager();
};


#endif //VOLUMEMANAGER_H
