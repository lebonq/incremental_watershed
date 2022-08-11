//
// Created by lebon on 29/06/2022.
//

#include "test.h"
#include "../framework/data.h"
#include "../markers.h"
#include "../framework/dataStructures/graph.h"
#include "../framework/imageManager.h"
#include <opencv2/highgui/highgui.hpp>
#include <benchmark/benchmark.h>

static void BM_Add_Marker_1_Cell2(benchmark::State& state) {
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        state.ResumeTiming();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_1_Cell2_MP(benchmark::State& state) {
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        state.ResumeTiming();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_2_Cell2(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
        state.ResumeTiming();
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,false);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_2_Cell2_MP(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
        state.ResumeTiming();
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,true);
        state.PauseTiming();
    }
}

static void BM_Remove_Marker_3_Cell2(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,false);
        state.ResumeTiming();
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,false);
        state.PauseTiming();
    }
}

static void BM_Remove_Marker_3_Cell2_MP(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,true);
        state.ResumeTiming();
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,true);
        state.PauseTiming();
    }
}

static void BM_Remove_Marker_4_Cell2(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,false);
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,false);
        state.ResumeTiming();
        testImg.removeMarkers(markers4Cell2,nbMarkers4Cell2,false);
        state.PauseTiming();
    }
}

static void BM_Remove_Marker_4_Cell2_MP(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,true);
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,true);
        state.ResumeTiming();
        testImg.removeMarkers(markers4Cell2,nbMarkers4Cell2,true);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_5_Cell2(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,false);
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,false);
        testImg.removeMarkers(markers4Cell2,nbMarkers4Cell2,false);
        state.ResumeTiming();
        testImg.addMarkers(markers5Cell2,nbMarkers5Cell2,false);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_5_Cell2_MP(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,true);
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,true);
        testImg.removeMarkers(markers4Cell2,nbMarkers4Cell2,true);
        state.ResumeTiming();
        testImg.addMarkers(markers5Cell2,nbMarkers5Cell2,true);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_1_Cell2_MP_LABELLING(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();

        std::fill_n(testImg.partitionMP_.getParents(), testImg.getGraph().getNbVertex(), -2); // Set segemnt to -2 by default
        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers1Cell2;i++){
            marker = markers1Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]++;
                if(testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)]= true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        state.ResumeTiming();
        algorithms::splitSegmentMP(testImg, queueEdges);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_1_Cell2_LABELLING(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        bool* historyVisited = new bool[testImg.getGraph().getNbVertex()]();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers1Cell2;i++){
            marker = markers1Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]++;
                if(testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)]= true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }


        state.ResumeTiming();
        algorithms::splitSegment(testImg,historyVisited,queueEdges);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_1_Cell2_UPDATE_HIERARCHY(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();

        state.ResumeTiming();
        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers1Cell2;i++){
            marker = markers1Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]++;
                if(testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)]= true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        state.PauseTiming();
    }
}

static void BM_Add_Marker_2_Cell2_MP_LABELLING(benchmark::State& state) {
    auto testim = cv::imread("cell2.jpg", cv::IMREAD_GRAYSCALE);

    for (auto _: state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg", testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2, nbMarkers1Cell2, true);

        std::fill_n(testImg.partitionMP_.getParents(), testImg.getGraph().getNbVertex(), -2); // Set segemnt to -2 by default

        int marker, up = 0;
        QBT &qbt = testImg.getHierarchy().getQBT();
        int *parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for (int i = 0; i < nbMarkers2Cell2; i++) {
            marker = markers2Cell2[i];
            up = parent[marker];
            while (up != -1) {
                testImg.marks_[up]++;
                if (testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)] = true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        state.ResumeTiming();
        algorithms::splitSegmentMP(testImg, queueEdges);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_2_Cell2_LABELLING(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2, nbMarkers1Cell2, false);

        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        bool* historyVisited = new bool[testImg.getGraph().getNbVertex()]();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers2Cell2;i++){
            marker = markers2Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]++;
                if(testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)]= true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }

        state.ResumeTiming();
        algorithms::splitSegment(testImg,historyVisited,queueEdges);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_2_Cell2_UPDATE_HIERARCHY(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();


        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers1Cell2;i++){
            marker = markers1Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]++;
                if(testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)]= true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        queueEdges.clear();
        state.ResumeTiming();

        marker,up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers2Cell2;i++){
            marker = markers2Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]++;
                if(testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)]= true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        state.PauseTiming();
    }
}

