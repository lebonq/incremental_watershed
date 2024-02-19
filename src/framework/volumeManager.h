//
// Created by lebonq on 29/01/24.
//

#pragma once

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include <vector>
#include <filesystem>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "dataStructures/graph.h"
#include "algorithms3D.h"
#include "dataStructures/QEBT.h"
#include "libs/natural_sort.h"

class volumeManager {
private:
    std::vector<cv::Mat> volume_;

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
    std::unique_ptr<std::vector<bool>> isMarked_; //avoid to add/remove marker at the same pixel twice
    std::unique_ptr<std::vector<int>> colorTab_; //Color for each segment
    int tagCount_ = 1;

public:
    volumeManager() = default;
    cv::Mat loadDicomFile(const std::string& filename);
    void loadVolume(const std::string& folder_name);
    void createGraph();
    void createToyGraph();
    void buildHierarchy();
    void initPostprocessStructure();
    cv::Mat getSegmentedSlice(int z);
    std::vector<cv::Mat> getSegmentedVolume();

    int getWidth() const { return this->width_; };
    int getHeight() const { return this->height_; };
    int getDepth() const { return this->depth_; };

    int getTagCount() const { return this->tagCount_; }
    void setTagCount(int tagCount) { this->tagCount_ = tagCount; }

    void addMarkers(std::vector<int>& markers);
    void removeMarkers(std::vector<int>& markers, int nbMarkers);
    bool isInMStEdit(int edge);
    void dualisation_segmentation(std::vector<int> &markers, int value);

    std::vector<int>& getMapGraphMst() const { return *this->map_graph_mst_; }
    std::vector<int>& getSegments() const { return *this->segments_; }
    void modifySegments(int index, int value) { this->segments_->at(index) = value; }
    std::vector<int>& getMarks() { return *marks_.get(); }
    void modifyMarks(int index, int value) { this->marks_->at(index) = value; }
    std::vector<int>& getSizePart() { return *this->sizePart_; }
    void setSizePart(int index, int value) { this->sizePart_->at(index) = value; }
    std::vector<bool>& getWs() { return *this->ws_; }
    void modifyWs(int index, bool value) { this->ws_->at(index) = value; }
    std::vector<bool>& getMstEdit() { return *this->mstEdit_; }
    void modifyMstEdit(int index, bool value) { this->mstEdit_->at(index) = value; }

    QEBT& getHierarchy() { return *this->hierarchy_; }
    graph& getGraph() { return *this->graph_; }

    //Give us the edge id from the node ID
    int getEdge(int n);

    ~volumeManager();
};

