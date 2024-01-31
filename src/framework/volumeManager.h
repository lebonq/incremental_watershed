//
// Created by lebonq on 29/01/24.
//

#pragma once

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include <vector>
#include <filesystem>
#include "dataStructures/graph.h"
#include "algorithms3D.h"
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
    std::unique_ptr<std::vector<int>> segments_; //Label for each vertex
    std::unique_ptr<std::vector<int>> marks_; //visitCount in the paper
    std::unique_ptr<std::vector<int>> sizePart_; //Size of each segment
    std::unique_ptr<std::vector<bool>> ws_; //Is the node a watershed
    std::unique_ptr<std::vector<bool>> mstEdit_; //Is the edge in the MST
    int tagCount_ = 1;

public:
    volumeManager() = default;
    DicomImage* loadDicomFile(const std::string& filename);
    void loadVolume(const std::string& folder_name);
    void createGraph();
    void buildHierarchy();
    void initPostprocessStructure();

    int getWidth() const { return this->width_; };
    int getHeight() const { return this->height_; };
    int getDepth() const { return this->depth_; };

    int getTagCount() const { return this->tagCount_; }
    void setTagCount(int tagCount) { this->tagCount_ = tagCount; }

    std::vector<int>& getMapGraphMst() const { return *this->map_graph_mst_; }
    std::vector<int>& getSegments() const { return *this->segments_; }
    std::vector<int>& getMarks() const { return *this->marks_; }
    std::vector<int>& getSizePart() const { return *this->sizePart_; }
    std::vector<bool>& getWs() const { return *this->ws_; }
    std::vector<bool>& getMstEdit() const { return *this->mstEdit_; }

    QEBT& getHierarchy() const { return *this->hierarchy_; }
    graph& getGraph() const { return *this->graph_; }

    //Give us the edge id from the node ID
    int getEdge(int n){
        if (n < this->graph_->getNbVertex()) {
            std::cout << "Error : n is not a valid node" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        return n - this->graph_->getNbVertex();
    };

    ~volumeManager();
};