static void BM_Remove_Marker_3_Cell2_MP_LABELLING(benchmark::State& state) {
    auto testim = cv::imread("cell2.jpg", cv::IMREAD_GRAYSCALE);

    for (auto _: state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg", testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2, nbMarkers1Cell2, true);
        testImg.addMarkers(markers2Cell2, nbMarkers2Cell2, true);

        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers3Cell2;i++){
            marker = markers3Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]--;
                if(testImg.marks_[up] == 1) {
                    testImg.ws_[testImg.getEdge(up)]= false;
                    testImg.mstEdit_[testImg.getEdge(up)]= true;
                    queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        state.ResumeTiming();
        algorithms::mergeSegmentMP(testImg, queueEdges);
        state.PauseTiming();
    }
}

static void BM_Remove_Marker_3_Cell2_LABELLING(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2, nbMarkers1Cell2, false);
        testImg.addMarkers(markers2Cell2, nbMarkers2Cell2, false);

        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers3Cell2;i++){
            marker = markers3Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]--;
                if(testImg.marks_[up] == 1) {
                    testImg.ws_[testImg.getEdge(up)]= false;
                    testImg.mstEdit_[testImg.getEdge(up)]= true;
                    state.ResumeTiming();
                    algorithms::mergeSegment(mstL[testImg.getEdge(up)],testImg);
                    state.PauseTiming();
                    break;
                }
                up = parent[up];
            }
        }
    }
}

static void BM_Remove_Marker_3_Cell2_UPDATE_HIERARCHY(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg", testim);
        testImg.init();


        int marker, up = 0;
        QBT &qbt = testImg.getHierarchy().getQBT();
        int *parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for (int i = 0; i < nbMarkers1Cell2; i++) {
            marker = markers1Cell2[i];
            up = parent[marker];
            while (up != -1) {
                testImg.marks_[up]++;
                if (testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)] = true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        queueEdges.clear();

        marker, up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for (int i = 0; i < nbMarkers2Cell2; i++) {
            marker = markers2Cell2[i];
            up = parent[marker];
            while (up != -1) {
                testImg.marks_[up]++;
                if (testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)] = true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }

        queueEdges.clear();

        state.ResumeTiming();

        marker, up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers3Cell2;i++){
            marker = markers3Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]--;
                if(testImg.marks_[up] == 1) {
                    testImg.ws_[testImg.getEdge(up)]= false;
                    testImg.mstEdit_[testImg.getEdge(up)]= true;
                    break;
                }
                up = parent[up];
            }
        }
        state.PauseTiming();
    }
}

static void BM_Remove_Marker_4_Cell2_MP_LABELLING(benchmark::State& state) {
    auto testim = cv::imread("cell2.jpg", cv::IMREAD_GRAYSCALE);

    for (auto _: state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg", testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2, nbMarkers1Cell2, true);
        testImg.addMarkers(markers2Cell2, nbMarkers2Cell2, true);
        testImg.removeMarkers(markers3Cell2, nbMarkers3Cell2, true);

        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers4Cell2;i++){
            marker = markers4Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]--;
                if(testImg.marks_[up] == 1) {
                    testImg.ws_[testImg.getEdge(up)]= false;
                    testImg.mstEdit_[testImg.getEdge(up)]= true;
                    queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        state.ResumeTiming();
        algorithms::mergeSegmentMP(testImg, queueEdges);
        state.PauseTiming();

    }
}

