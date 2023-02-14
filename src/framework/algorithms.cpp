//
// Created by lebon on 17/05/2022.
//

#include "algorithms.h"

//#define PIXELS_COUNT

int nb_pixels_visited = 0;

void algorithms::kruskal(graph &G, Q &Q, int w, int *temp) {

    for (int i = 0; i < G.getNbVertex(); i++) {
        Q.makeSet(i);
    }
    std::vector<int> &mst = G.getMst();
    int count = 0;

    //std::cout << G.getNbVertex() << std::endl;
    for (int j = 0; j < max_value_count; j++) {
        for (int i = 0; i < G.count_[j]; i++) {
            int cx, cy = 0;
            int edge = G.sortedEdges_[j][i];

            //std::cout << "edge : " << edge << std::endl;

            if (((edge) & (1 << (0))) == 1) {//if odd or even not the same formula
                cx = edge / 2;
                cy = (edge / 2) + w;
            } else {
                cx = (edge + 1) / 2;
                cy = ((edge + 1) / 2) + 1;
            }

            //std::cout << "cx : " << cx << " cy : " << cy << std::endl;
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
    //Print the MST
    /*
    for(int i = 0; i < G.getNbVertex()*2; i++) {
        if(G.getMst()[i]) {
            std::cout << i << " ";
        }
    }*/
}

int algorithms::breadthFirstSearchLabel(imageManager &im, int tag, int p) {
    std::vector<int> queue;
    queue.push_back(p);
    int count = 1;
    int w = im.getWidth();
    int h = im.getHeight();
    int wh = w * h;


    int v, vRight, vLeft, vUp, vDown;
    while (!queue.empty()) {
        v = queue.back();
        queue.pop_back();
        im.segments_[v] = tag;

        vRight = v + 1;
        vLeft = v - 1;
        vUp = v - w;
        vDown = v + w;

        if (vRight < wh && (v + 1) % w != 0) {//check if adjacent to v exist
            if (im.mstEdit_[im.indexTemp[2 * v]] == true &&
                im.segments_[vRight] != tag) {//If yes we check if the edge is revealnt and present in MST
                queue.push_back(vRight);
                im.segments_[vRight] = tag;
                count++;
            }
        }

        if (vLeft >= 0 && v % w != 0) {
            if (im.mstEdit_[im.indexTemp[(2 * v) - 2]] == true && im.segments_[vLeft] != tag) {
                queue.push_back(vLeft);
                im.segments_[vLeft] = tag;
                count++;
            }
        }

        if (vDown < wh) {
            if (im.mstEdit_[im.indexTemp[(2 * v) + 1]] == true && im.segments_[vDown] != tag) {
                queue.push_back(vDown);
                im.segments_[vDown] = tag;
                count++;
            }
        }

        if (vUp >= 0) {
            if (im.mstEdit_[im.indexTemp[((2 * v) - (2 * w)) + 1]] == true && im.segments_[vUp] != tag) {
                queue.push_back(vUp);
                im.segments_[vUp] = tag;
                count++;
            }
        }

    }
    return count;
}

void algorithms::splitSegment(imageManager &im, bool *historyVisited,
                              std::vector<int> queueEdges) {
    int p1, p2, tag1, tag2, newTag; //Are being erased
    int w = im.getWidth();
    int *seg = im.segments_;
    int *sizeSeg = im.sizePart_;

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
            if (im.tag_.empty()) {
                newTag = im.tagCount_;
                im.tagCount_++;
            } else {
                newTag = im.tag_.back();
                im.tag_.pop_back();
            }
            sizeSeg[newTag] = breadthFirstSearchLabel(im, newTag, p2);
            sizeSeg[tag2] -= sizeSeg[newTag];
            if (sizeSeg[tag2] <= 0) {
                im.tag_.push_back(tag2);
            }
            historyVisited[newTag] = true;
#ifdef PIXELS_COUNT
            nb_pixels_visited += sizeSeg[newTag];
#endif
        }

        if (!historyVisited[tag1]) {
            if (im.tag_.empty()) {
                newTag = im.tagCount_;
                im.tagCount_++;
            } else {
                newTag = im.tag_.back();
                im.tag_.pop_back();
            }
            sizeSeg[newTag] = breadthFirstSearchLabel(im, newTag, p1);
            sizeSeg[tag1] -= sizeSeg[newTag];
            if (sizeSeg[tag1] <= 0) {
                im.tag_.push_back(tag1);
            }
            historyVisited[newTag] = true;
#ifdef PIXELS_COUNT
            nb_pixels_visited += sizeSeg[newTag];
#endif
        }
    }
}

