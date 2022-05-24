//
// Created by lebon on 12/05/2022.
//
#include "graph.h"
#include "imageManager.h"
#include "algorithms.h"
#include "dataStructures/QBT.h"
#include "dataStructures/QT.h"
#include "dataStructures/QEBT.h"

int main(int argc, char* argv[])
{
    auto testim = cv::imread("coins.gray.png",cv::IMREAD_GRAYSCALE);
    //todo declare inside the imageManager
    auto testImg = imageManager("coins.gray.png",testim);
    testImg.toGraph();
    auto QbtImage = QEBT(testImg.getGraph().getNbVertex());
    algorithms::kruskal(testImg.getGraph(),QbtImage,252,312);

    // test MST PWK
    auto graphTestQBT = graph(8);
    graphTestQBT.setWeight(0,2);
    graphTestQBT.setWeight(1,0);
    graphTestQBT.setWeight(2,2);
    graphTestQBT.setWeight(3,2);
    graphTestQBT.setWeight(4,0);
    graphTestQBT.setWeight(5,3);
    graphTestQBT.setWeight(6,-1);
    graphTestQBT.setWeight(7,1);
    graphTestQBT.setWeight(8,1);
    graphTestQBT.setWeight(9,-1);
    graphTestQBT.setWeight(10,2);
    graphTestQBT.setWeight(11,-1);
    graphTestQBT.setWeight(12,0);
    graphTestQBT.setWeight(13,-1);
    graphTestQBT.setWeight(14,-1);
    graphTestQBT.setWeight(15,-1);
    graphTestQBT.init_sortedEdges();
    auto Qbttest = QEBT(graphTestQBT.getNbVertex());

    algorithms::kruskal(graphTestQBT,Qbttest,4,2);

    //test random graph
    /*auto graphTestQBT = graph(9);
    graphTestQBT.setWeight(0,2);
    graphTestQBT.setWeight(1,0);
    graphTestQBT.setWeight(2,2);
    graphTestQBT.setWeight(3,3);
    graphTestQBT.setWeight(4,-1);
    graphTestQBT.setWeight(5,3);
    graphTestQBT.setWeight(6,0);
    graphTestQBT.setWeight(7,2);
    graphTestQBT.setWeight(8,1);
    graphTestQBT.setWeight(9,2);
    graphTestQBT.setWeight(10,-1);
    graphTestQBT.setWeight(11,1);
    graphTestQBT.setWeight(12,0);
    graphTestQBT.setWeight(13,-1);
    graphTestQBT.setWeight(14,1);
    graphTestQBT.setWeight(15,-1);
    graphTestQBT.setWeight(16,-1);
    graphTestQBT.setWeight(17,-1);
    graphTestQBT.init_sortedEdges();
    auto Qbttest = QBT(graphTestQBT.getNbVertex());

    algorithms::kruskal(graphTestQBT,Qbttest,3,3);*/

    return 0;
}