static void BM_Remove_Marker_4_Cell2_LABELLING(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2, nbMarkers1Cell2, false);
        testImg.addMarkers(markers2Cell2, nbMarkers2Cell2, false);
        testImg.removeMarkers(markers3Cell2, nbMarkers3Cell2, false);

        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers4Cell2;i++){
            marker = markers4Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]--;
                if(testImg.marks_[up] == 1) {
                    testImg.ws_[testImg.getEdge(up)]= false;
                    testImg.mstEdit_[testImg.getEdge(up)]= true;
                    state.ResumeTiming();
                    algorithms::mergeSegment(mstL[testImg.getEdge(up)],testImg);
                    state.PauseTiming();
                    break;
                }
                up = parent[up];
            }
        }
        state.PauseTiming();
    }
}

static void BM_Remove_Marker_4_Cell2_UPDATE_HIERARCHY(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg", testim);
        testImg.init();


        int marker, up = 0;
        QBT &qbt = testImg.getHierarchy().getQBT();
        int *parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for (int i = 0; i < nbMarkers1Cell2; i++) {
            marker = markers1Cell2[i];
            up = parent[marker];
            while (up != -1) {
                testImg.marks_[up]++;
                if (testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)] = true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        queueEdges.clear();

        marker, up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for (int i = 0; i < nbMarkers2Cell2; i++) {
            marker = markers2Cell2[i];
            up = parent[marker];
            while (up != -1) {
                testImg.marks_[up]++;
                if (testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)] = true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }

        queueEdges.clear();

        marker, up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers3Cell2;i++){
            marker = markers3Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]--;
                if(testImg.marks_[up] == 1) {
                    testImg.ws_[testImg.getEdge(up)]= false;
                    testImg.mstEdit_[testImg.getEdge(up)]= true;
                    break;
                }
                up = parent[up];
            }
        }

        queueEdges.clear();

        state.ResumeTiming();

        marker, up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers4Cell2;i++){
            marker = markers4Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]--;
                if(testImg.marks_[up] == 1) {
                    testImg.ws_[testImg.getEdge(up)]= false;
                    testImg.mstEdit_[testImg.getEdge(up)]= true;
                    break;
                }
                up = parent[up];
            }
        }
        state.PauseTiming();
    }
}

static void BM_Add_Marker_5_Cell2_MP_LABELLING(benchmark::State& state) {
    auto testim = cv::imread("cell2.jpg", cv::IMREAD_GRAYSCALE);

    for (auto _: state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg", testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2, nbMarkers1Cell2, true);
        testImg.addMarkers(markers2Cell2, nbMarkers2Cell2, true);
        testImg.removeMarkers(markers3Cell2, nbMarkers3Cell2, true);
        testImg.removeMarkers(markers4Cell2, nbMarkers4Cell2, true);

        std::fill_n(testImg.partitionMP_.getParents(), testImg.getGraph().getNbVertex(), -2); // Set segemnt to -2 by default

        int marker, up = 0;
        QBT &qbt = testImg.getHierarchy().getQBT();
        int *parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for (int i = 0; i < nbMarkers5Cell2; i++) {
            marker = markers5Cell2[i];
            up = parent[marker];
            while (up != -1) {
                testImg.marks_[up]++;
                if (testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)] = true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        state.ResumeTiming();
        algorithms::splitSegmentMP(testImg, queueEdges);
        state.PauseTiming();

    }
}

static void BM_Add_Marker_5_Cell2_LABELLING(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2, nbMarkers1Cell2, false);
        testImg.addMarkers(markers2Cell2, nbMarkers2Cell2, false);
        testImg.removeMarkers(markers3Cell2, nbMarkers3Cell2, false);
        testImg.removeMarkers(markers4Cell2, nbMarkers4Cell2, false);

        state.ResumeTiming();

        int marker,up = 0;
        QBT& qbt = testImg.getHierarchy().getQBT();
        int* parent = qbt.getParents();
        bool* historyVisited = new bool[testImg.getGraph().getNbVertex()]();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers5Cell2;i++){
            marker = markers5Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]++;
                if(testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)]= true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }

        state.ResumeTiming();
        algorithms::splitSegment(testImg,historyVisited,queueEdges);
        state.PauseTiming();
    }
}