/**
 * Merge 2 segments
 * @param p1 Point on the first segment
 * @param p2 Point on the second segment
 * @param im
 */
void algorithms::mergeSegment(int edge, imageManager &im) {
    int *seg = im.segments_;
    int *sizeSeg = im.sizePart_;
    int p1, p2;

    if (((edge) & (1 << (0))) == 1) {//if odd or even not the same formula
        //Copy and paste of line #22 to avoid doing jump into memory
        p1 = edge / 2;
        p2 = (edge / 2) + im.getWidth();
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
    im.tag_.push_back(tag2);

    int count = breadthFirstSearchLabel(im, tag1, p2);

#ifdef PIXELS_COUNT
    nb_pixels_visited += count;
#endif
}

void algorithms::removeMarker(imageManager &im, int *markers, int nbMarkers) {
#ifdef PIXELS_COUNT
    nb_pixels_visited = 0;
#endif
    int marker, up = 0;
    QBT &qbt = im.getHierarchy().getQBT();
    int *parent = qbt.getParents();
    auto mstL = im.getGraph().getMst();

    for (int i = 0; i < nbMarkers; i++) {
        marker = markers[i];
        up = parent[marker];
        while (up != -1) {
            im.marks_[up]--;
            if (im.marks_[up] == 1) {
                im.ws_[im.getEdge(up)] = false;
                im.mstEdit_[im.getEdge(up)] = true;
                mergeSegment(mstL[im.getEdge(up)], im);
                break;
            }
            up = parent[up];
        }
    }
#ifdef PIXELS_COUNT
    std::cout << "nb_pixels_visited: " << nb_pixels_visited << std::endl;
#endif
}

void algorithms::addMarker(imageManager &im, int *markers, int nbMarkers) {
#ifdef PIXELS_COUNT
    nb_pixels_visited = 0;
#endif
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
#ifdef PIXELS_COUNT
    std::cout << "nb_pixels_visited: " << nb_pixels_visited << std::endl;
#endif
}

/**
 *
 * @param im Partition in the state we want to show it
 * @param nameOfImage
 */
void algorithms::showSegmentation(imageManager &im, std::string nameOfImage) {
    cv::Mat img(im.getHeight(), im.getWidth(),
                CV_8UC3, cv::Vec3b(255, 0, 0));
    // Verify if image is created or not
    if (img.empty()) {
        std::cout << "Could not load image" << std::endl;
        std::cin.get();
    }

    img.at<cv::Vec3b>(0, 0) = cv::Vec3b(0, 0, 0);

    int cpt = 0, seed;
    for (int y = 0; y < im.getHeight(); y++) {
        for (int x = 0; x < im.getWidth(); x++) {
            seed = im.segments_[cpt];
            srand(seed);
            img.at<cv::Vec3b>(y, x) = cv::Vec3b(rand() % 255, rand() % 255, rand() % 255);
            cpt++;
        }
    }

    namedWindow(nameOfImage, cv::WINDOW_AUTOSIZE);

    imshow(nameOfImage, img);
    // wait for any keypress
    cv::waitKey(0);
}

int algorithms::breadthFirstSearchLabelMP(imageManager &im, int p, int currentBlock, int xstart, int xend, int ystart,
                                          int yend, int *parentTmp,
                                          std::vector<int> *hVerticesQueue, std::vector<int> *vVerticesQueue) {
    std::vector<int> queue;
    queue.push_back(p);
    int count = 1;
    int w = im.getWidth();
    int h = im.getHeight();
    int wh = w * h;
    int tmpX;
    int tmpY;
    bool checkBlock;
    int root = p;

    int v, vRight, vLeft, vUp, vDown;
    while (!queue.empty()) {
        v = queue.back();
        queue.pop_back();

        vRight = v + 1;
        vLeft = v - 1;
        vUp = v - w;
        vDown = v + w;


        tmpX = vRight % w;
        tmpY = (vRight - tmpX) / w;
        checkBlock = (xstart <= tmpX && tmpX <= xend) && (ystart <= tmpY && tmpY <= yend);

        if (vRight < wh && (v + 1) % w != 0 && checkBlock == true) {//check if adjacent to v exist and is in block
            if (im.mstEdit_[im.indexTemp[2 * v]] == true && im.partitionMP_.findCanonical(vRight) !=
                                                            root) {//If yes we check if the edge is revealnt and present in MST
                queue.push_back(vRight);
                parentTmp[vRight] = root;
            }
        }


        tmpX = vLeft % w;
        tmpY = (vLeft - tmpX) / w;
        checkBlock = (xstart <= tmpX && tmpX <= xend) && (ystart <= tmpY && tmpY <= yend);

        if (vLeft >= 0 && v % w != 0 && checkBlock == true) {
            if (im.mstEdit_[im.indexTemp[(2 * v) - 2]] == true && im.partitionMP_.findCanonical(vLeft) != root) {
                queue.push_back(vLeft);
                parentTmp[vLeft] = root;
            }
        }
        if (checkBlock == false && vLeft >= 0 && v % w != 0 && im.mstEdit_[im.indexTemp[(2 * v) - 2]] == true) {
            hVerticesQueue[currentBlock].push_back(v);
        }

        tmpX = vDown % w;
        tmpY = (vDown - tmpX) / w;
        checkBlock = (xstart <= tmpX && tmpX <= xend) && (ystart <= tmpY && tmpY <= yend);

        if (vDown < wh && checkBlock == true) {
            if (im.mstEdit_[im.indexTemp[(2 * v) + 1]] == true && im.partitionMP_.findCanonical(vDown) != root) {
                queue.push_back(vDown);
                parentTmp[vDown] = root;
            }
        }

        tmpX = vUp % w;
        tmpY = (vUp - tmpX) / w;
        checkBlock = (xstart <= tmpX && tmpX <= xend) && (ystart <= tmpY && tmpY <= yend);

        if (vUp >= 0 && checkBlock == true) {
            if (im.mstEdit_[im.indexTemp[((2 * v) - (2 * w)) + 1]] == true &&
                im.partitionMP_.findCanonical(vUp) != root) {
                queue.push_back(vUp);
                parentTmp[vUp] = root;
            }
        }
        if (checkBlock == false && vUp >= 0 && im.mstEdit_[im.indexTemp[((2 * v) - (2 * w)) + 1]] == true) {
            vVerticesQueue[currentBlock].push_back(v);
        }


    }
    //im.partitionMP_.getParents()[root] = -1;
    return count;
}

void algorithms::splitSegmentMP(imageManager &im, std::vector<int> queueEdges) {
    int *parentTmp = im.partitionMP_.getParents();
    int w = im.getWidth(); //size of the image
    int h = im.getHeight();

    int wb = w / WB;
    int hb = h / HB;

    std::vector<int> *hVerticesQueue = new std::vector<int>[WB * HB];
    std::vector<int> *vVerticesQueue = new std::vector<int>[WB * HB];
    for (int i = 0; i < WB * HB; i++) {
        hVerticesQueue[i] = std::vector<int>();
        vVerticesQueue[i] = std::vector<int>();
    }

#pragma omp parallel num_threads(8)
    {
        int xstart, xend, ystart, yend, blockColumn, blockRow;
        int currentBlock = omp_get_thread_num();

        blockColumn = currentBlock % WB;
        blockRow = (currentBlock - blockColumn) / WB;

        xstart = wb * blockColumn;
        xend = wb * blockColumn + (wb - 1);

        ystart = hb * blockRow;
        yend = hb * blockRow + (hb - 1);

        if (blockRow == HB - 1) {
            yend = h - 1;
        }

        if (blockColumn == WB - 1) {
            xend = w - 1;
        }

        for (int r = ystart; r <= yend; r++) {
            for (int v = r * w + xstart; v <= r * w + xend; v++) {
                if (parentTmp[v] == -2) { //if not visited parent == -2
                    breadthFirstSearchLabelMP(im, v, currentBlock, xstart, xend, ystart, yend, parentTmp,
                                              hVerticesQueue, vVerticesQueue);
                }
            }
        }
    }

    //Horizontal merge
#pragma omp parallel num_threads(4)
    {

        int vl;
        int currentBlock = omp_get_thread_num();

        for (int i = 0; i < WB; i++) {
            for (auto v: hVerticesQueue[currentBlock * WB + i]) {
                vl = im.partitionMP_.findCanonicalPathCompression(v - 1);
                v = im.partitionMP_.findCanonicalPathCompression(v);
                parentTmp[v] = vl;
            }
        }
    }

    //Vertical merge
#pragma omp parallel num_threads(1)
    {
        int vu;
        int currentBlock = omp_get_thread_num();

        for (int j = 0; j < HB; j++) {
            for (int i = 0; i < WB; i++) {
                for (auto v: vVerticesQueue[currentBlock + i + WB * j]) {
                    vu = im.partitionMP_.findCanonicalPathCompression(v - w);
                    v = im.partitionMP_.findCanonicalPathCompression(v);
                    parentTmp[v] = vu;
                }
            }
        }
    }

    delete[] hVerticesQueue;
    delete[] vVerticesQueue;
}

void algorithms::mergeSegmentMP(imageManager &im, std::vector<int> queueEdges) {
    int p1, p2, s1, s2, si1, si2;
    int *parentTmp = im.partitionMP_.getParents();
    int w = im.getWidth(); //size of the image
    int h = im.getHeight();
    int n = im.getGraph().getNbVertex(); //number of vertices in the graph

    int indexInterval = (n - 1) / (WB * HB);

    std::vector<std::pair<int, int>> *queueMerge = new std::vector<std::pair<int, int>>[WB * HB];
    std::vector<std::pair<int, int>> queueBondary;

    for (int i = 0; i < WB * HB; i++) {
        queueMerge[i] = std::vector<std::pair<int, int>>();
    }

    for (auto edge: queueEdges) {
        if (((edge) & (1 << (0))) == 1) {//if odd or even not the same formula
            //Copy and paste of line #22 to avoid doing jump into memory
            p1 = edge / 2;
            p2 = (edge / 2) + w;
        } else {
            p1 = (edge + 1) / 2;
            p2 = ((edge + 1) / 2) + 1;
        }

        s1 = im.partitionMP_.findCanonicalPathCompression(p1);
        s2 = im.partitionMP_.findCanonicalPathCompression(p2);

        si1 = s1 / indexInterval;
        si2 = s2 / indexInterval;

        if (si1 == WB * HB) {
            si1 = WB * HB - 1;
        }
        if (si2 == WB * HB) {
            si2 = WB * HB - 1;
        }

        if (si1 == si2) {
            queueMerge[si1].push_back(std::pair<int, int>(s1, s2));
        } else {
            queueBondary.push_back(std::pair<int, int>(s1, s2));
        }
    }

#pragma omp parallel num_threads(8)
    {
        int idThread = omp_get_thread_num();
        for (auto edge: queueMerge[idThread]) {
            parentTmp[im.partitionMP_.findCanonicalPathCompression(
                    edge.second)] = im.partitionMP_.findCanonicalPathCompression(edge.first);
        }
    }

    for (auto edge: queueBondary) {
        parentTmp[im.partitionMP_.findCanonicalPathCompression(
                edge.first)] = im.partitionMP_.findCanonicalPathCompression(edge.second);
    }

    delete[] queueMerge;

}

void algorithms::removeMarkerMP(imageManager &im, int *markers, int nbMarkers) {
    int marker, up = 0;
    QBT &qbt = im.getHierarchy().getQBT();
    int *parent = qbt.getParents();
    std::vector<int> queueEdges;
    auto mstL = im.getGraph().getMst();

    for (int i = 0; i < nbMarkers; i++) {
        marker = markers[i];
        up = parent[marker];
        while (up != -1) {
            im.marks_[up]--;
            if (im.marks_[up] == 1) {
                im.ws_[im.getEdge(up)] = false;
                im.mstEdit_[im.getEdge(up)] = true;
                queueEdges.push_back(mstL[im.getEdge(up)]);
                break;
            }
            up = parent[up];
        }
    }

    mergeSegmentMP(im, queueEdges);
}

void algorithms::addMarkerMP(imageManager &im, int *markers, int nbMarkers) {
    std::fill_n(im.partitionMP_.getParents(), im.getGraph().getNbVertex(), -2); // Set segemnt to -2 by default

    int marker, up = 0;
    QBT &qbt = im.getHierarchy().getQBT();
    int *parent = qbt.getParents();
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

    splitSegmentMP(im, queueEdges);
}

void algorithms::showSegmentationMP(imageManager &im, std::string nameOfImage) {
    cv::Mat img(im.getHeight(), im.getWidth(),
                CV_8UC3, cv::Vec3b(255, 0, 0));
    // Verify if image is created or not
    if (img.empty()) {
        std::cout << "Could not load image" << std::endl;
        std::cin.get();
    }

    img.at<cv::Vec3b>(0, 0) = cv::Vec3b(0, 0, 0);

    int cpt = 0, seed;
    for (int y = 0; y < im.getHeight(); y++) {
        for (int x = 0; x < im.getWidth(); x++) {
            seed = im.partitionMP_.findCanonicalPathCompression(cpt);
            srand(seed);
            img.at<cv::Vec3b>(y, x) = cv::Vec3b(rand() % 255, rand() % 255, rand() % 255);
            cpt++;
        }
    }

    namedWindow(nameOfImage, cv::WINDOW_AUTOSIZE);

    imshow(nameOfImage, img);
    //Uncomment to save the image as a file
    //cv::imwrite(nameOfImage,img);
    // wait for any keypress
    cv::waitKey(0);
}

bool algorithms::get_vector_from_txt(const std::string &file_path, std::vector<int> &values) {

    std::string file_add = file_path + "_add.txt";
    std::string file_remove = file_path + "_remove.txt";
    //std::cout << file_add << std::endl;

    std::string line;       /* string to hold each line read from file  */
    std::ifstream f(file_add);                  /* file stream to read  */
    bool remove = false;

    if (!f.is_open()) {
        f = std::ifstream(file_remove);
        remove = true;
    }


    while (getline(f, line)) { /* read each line into line */
        /* if no digits in line - get next */
        if (line.find_first_of("0123456789") == std::string::npos)
            continue;
        int itmp;                               /* temporary int */
        std::stringstream ss(line);            /* stringstream from line */
        while (ss >> itmp) {                    /* read int from stringstream */
            std::string tmpstr;                 /* temporary string to ',' */
            values.push_back(itmp);                /* add int to tmp */
            if (!getline(ss, tmpstr, ','))     /* read to ',' w/tmpstr */
                break;                          /* done if no more ',' */
        }
    }

    return remove;

    /*std::cout << values.size() << std::endl;
    for (auto col : values)
        std::cout << "  " << col;
    std::cout << '\n';*/
}

void algorithms::get_tab_from_image(const std::string &file_path, std::vector<int> &values) {

    cv::Mat marker_image;


    try {
        marker_image = cv::imread(file_path + "_add.png", CV_8UC1);
        if (marker_image.empty()) {
            marker_image = cv::imread(file_path + "_remove.png",
                                      CV_8UC1);
        }
    } catch (cv::Exception e) { ;
        try {
            marker_image = cv::imread(file_path + "_remove.png",
                                      CV_8UC1);
        } catch (cv::Exception e) {
            std::cout << e.msg << std::endl;
            std::exit(e.code);
        }
    }

    cv::Mat flatMarker;
    marker_image.copyTo(flatMarker);
    flatMarker = flatMarker.reshape(1, 1);

    cv::Mat nonZeroCoordinates_flat, nonZeroCoordinates_all;

    findNonZero(flatMarker, nonZeroCoordinates_flat);
    int nbmarkers = nonZeroCoordinates_flat.total();

    //std::cout << *nbmarkers << std::endl;

    for (int i = 0; i < nbmarkers; i++) {
        values.push_back(nonZeroCoordinates_flat.at<cv::Point>(i).x);
    }

}

void algorithms::get_DIFT_seed_from_image(const std::string &file_path, const int id) {

    std::string fp_cur = file_path + std::to_string(id);
    std::string fp_prev = file_path + std::to_string(id-1);
    cv::Mat marker_image_cur;
    cv::Mat marker_image_prev;
    std::string name_out;

    bool remove = false;

    try {
        marker_image_cur = cv::imread(fp_cur + "_add.png", CV_8UC1);
        name_out = fp_cur + "_add_DIFT.txt";
        if (marker_image_cur.empty()) {
            marker_image_cur = cv::imread(fp_cur + "_remove.png",
                                      CV_8UC1);
            name_out = fp_cur + "_remove_DIFT.txt";
            remove = true;
        }
    } catch (cv::Exception e) { ;
        std::cout << e.msg << std::endl;
        std::exit(e.code);
    }

    try {
        marker_image_prev = cv::imread(fp_prev + "_add.png", CV_8UC1);
        if (marker_image_prev.empty()) {
            marker_image_prev = cv::imread(fp_prev + "_remove.png",
                                      CV_8UC1);
        }
    } catch (cv::Exception e) { ;
        std::cout << e.msg << std::endl;
        std::exit(e.code);
    }

    cv::Mat marker_image;
    if (id == 0) marker_image = marker_image_cur;
    else if( remove == true){
        subtract(marker_image_prev, marker_image_cur, marker_image);
    }
    else{
        subtract(marker_image_cur, marker_image_prev, marker_image);
    }

    cv::Mat nonZero;
    findNonZero(marker_image, nonZero);

    int nb_markers = nonZero.total();
    int width = marker_image.cols;
    int height = marker_image.rows;


    std::ofstream outputFile(name_out);

    outputFile << nb_markers << " " << width << " " << height << std::endl;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if(int(marker_image.at<uchar>(y,x)) == 1){
                    outputFile << x << " " << y << " 0 " << 0 << std::endl;
            }
            else if(int(marker_image.at<uchar>(y,x)) == 2){
                    outputFile << x << " " << y << " 1 " << 1 << std::endl;
            }
        }
    }
    outputFile.close();
}

void algorithms::vector_to_csv(std::vector<double> &vector, std::string file_path) {
    // Open an output filestream and create a CSV file
    std::ofstream outputFile(file_path);

    // Write each element of the vector to the CSV file, separated by a comma
    for (size_t i = 0; i < vector.size(); i++) {
        // Check if the current element is the last one
        if (i == vector.size() - 1)
            // If it is, write the element without a comma after it
            outputFile << vector[i];
        else
            // If it's not the last element, write the element followed by a comma
            outputFile << vector[i] << ",";
    }

    // Close the file
    outputFile.close();
}