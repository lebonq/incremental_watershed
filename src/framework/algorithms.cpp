//
// Created by lebon on 17/05/2022.
//

#include "algorithms.h"

void algorithms::kruskal(graph& G,Q& Q, int w,int* temp) {

    for (int i = 0; i < G.getNbVertex(); i++) {
        Q.makeSet(i);
    }
    std::vector<int>& mst = G.getMst();
    int count = 0;

    //std::cout << G.getNbVertex() << std::endl;
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
                    mst.push_back(edge);

                    temp[edge] = count;
                    count++;
                }

        }
    }
    //Print the MST
    /*
    for(int i = 0; i < G.getNbVertex()*2; i++) {
        if(G.getMst()[i]) {
            std::cout << i << " ";
        }
    }*/
}

int algorithms::breadthFirstSearchLabel(imageManager& im, int tag, int p){
    std::vector<int> queue;
    queue.push_back(p);
    int count = 1;
    int w = im.getWidth();
    int h = im.getHeight();
    int wh = w*h;


    int v,vRight,vLeft,vUp,vDown;
    while(!queue.empty()){
        v = queue.back();
        queue.pop_back();
        im.segments_[v] = tag;

        vRight = v + 1;
        vLeft = v - 1;
        vUp = v - w;
        vDown = v + w;

        if(vRight < wh && (v+1)%w != 0 ){//check if adjacent to v exist
            if(im.mstEdit_[im.indexTemp[2*v]] == true && im.segments_[vRight] != tag){//If yes we check if the edge is revealnt and present in MST
                queue.push_back(vRight);
                im.segments_[vRight] = tag;
                count++;
            }
        }// 418708 418707 not in mst edit

        if(vLeft >= 0 && v%w != 0 ){
            if(im.mstEdit_[im.indexTemp[(2*v)-2]] == true && im.segments_[vLeft] != tag){
                queue.push_back(vLeft);
                im.segments_[vLeft] = tag;
                count++;
            }
        }

        if(vDown < wh){
            if(im.mstEdit_[im.indexTemp[(2*v)+1]] == true && im.segments_[vDown] != tag){
                queue.push_back(vDown);
                im.segments_[vDown] = tag;
                count++;
            }
        }

        if(vUp >= 0){
            if(im.mstEdit_[im.indexTemp[((2*v)-(2*w))+1]] == true && im.segments_[vUp] != tag){
                queue.push_back(vUp);
                im.segments_[vUp] = tag;
                count++;
            }
        }

    }
    return count;
}

void algorithms::splitSegment(imageManager & im, bool* historyVisited,
                              std::vector<int> queueEdges) {
    int p1,p2,tag1,tag2,newTag; //Are being erased
    int w = im.getWidth();
    int* seg = im.segments_;
    int * sizeSeg = im.sizePart_;

    for(int edge : queueEdges){
        if(((edge) & (1<<(0))) == 1){//if odd or even not the same formula
            //Copy and paste of line #22 to avoid doing jump into memory
            p1 = edge/2;
            p2 = (edge/2)+w;
        }
        else{
            p1 = (edge+1)/2;
            p2 = ((edge+1)/2)+1;
        }

        tag1 = seg[p1];
        tag2 = seg[p2];

        if(!historyVisited[tag2]){
            if(im.tag_.empty()){
                newTag = im.tagCount_;
                im.tagCount_++;
            }
            else{
                newTag = im.tag_.back();
                im.tag_.pop_back();
            }
            sizeSeg[newTag] = breadthFirstSearchLabel(im,newTag,p2);
            sizeSeg[tag2] -= sizeSeg[newTag];
            if(sizeSeg[tag2] <= 0){
                im.tag_.push_back(tag2);
            }
            historyVisited[newTag] = true;
        }

        if(!historyVisited[tag1]){
            if(im.tag_.empty()){
                newTag = im.tagCount_;
                im.tagCount_++;
            }
            else{
                newTag = im.tag_.back();
                im.tag_.pop_back();
            }
            sizeSeg[newTag] = breadthFirstSearchLabel(im,newTag,p1);
            sizeSeg[tag1] -= sizeSeg[newTag];
            if(sizeSeg[tag1] <= 0){
                im.tag_.push_back(tag1);
            }
            historyVisited[newTag] = true;
        }
    }
}