static void BM_Add_Marker_5_Cell2_UPDATE_HIERARCHY(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg", testim);
        testImg.init();


        int marker, up = 0;
        QBT &qbt = testImg.getHierarchy().getQBT();
        int *parent = qbt.getParents();
        std::vector<int> queueEdges;
        auto mstL = testImg.getGraph().getMst();

        for (int i = 0; i < nbMarkers1Cell2; i++) {
            marker = markers1Cell2[i];
            up = parent[marker];
            while (up != -1) {
                testImg.marks_[up]++;
                if (testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)] = true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        queueEdges.clear();

        marker, up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for (int i = 0; i < nbMarkers2Cell2; i++) {
            marker = markers2Cell2[i];
            up = parent[marker];
            while (up != -1) {
                testImg.marks_[up]++;
                if (testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)] = true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }

        queueEdges.clear();

        marker, up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers3Cell2;i++){
            marker = markers3Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]--;
                if(testImg.marks_[up] == 1) {
                    testImg.ws_[testImg.getEdge(up)]= false;
                    testImg.mstEdit_[testImg.getEdge(up)]= true;
                    break;
                }
                up = parent[up];
            }
        }

        queueEdges.clear();

        marker, up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for(int i = 0; i < nbMarkers4Cell2;i++){
            marker = markers4Cell2[i];
            up = parent[marker];
            while(up != -1){
                testImg.marks_[up]--;
                if(testImg.marks_[up] == 1) {
                    testImg.ws_[testImg.getEdge(up)]= false;
                    testImg.mstEdit_[testImg.getEdge(up)]= true;
                    break;
                }
                up = parent[up];
            }
        }

        state.ResumeTiming();

        marker, up = 0;
        qbt = testImg.getHierarchy().getQBT();
        parent = qbt.getParents();
        mstL = testImg.getGraph().getMst();

        for (int i = 0; i < nbMarkers5Cell2; i++) {
            marker = markers5Cell2[i];
            up = parent[marker];
            while (up != -1) {
                testImg.marks_[up]++;
                if (testImg.marks_[up] == 2) {
                    testImg.ws_[testImg.getEdge(up)] = true;
                    testImg.mstEdit_[testImg.getEdge(up)] = false;
                    //queueEdges.push_back(mstL[testImg.getEdge(up)]);
                    break;
                }
                up = parent[up];
            }
        }
        state.PauseTiming();
    }
}

#define bench_num 100
// Register the function as a benchmark
BENCHMARK(BM_Add_Marker_1_Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Add_Marker_1_Cell2_MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Add_Marker_2_Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Add_Marker_2_Cell2_MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Remove_Marker_3_Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Remove_Marker_3_Cell2_MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Remove_Marker_4_Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Remove_Marker_4_Cell2_MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Add_Marker_5_Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Add_Marker_5_Cell2_MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Add_Marker_1_Cell2_MP_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Add_Marker_1_Cell2_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Add_Marker_1_Cell2_UPDATE_HIERARCHY)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Add_Marker_2_Cell2_MP_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Add_Marker_2_Cell2_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Add_Marker_2_Cell2_UPDATE_HIERARCHY)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Remove_Marker_3_Cell2_MP_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Remove_Marker_3_Cell2_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Remove_Marker_3_Cell2_UPDATE_HIERARCHY)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Remove_Marker_4_Cell2_MP_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Remove_Marker_4_Cell2_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Remove_Marker_4_Cell2_UPDATE_HIERARCHY)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Add_Marker_5_Cell2_MP_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_Add_Marker_5_Cell2_LABELLING)->Unit(benchmark::kMillisecond)->Iterations(bench_num);

BENCHMARK(BM_Add_Marker_5_Cell2_UPDATE_HIERARCHY)->Unit(benchmark::kMillisecond)->Iterations(bench_num);


BENCHMARK_MAIN();