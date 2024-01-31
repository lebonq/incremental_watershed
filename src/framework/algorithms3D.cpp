//
// Created by lebonq on 31/01/24.
//

#include "algorithms3D.h"

int algorithms3D::breadthFirstSearchLabel(volumeManager &vol, int tag, int p) {
    std::vector<int> queue;
    queue.push_back(p);
    int count = 1;
    int w = vol.getWidth();
    int h = vol.getHeight();
    int d = vol.getDepth();
    int wh = w * h;

    auto segments = vol.getSegments();
    auto mstEdit = vol.getMstEdit();
    auto map_graph_mst = vol.getMapGraphMst();


    int v, vRight, vLeft, vUp, vDown, vFoward, vBackward;
    while (!queue.empty()) {
        v = queue.back();
        queue.pop_back();
        segments[v] = tag;

        vRight = v + 1;
        vLeft = v - 1;
        vUp = v - w;
        vDown = v + w;
        vFoward = v + wh;
        vBackward = v - wh;

        if (vRight < wh && (v + 1) % w != 0) {//check if adjacent to v exist
            if (mstEdit[map_graph_mst[2 * v]] == true &&
                segments[vRight] != tag) {//If yes we check if the edge is revealnt and present in MST
                queue.push_back(vRight);
                segments[vRight] = tag;
                count++;
                }
        }

        if (vLeft >= 0 && v % w != 0) {
            if (mstEdit[map_graph_mst[(2 * v) - 2]] == true && segments[vLeft] != tag) {
                queue.push_back(vLeft);
                segments[vLeft] = tag;
                count++;
            }
        }

        if (vDown < wh) {
            if (mstEdit[map_graph_mst[(2 * v) + 1]] == true && segments[vDown] != tag) {
                queue.push_back(vDown);
                segments[vDown] = tag;
                count++;
            }
        }

        if (vUp >= 0) {
            if (mstEdit[map_graph_mst[((2 * v) - (2 * w)) + 1]] == true && segments[vUp] != tag) {
                queue.push_back(vUp);
                segments[vUp] = tag;
                count++;
            }
        }




    }
    return count;
}

void algorithms3D::kruskal(graph &G, Q &Q, int w, int h, int *temp) {

    //Creation of sigleton for each vertex
    for (int i = 0; i < G.getNbVertex(); i++) {
        Q.makeSet(i);
    }

    std::vector<int> &mst = G.getMst();
    int count = 0;

    int prev = 1;

    // Loop over all possible pixel intensity values from 0 to 255.
    // This is done because the algorithm is processing the graph edges based on their weights,

    for (int j = 0; j < 256; j++) {
        for (int i = 0; i < G.count_[j]; i++) {
            int cx, cy = 0;
            int edge = G.sortedEdges_[j][i];

            //std::cerr << "edge : " << edge << std::endl;

            int dir = edge % 3;//Compute which direction the edge is

            cx = (edge-dir)/3;//Compute the orign of the edge

            switch (dir) {//Compute the destination of the edge
            case 0:
                cy = cx + 1;
                break;
            case 1:
                cy = cx + w;
                break;
            case 2:
                cy = cx + w*h;
                break;
            }

            //std::cerr << "cx : " << cx << " cy : " << cy << std::endl;

            cx = Q.findCannonical(cx);
            cy = Q.findCannonical(cy);
            //std::cout << "Canonical : cx : " << cx << " cy : " << cy << std::endl;

            if (cx != cy) {
                Q.makeUnion(cx, cy);
                mst.push_back(edge);//add edge in mst

                temp[edge] = count;// Allow us to know where an edge is in the MST
                count++;
            }
        }
    }
    //std::cerr << "There is " << count << " edges in the hierarchy" << std::endl;
}

