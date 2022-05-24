//
// Created by lebon on 17/05/2022.
//

#include "algorithms.h"

void algorithms::kruskal(graph& G,Q& Q, int w, int h) {

    std::vector<int> test;

    for (int i = 0; i < G.getNbVertex(); i++) {
        Q.makeSet(i);
    }

    //std::cout << G.getNbVertex() << std::endl;
    int evenOrOdd = w%2;
    for (int j = 0; j < max_value_count; j++) {
        for(int i = 0; i < G.count_[j]; i++) {
            int cx, cy = 0;
            int edge = G.sortedEdges_[j][i];

            //std::cout << "edge : " << edge << std::endl;

            if(((edge) & (1<<(0))) == 1){//if odd or even not the same formula
                cx = edge/2;
                cy = (edge/2)+w;
            }
            else{
                cx = (edge+1)/2;
                cy = ((edge+1)/2)+1;
            }

                //std::cout << "cx : " << cx << " cy : " << cy << std::endl;
                cx = Q.findCannonical(cx);
                cy = Q.findCannonical(cy);
                //std::cout << "Canonical : cx : " << cx << " cy : " << cy << std::endl;

                if(cx != cy) {
                    Q.makeUnion(cx, cy);
                    test.push_back(edge);//add in the MST
                    //TODO MST data structure
                }

        }

    }
    //Print the MST

    for(auto i : test){
        //std::cout << i << ",";
    }
    //std::cout << std::endl;
}