/**
 * Merge 2 segments
 * @param p1 Point on the first segment
 * @param p2 Point on the second segment
 * @param im
 */
void algorithms::mergeSegment(int edge, imageManager & im) {
    int* seg = im.segments_;
    int * sizeSeg = im.sizePart_;
    int p1,p2;

    if(((edge) & (1<<(0))) == 1){//if odd or even not the same formula
        //Copy and paste of line #22 to avoid doing jump into memory
        p1 = edge/2;
        p2 = (edge/2)+im.getWidth();
    }
    else{
        p1 = (edge+1)/2;
        p2 = ((edge+1)/2)+1;
    }

    int tag1 = seg[p1];
    int tag2 = seg[p2];

    if(sizeSeg[tag1] < sizeSeg[tag2]){
        int tmp = tag1;
        tag1 = tag2;
        tag2 = tmp;
    }

    sizeSeg[tag1] += sizeSeg[tag2];
    sizeSeg[tag2] = 0;
    im.tag_.push_back(tag2);

    breadthFirstSearchLabel(im,tag1,p2);
}

void algorithms::removeMarker(imageManager & im,int* markers,int nbMarkers) {
    int marker,up = 0;
    QBT& qbt = im.getHierarchy().getQBT();
    int* parent = qbt.getParents();
    int sizeParent = qbt.getSize();
    auto mstL = im.getGraph().getMst();

    for(int i = 0; i < nbMarkers;i++){
        marker = markers[i];
        up = parent[marker];
        while(up != -1){
            im.marks_[up]--;
            if(im.marks_[up] == 1) {
                im.ws_[im.getEdge(up)]= false;
                im.mstEdit_[im.getEdge(up)]= true;
                mergeSegment(mstL[im.getEdge(up)],im);
                break;
            }
            up = parent[up];
        }
    }
}

void algorithms::addMarker(imageManager & im,int* markers,int nbMarkers) {
    int marker,up = 0;
    QBT& qbt = im.getHierarchy().getQBT();
    int* parent = qbt.getParents();
    int sizeParent = qbt.getSize();
    bool* historyVisited = new bool[im.getGraph().getNbVertex()]();
    std::vector<int> queueEdges;
    auto mstL = im.getGraph().getMst();

    for(int i = 0; i < nbMarkers;i++){
        marker = markers[i];
        up = parent[marker];
        while(up != -1){
            im.marks_[up]++;
            if(im.marks_[up] == 2) {
                im.ws_[im.getEdge(up)]= true;
                im.mstEdit_[im.getEdge(up)] = false;
                queueEdges.push_back(mstL[im.getEdge(up)]);
                break;
            }
            up = parent[up];
        }
    }

    splitSegment(im,historyVisited,queueEdges);

    delete[] historyVisited;
}

void algorithms::showSegmentation(imageManager & im,std::string nameOfImage) {
    cv::Mat img(im.getHeight(), im.getWidth(),
                 CV_8UC3, cv::Vec3b(255, 0, 0));
    // Verify if image is created or not
    if(img.empty()){
        std::cout << "Could not load image" << std::endl;
        std::cin.get();
    }

    img.at<cv::Vec3b>(0, 0) = cv::Vec3b(0, 0, 0);

    int cpt = 0,seed;
    for(int y = 0; y < im.getHeight(); y++){
        for(int x = 0; x < im.getWidth(); x++){
            seed = im.segments_[cpt];
            srand(seed);
            img.at<cv::Vec3b>(y, x) = cv::Vec3b(rand()%255, rand()%255, rand()%255);
            cpt++;
        }
    }

    namedWindow(nameOfImage, cv::WINDOW_AUTOSIZE);

    imshow(nameOfImage, img);
    // wait for any keypress
    cv::waitKey(0);
}