void algorithms3D::splitSegment(volumeManager &vol, bool *historyVisited,
                              std::vector<int> queueEdges) {
    int p1, p2, tag1, tag2, newTag; //Are being erased
    int w = vol.getWidth();
    std::vector<int> seg = vol.getSegments();
    std::vector<int> sizeSeg = vol.getSizePart();

    for (int edge: queueEdges) {
        if (((edge) & (1 << (0))) == 1) {//if odd or even not the same formula
            //Copy and paste of line #22 to avoid doing jump into memory
            p1 = edge / 2;
            p2 = (edge / 2) + w;
        } else {
            p1 = (edge + 1) / 2;
            p2 = ((edge + 1) / 2) + 1;
        }

        tag1 = seg[p1];
        tag2 = seg[p2];

        if (!historyVisited[tag2]) {
            newTag = vol.getTagCount();
            vol.setTagCount(newTag+1);

            sizeSeg[newTag] = breadthFirstSearchLabel(vol, newTag, p2);
            sizeSeg[tag2] -= sizeSeg[newTag];

            historyVisited[newTag] = true;
        }

        if (!historyVisited[tag1]) {

            newTag = vol.getTagCount();
            vol.setTagCount(newTag+1);

            sizeSeg[newTag] = breadthFirstSearchLabel(vol, newTag, p1);
            sizeSeg[tag1] -= sizeSeg[newTag];

            historyVisited[newTag] = true;

        }
    }
}

/**
 * Merge 2 segments
 * @param p1 Point on the first segment
 * @param p2 Point on the second segment
 * @param vol
 */
void algorithms3D::mergeSegment(int edge, volumeManager &vol) {
    std::vector<int> seg = vol.getSegments();
    std::vector<int> sizeSeg = vol.getSizePart();
    int p1, p2;

    if (((edge) & (1 << (0))) == 1) {//if odd or even not the same formula
        //Copy and paste of line #22 to avoid doing jump into memory
        p1 = edge / 2;
        p2 = (edge / 2) + vol.getWidth();
    } else {
        p1 = (edge + 1) / 2;
        p2 = ((edge + 1) / 2) + 1;
    }

    int tag1 = seg[p1];
    int tag2 = seg[p2];

    if (sizeSeg[tag1] < sizeSeg[tag2]) {
        int tmp = tag1;
        tag1 = tag2;
        tag2 = tmp;
    }

    sizeSeg[tag1] += sizeSeg[tag2];
    sizeSeg[tag2] = 0;

    int count = breadthFirstSearchLabel(vol, tag1, p2);

}

void algorithms3D::removeMarker(volumeManager &vol, int *markers, int nbMarkers) {

    int marker, up = 0;
    QBT &qbt = vol.getHierarchy().getQBT();
    int *parent = qbt.getParents();
    auto mstL = vol.getGraph().getMst();
    auto marks = vol.getMarks();
    auto ws = vol.getWs();
    auto mstEdit = vol.getMstEdit();

    for (int i = 0; i < nbMarkers; i++) {
        marker = markers[i];
        up = parent[marker];
        while (up != -1) {
            vol.marks_[up]--;
            if (vol.marks_[up] == 1) {
                vol.ws_[vol.getEdge(up)] = false;
                vol.mstEdit_[vol.getEdge(up)] = true;
                mergeSegment(mstL[vol.getEdge(up)], vol);
                break;
            }
            up = parent[up];
        }
    }

}

void algorithms3D::addMarker(volumeManager &im, int *markers, int nbMarkers) {

    int marker, up = 0;
    QBT &qbt = im.getHierarchy().getQBT();
    int *parent = qbt.getParents();
    bool *historyVisited = new bool[im.getGraph().getNbVertex()]();

    std::vector<int> queueEdges;
    auto mstL = im.getGraph().getMst();

    for (int i = 0; i < nbMarkers; i++) {
        marker = markers[i];
        up = parent[marker];
        while (up != -1) {
            im.marks_[up]++;
            if (im.marks_[up] == 2) {
                im.ws_[im.getEdge(up)] = true;
                im.mstEdit_[im.getEdge(up)] = false;
                queueEdges.push_back(mstL[im.getEdge(up)]);
                break;
            }
            up = parent[up];
        }
    }

    splitSegment(im, historyVisited, queueEdges);

    delete[] historyVisited;

}
