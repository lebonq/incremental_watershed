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
                    mst.push_back(edge);//add edge in mst

                    temp[edge] = count;// Allow us to know where an edge is in the MST
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
    auto mstL = im.getGraph().getMst();

    for(int i = 0; i < nbMarkers;i++){
        marker = markers[i];
        up = parent[marker];
        while(up != -1){
            im.marks_[up]--;
            if(im.marks_[up] == 1) {
                im.ws_[im.getEdge(up)]= false;
                im.mstEdit_[im.getEdge(up)]= true;
                //mergeSegment(mstL[im.getEdge(up)],im);
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

/**
 *
 * @param im Partition in the state we want to show it
 * @param nameOfImage
 */
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

int algorithms::breadthFirstSearchLabelMP(imageManager& im, int p,int currentBlock, int xstart,int xend, int ystart, int yend, int* parentTmp,
                                          std::map<int,int>* hVerticesQueue,std::map<int,int>* vVerticesQueue) {
    std::vector<int> queue;
    queue.push_back(p);
    int count = 1;
    int w = im.getWidth();
    int h = im.getHeight();
    int wh = w*h;
    int tmpX;
    int tmpY;
    bool checkBlock;
    int root = p;

    int WB = 4;
    int HB = 4;

    int v,vRight,vLeft,vUp,vDown;
    while(!queue.empty()){
        v = queue.back();
        queue.pop_back();

        vRight = v + 1;
        vLeft = v - 1;
        vUp = v - w;
        vDown = v + w;


        tmpX = vRight%w;
        tmpY = (vRight-tmpX)/w;
        checkBlock = (xstart <= tmpX && tmpX <= xend) && (ystart <= tmpY && tmpY <= yend);

        if(vRight < wh && (v+1)%w != 0 && checkBlock == true){//check if adjacent to v exist and is in block
            if(im.mstEdit_[im.indexTemp[2*v]] == true && im.partitionMP_.findCanonical(vRight) != root){//If yes we check if the edge is revealnt and present in MST
                queue.push_back(vRight);
                parentTmp[vRight] = root;
                count++;
            }
        }
        if(checkBlock == false && vRight < wh && (v+1)%w != 0  &&  im.mstEdit_[im.indexTemp[2*v]] == true){
            hVerticesQueue[currentBlock+1].insert(std::pair<int,int>(vRight,root));
        }


        tmpX = vLeft%w;
        tmpY = (vLeft-tmpX)/w;
        checkBlock = (xstart <= tmpX && tmpX <= xend) && (ystart <= tmpY && tmpY <= yend);

        if(vLeft >= 0 && v%w != 0 && checkBlock == true){
            if(im.mstEdit_[im.indexTemp[(2*v)-2]] == true && im.partitionMP_.findCanonical(vLeft) != root){
                queue.push_back(vLeft);
                parentTmp[vLeft] = root;
                count++;
            }
        }

        tmpX = vDown%w;
        tmpY = (vDown-tmpX)/w;
        checkBlock = (xstart <= tmpX && tmpX <= xend) && (ystart <= tmpY && tmpY <= yend);

        if(vDown < wh && checkBlock == true){
            if(im.mstEdit_[im.indexTemp[(2*v)+1]] == true && im.partitionMP_.findCanonical(vDown) != root){
                queue.push_back(vDown);
                parentTmp[vDown] = root;
                count++;
            }
        }
        if(checkBlock == false && vDown < wh &&  im.mstEdit_[im.indexTemp[(2*v)+1]] == true){
            vVerticesQueue[currentBlock+WB].insert(std::pair<int,int>(vDown,root));
        }

        tmpX = vUp%w;
        tmpY = (vUp-tmpX)/w;
        checkBlock = (xstart <= tmpX && tmpX <= xend) && (ystart <= tmpY && tmpY <= yend);

        if(vUp >= 0 && checkBlock == true){
            if(im.mstEdit_[im.indexTemp[((2*v)-(2*w))+1]] == true && im.partitionMP_.findCanonical(vUp) != root){
                queue.push_back(vUp);
                parentTmp[vUp] = root;
                count++;
            }
        }



    }
    im.partitionMP_.getParents()[root] = -1;
    return count;
}

void algorithms::splitSegmentMP(imageManager& im, std::vector<int> queueEdges) {
    int p1, p2, block1, block2;
    int WB = 4; // Number of blocks in the width and height of the image
    int HB = 4;
    int *parentTmp = im.partitionMP_.getParents();
    int w = im.getWidth(); //size of the image
    int h = im.getHeight();

    int wb = w / WB;
    int hb = h / HB;

    //int* statut = new int[WB*HB](); //0 waiting 1 in progress 2 done
    //int* lock = new int[WB*HB](); //0 not locked 1 locked (Only thread one can touch it)

    //int* wait = new int[WB*HB](); //1 have to wait 0 can go (Only thread one can touch it)

    //tbb::concurrent_vector<int> *blockQueue = new tbb::concurrent_vector<int>[WB * HB];

    std::map<int,int> *hVerticesQueue = new std::map<int,int>[WB * HB];
    std::map<int,int> *vVerticesQueue = new std::map<int,int>[WB * HB];
    for (int i = 0; i < WB * HB; i++) {
        //blockQueue[i] = tbb::concurrent_vector<int>();
        hVerticesQueue[i] = std::map<int,int>();
        vVerticesQueue[i] = std::map<int,int>();
    }


    //Filtering point to put them in the right block
    /*for (auto edge: queueEdges) {
        if (((edge) & (1 << (0))) == 1) {//if odd or even not the same formula
            //Copy and paste of line #22 to avoid doing jump into memory
            p1 = edge / 2;
            p2 = (edge / 2) + w;
        } else {
            p1 = (edge + 1) / 2;
            p2 = ((edge + 1) / 2) + 1;
        }

        //TODO Check if this edge is between block
        block1 = ((p1 % w) / wb) + (((p1 / w) / hb) * WB);
        //std::cout << "block1 : " << block1 << " p1 : " << p1 << std::endl;
        blockQueue[block1].push_back(p1);
        statut[block1] = 1;
        block2 = ((p2 % w) / wb) + (((p2 / w) / hb) * WB);
        statut[block2] = 1;
        //std::cout << "block2 : " << block2 << " p2 : " << p2 << std::endl;
        blockQueue[block2].push_back(p2);
    }*/

    #pragma omp parallel num_threads(16)
    {
        int xstart, xend, ystart, yend, blockColumn, blockRow;
        bool selfLock = false;
        int v;
        int currentBlock = omp_get_thread_num();

        blockColumn = currentBlock % WB;
        blockRow = (currentBlock - blockColumn) / WB;

        xstart = wb * blockColumn;
        xend = wb * blockColumn + (wb - 1);

        ystart = hb * blockRow;
        yend = hb * blockRow + (hb - 1);

        if(blockRow == HB-1){
            yend = h-1;
        }

        //int idxCur = 0;
        //while(lock[currentBlock] == 0){

                /*if(statut[currentBlock]  == 1 && selfLock == false){
                    int size = blockQueue[currentBlock].size();
                    while(idxCur < size){
                        v = blockQueue[currentBlock][idxCur];
                        breadthFirstSearchLabelMP(im, v,currentBlock, xstart, xend, ystart, yend, parentTmp,statut,wait,blockQueue);
                        idxCur++;
                    }

                    statut[currentBlock] = 0;
                }*/

                 for(int r = ystart; r <= yend;r++) {
                    for (int v = r * w + xstart; v <= r * w + xend; v++) {
                        if (parentTmp[v] == -2) { //if not visited parent == -2
                            breadthFirstSearchLabelMP(im, v, currentBlock, xstart, xend, ystart, yend, parentTmp,hVerticesQueue,vVerticesQueue);
                        }
                    }
                }
                /*statut[currentBlock] = 1;

                if(currentBlock == 0){
                    bool check = true;
                    for(int i = 0; i < WB*HB; i++){
                        if(statut[i] == 0){
                            check = false;
                        }
                    }
                    if(check == true){
                        for(int i = 0; i < WB*HB; i++){
                            lock[i] = 1;
                        }
                    }
                }
        }*/

            /*#pragma omp critical (Print)
            {
                std::cout << "Block " << currentBlock << " is column " << blockColumn << " and row " << blockRow
                          << std::endl;
                std::cout << "Block " << currentBlock << " is xstart " << xstart << " and xend " << xend
                          << std::endl;
                std::cout << "Block " << currentBlock << " is ystart " << ystart << " and yend " << yend
                          << std::endl;
                std::cout << "lock : " << lock << std::endl;
            }*/
        /*
        #pragma omp critical (BlockStack)
        {
            end = queueBlock.empty();
        }
    }*/


    }

    #pragma omp parallel num_threads(16)
    {
        int v,vl;
        int currentBlock = omp_get_thread_num();
        for(auto it = hVerticesQueue[currentBlock].begin();
            it != hVerticesQueue[currentBlock].end();
            it++){
            v = im.partitionMP_.findCanonical(it->first);
            parentTmp[v] = it->second;
        }
    }

    #pragma omp parallel num_threads(16)
    {
        int v,vu;
        int currentBlock = omp_get_thread_num();
        for(auto it = vVerticesQueue[currentBlock].begin();
            it != vVerticesQueue[currentBlock].end();
            it++){
            v = im.partitionMP_.findCanonical(it->first);
            parentTmp[v] = it->second;
        }
    }

}

void algorithms::mergeSegmentMP(imageManager& im, std::vector<int> queueEdges) {

}

void algorithms::removeMarkerMP(imageManager & im,int* markers,int nbMarkers) {
    int marker,up = 0;
    QBT& qbt = im.getHierarchy().getQBT();
    int* parent = qbt.getParents();
    std::vector<int> queueEdges;
    auto mstL = im.getGraph().getMst();

    for(int i = 0; i < nbMarkers;i++){
        marker = markers[i];
        up = parent[marker];
        while(up != -1){
            im.marks_[up]--;
            if(im.marks_[up] == 1) {
                im.ws_[im.getEdge(up)]= false;
                im.mstEdit_[im.getEdge(up)]= true;
                queueEdges.push_back(mstL[im.getEdge(up)]);
                //mergeSegment(mstL[im.getEdge(up)],im);
                break;
            }
            up = parent[up];
        }
    }

    mergeSegmentMP(im, queueEdges);
}

void algorithms::addMarkerMP(imageManager & im,int* markers,int nbMarkers) {
    int marker,up = 0;
    QBT& qbt = im.getHierarchy().getQBT();
    int* parent = qbt.getParents();
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

    splitSegmentMP(im,queueEdges);
}

void algorithms::showSegmentationMP(imageManager & im,std::string nameOfImage) {
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
            seed = im.partitionMP_.findCanonical(cpt);
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