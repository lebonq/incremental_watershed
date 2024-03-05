//
// Created by lebon on 17/05/2022.
//

#include "algorithms.h"

//#define PIXELS_COUNT

bool re_init1 = false;
bool re_init2 = false;

std::condition_variable finish_1;
std::mutex finish_1_m;

bool finish_1_b = false;

std::condition_variable finish_2;
std::mutex finish_2_m;

bool finish_2_b = false;

std::condition_variable start_1;
std::mutex start_1_m;

bool start_1_b = false;

std::condition_variable start_2;
std::mutex start_2_m;

bool start_2_b = false;

std::atomic<bool> isInCriticalSection(false);

bool split = false;

int nb_pixels_visited = 0;

void algorithms::kruskal(graph& G, Q& Q, int w, int* temp)
{
    for (int i = 0; i < G.getNbVertex(); i++)
    {
        Q.makeSet(i);
    }
    std::vector<int>& mst = G.getMst();
    int count = 0;

    //std::cout << G.getNbVertex() << std::endl;
    for (int j = 0; j < 256; j++)
    {
        for (int i = 0; i < G.count_[j]; i++)
        {
            int cx, cy = 0;
            int edge = G.sortedEdges_[j][i];

            //std::cout << "edge : " << edge << std::endl;

            if (((edge) & (1 << (0))) == 1)
            {
                //if odd or even not the same formula
                cx = edge / 2;
                cy = (edge / 2) + w;
            }
            else
            {
                cx = (edge + 1) / 2;
                cy = ((edge + 1) / 2) + 1;
            }

            //std::cout << "cx : " << cx << " cy : " << cy << std::endl;
            cx = Q.findCannonical(cx);
            cy = Q.findCannonical(cy);
            //std::cout << "Canonical : cx : " << cx << " cy : " << cy << std::endl;

            if (cx != cy)
            {
                Q.makeUnion(cx, cy);
                mst.push_back(edge); //add edge in mst

                temp[edge] = count; // Allow us to know where an edge is in the MST
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

int algorithms::breadthFirstSearchLabel(imageManager& im, int tag, int p)
{
    std::vector<int> queue;
    queue.push_back(p);
    int count = 1;
    int w = im.getWidth();
    int h = im.getHeight();
    int wh = w * h;


    int v, vRight, vLeft, vUp, vDown;
    while (!queue.empty())
    {
        v = queue.back();
        queue.pop_back();
        im.segments_[v] = tag;

        vRight = v + 1;
        vLeft = v - 1;
        vUp = v - w;
        vDown = v + w;

        if (vRight < wh && (v + 1) % w != 0)
        {
            //check if adjacent to v exist
            if (im.mstEdit_[im.map_graph_mst[2 * v]] == true &&
                im.segments_[vRight] != tag)
            {
                //If yes we check if the edge is revealnt and present in MST
                queue.push_back(vRight);
                im.segments_[vRight] = tag;
                count++;
            }
        }

        if (vLeft >= 0 && v % w != 0)
        {
            if (im.mstEdit_[im.map_graph_mst[(2 * v) - 2]] == true && im.segments_[vLeft] != tag)
            {
                queue.push_back(vLeft);
                im.segments_[vLeft] = tag;
                count++;
            }
        }

        if (vDown < wh)
        {
            if (im.mstEdit_[im.map_graph_mst[(2 * v) + 1]] == true && im.segments_[vDown] != tag)
            {
                queue.push_back(vDown);
                im.segments_[vDown] = tag;
                count++;
            }
        }

        if (vUp >= 0)
        {
            if (im.mstEdit_[im.map_graph_mst[((2 * v) - (2 * w)) + 1]] == true && im.segments_[vUp] != tag)
            {
                queue.push_back(vUp);
                im.segments_[vUp] = tag;
                count++;
            }
        }
    }
    return count;
}

void algorithms::breadthFirstSearchLabel_optimised(imageManager& im, int* p, int* old_tag, bool* re_init,
                                                   bool* re_init_c,
                                                   std::condition_variable& finish, std::mutex& finish_m,
                                                   bool* finish_b,
                                                   std::condition_variable& start, std::mutex& start_m, bool* start_b)
{
    std::vector<int> queue;
    std::vector<bool> explored(im.getGraph().getNbVertex(), false);
    std::vector<int> queue_label;

    int const w = im.getWidth();
    int const h = im.getHeight();
    int const wh = w * h;
    int count = 1;

    while (split == true)
    {
        {
            std::unique_lock<std::mutex> lk(finish_m);
            finish.wait(lk, [finish_b,p]
            {
                return *finish_b == false;
            });
            // std::cout << "Thread " << *p << " started" << std::endl;
        }

        if (split == false) return;

        queue.push_back(*p);
        explored[*p] = true;
        queue_label.push_back(*p);

        int v, vRight, vLeft, vUp, vDown;
        while (!queue.empty())
        {
            v = queue.back();
            queue.pop_back();

            vRight = v + 1;
            vLeft = v - 1;
            vUp = v - w;
            vDown = v + w;

            if (vRight < wh && (v + 1) % w != 0)
            {
                //check if adjacent to v exist
                if (im.mstEdit_[im.map_graph_mst[2 * v]] == true && explored[vRight] != true)
                {
                    //If yes we check if the edge is revealnt and present in MST
                    queue.push_back(vRight);
                    explored[vRight] = true;
                    queue_label.push_back(vRight);
                    count++;
                }
            }

            if (vLeft >= 0 && v % w != 0)
            {
                if (im.mstEdit_[im.map_graph_mst[(2 * v) - 2]] == true && explored[vLeft] != true)
                {
                    queue.push_back(vLeft);
                    explored[vLeft] = true;
                    queue_label.push_back(vLeft);
                    count++;
                }
            }

            if (vDown < wh)
            {
                if (im.mstEdit_[im.map_graph_mst[(2 * v) + 1]] == true && explored[vDown] != true)
                {
                    queue.push_back(vDown);
                    explored[vDown] = true;
                    queue_label.push_back(vDown);
                    count++;
                }
            }

            if (vUp >= 0)
            {
                if (im.mstEdit_[im.map_graph_mst[((2 * v) - (2 * w)) + 1]] == true && explored[vUp] != true)
                {
                    queue.push_back(vUp);
                    explored[vUp] = true;
                    queue_label.push_back(vUp);
                    count++;
                }
            }

            if (*re_init == true)
            {
                //   std::cout << "Thread " << *p << " re_init" << std::endl;
                break;
            }
            //std::cout << "Thread " << *p << " explore" << std::endl;
        }

        if (!isInCriticalSection.exchange(true))
        {
            int* sizeSeg = im.sizePart_;
            int newTag = im.tagCount_;
            im.tagCount_++;

            sizeSeg[newTag] = count; //breadthFirstSearchLabel(im, newTag, p);
            sizeSeg[*old_tag] -= count; //sizeSeg[newTag];

            //Update the CC mapping

            for (auto vertex : queue_label)
            {
                im.segments_[vertex] = newTag;
            }
            // std::cout << "CC size of " << count << " created by " << *p << std::endl;
            *re_init_c = true;
            // std::cout << "Thread " << *p << " critical section just said to reinit" << std::endl;
        }
        //reset datastructure
        queue.clear();
        queue_label.clear();
        explored.clear();
        count = 1;

        {
            std::lock_guard<std::mutex> lk_finish(finish_m);
            *finish_b = true;
        }
        finish.notify_all();
        //std::cout << "Thread " << *p << " finished" << std::endl;
    }
}

int algorithms::breadthFirstSearchLabel_v2(imageManager& im, int* p, int* buffer, int* n)
{
    std::vector<int> queue;
    std::vector explored(im.getGraph().getNbVertex(), false);
    queue.push_back(*p);
    explored[*p] = true;
    buffer[*n] = *p;
    *n += 1;
    int count = 1;
    int w = im.getWidth();
    int h = im.getHeight();
    int wh = w * h;


    int v, vRight, vLeft, vUp, vDown;
    while (!queue.empty())
    {
        v = queue.back();
        queue.pop_back();

        vRight = v + 1;
        vLeft = v - 1;
        vUp = v - w;
        vDown = v + w;

        if (vRight < wh && (v + 1) % w != 0)
        {
            //check if adjacent to v exist
            if (im.mstEdit_[im.map_graph_mst[2 * v]] == true &&
                explored[vRight] != true)
            {
                //If yes we check if the edge is revealnt and present in MST
                queue.push_back(vRight);
                explored[vRight] = true;
                buffer[*n] = vRight;
                *n += 1;
                count++;
            }
        }

        if (vLeft >= 0 && v % w != 0)
        {
            if (im.mstEdit_[im.map_graph_mst[(2 * v) - 2]] == true && explored[vLeft] != true)
            {
                queue.push_back(vLeft);
                explored[v] = true;
                buffer[*n] = vLeft;
                *n += 1;
                count++;
            }
        }

        if (vDown < wh)
        {
            if (im.mstEdit_[im.map_graph_mst[(2 * v) + 1]] == true && explored[vDown] != true)
            {
                queue.push_back(vDown);
                explored[vDown] = true;
                buffer[*n] = vDown;
                *n += 1;
                count++;
            }
        }

        if (vUp >= 0)
        {
            if (im.mstEdit_[im.map_graph_mst[((2 * v) - (2 * w)) + 1]] == true && explored[vUp] != true)
            {
                queue.push_back(vUp);
                explored[vUp] = true;
                buffer[*n] = vUp;
                *n += 1;
                count++;
            }
        }
    }
    return count;
}


void algorithms::splitSegment(imageManager& im, bool* historyVisited,
                              std::vector<int> queueEdges)
{
    int p1, p2, tag1, tag2, newTag; //Are being erased
    int w = im.getWidth();
    int* seg = im.segments_;
    int* sizeSeg = im.sizePart_;

    for (int edge : queueEdges)
    {
        if (((edge) & (1 << (0))) == 1)
        {
            //if odd or even not the same formula
            //Copy and paste of line #22 to avoid doing jump into memory
            p1 = edge / 2;
            p2 = (edge / 2) + w;
        }
        else
        {
            p1 = (edge + 1) / 2;
            p2 = ((edge + 1) / 2) + 1;
        }

        tag1 = seg[p1];
        tag2 = seg[p2];

        if (!historyVisited[tag2])
        {
            if (im.tag_.empty())
            {
                newTag = im.tagCount_;
                im.tagCount_++;
            }
            else
            {
                newTag = im.tag_.back();
                im.tag_.pop_back();
            }
            sizeSeg[newTag] = breadthFirstSearchLabel(im, newTag, p2);
            sizeSeg[tag2] -= sizeSeg[newTag];
            if (sizeSeg[tag2] <= 0)
            {
                im.tag_.push_back(tag2);
            }
            historyVisited[newTag] = true;
#ifdef PIXELS_COUNT
            nb_pixels_visited += sizeSeg[newTag];
#endif
        }

        if (!historyVisited[tag1])
        {
            if (im.tag_.empty())
            {
                newTag = im.tagCount_;
                im.tagCount_++;
            }
            else
            {
                newTag = im.tag_.back();
                im.tag_.pop_back();
            }
            sizeSeg[newTag] = breadthFirstSearchLabel(im, newTag, p1);
            sizeSeg[tag1] -= sizeSeg[newTag];
            if (sizeSeg[tag1] <= 0)
            {
                im.tag_.push_back(tag1);
            }
            historyVisited[newTag] = true;
#ifdef PIXELS_COUNT
            nb_pixels_visited += sizeSeg[newTag];
#endif
        }
    }
}

void algorithms::splitSegment_optimised(imageManager& im, bool* historyVisited,
                                        std::vector<int> queueEdges)
{
    split = true;
    int p1, p2, tag1, tag2;
    int w = im.getWidth();
    int* seg = im.segments_;
    int* sizeSeg = im.sizePart_;

    std::thread t1(breadthFirstSearchLabel_optimised, std::ref(im), &p1, &tag1, &re_init1, &re_init2,
                   std::ref(finish_1), std::ref(finish_1_m), &finish_1_b,
                   std::ref(start_1), std::ref(start_1_m), &start_1_b);
    std::thread t2(breadthFirstSearchLabel_optimised, std::ref(im), &p2, &tag2, &re_init2, &re_init1,
                   std::ref(finish_2), std::ref(finish_2_m), &finish_2_b,
                   std::ref(start_2), std::ref(start_2_m), &start_2_b);

    for (int edge : queueEdges)
    {
        isInCriticalSection.store(false);

        if (((edge) & (1 << (0))) == 1)
        {
            //if odd or even not the same formula
            //Copy and paste of line #22 to avoid doing jump into memory
            p1 = edge / 2;
            p2 = (edge / 2) + w;
        }
        else
        {
            p1 = (edge + 1) / 2;
            p2 = ((edge + 1) / 2) + 1;
        }

        tag1 = seg[p1];
        tag2 = seg[p2];

        int old_tag1 = tag1;
        int old_tag2 = tag2;

        //std::cout << "Thread 1 : " << p1 << " Thread 2 : " << p2 << std::endl;

        //std::cout << "Start 1" << std::endl;
        if (historyVisited[tag1] == false)
        {
            std::lock_guard<std::mutex> lk(finish_1_m);
            finish_1_b = false;
            finish_1.notify_all();
        }

        //std::cout << "Started 1" << std::endl;

        //std::cout << "Start 2" << std::endl;
        if (historyVisited[tag2] == false)
        {
            std::lock_guard<std::mutex> lk(finish_2_m);
            finish_2_b = false;
            finish_2.notify_all();
        }

        //std::cout << "Started 2" << std::endl;

        if (historyVisited[tag1] == false)
        {
            std::unique_lock<std::mutex> lk_finish_1(finish_1_m);
            finish_1.wait(lk_finish_1, [] { return finish_1_b == true; });
        }
        //std::cout << "Thread 1 finished" << std::endl;

        if (historyVisited[tag2] == false)
        {
            std::unique_lock<std::mutex> lk_finish_2(finish_2_m);
            finish_2.wait(lk_finish_2, [] { return finish_2_b == true; });
        }
        //std::cout << "Thead 2 finished" << std::endl;

        int new_tag1 = seg[p1];
        int new_tag2 = seg[p2];

        if (new_tag1 != old_tag1)
        {
            historyVisited[new_tag1] = true;
        }
        else if (new_tag2 != old_tag2)
        {
            historyVisited[new_tag2] = true;
        }

        finish_1_b = false;
        finish_2_b = false;

        re_init1 = false;
        re_init2 = false;
    }
    split = false;

    {
        std::lock_guard<std::mutex> lk(finish_1_m);
        finish_1_b = false;
        finish_1.notify_all();
    }
    {
        std::lock_guard<std::mutex> lk(finish_2_m);
        finish_2_b = false;
        finish_2.notify_all();
    }
    t1.join();
    t2.join();
}

std::tuple<int, int> algorithms::edge_to_vertices(const int edge, const int w)
{
    int p1, p2;
    if (((edge) & (1 << (0))) == 1)
    {
        p1 = edge / 2;
        p2 = (edge / 2) + w;
    }
    else
    {
        p1 = (edge + 1) / 2;
        p2 = ((edge + 1) / 2) + 1;
    }

    return std::make_tuple(p1, p2);
}

void algorithms::splitSegment_optimised_v2(imageManager& im, bool* historyVisited,
                                           std::vector<int> queueEdges)
{

    int nb_t = 6;
    //Creating buffers for thread
    std::vector<std::vector<int>> buffer(nb_t);
    //Vector has to be initialized with the size of the buffer
    for (int i = 0; i < nb_t; i++)
    {
        buffer[i].resize(im.getGraph().getNbVertex());
    }

    int* idx_buffer = new int[nb_t]; //nb of value in a buffer
    memset(idx_buffer, 0, nb_t * sizeof(int));

    std::vector<int> vertices;

    int tag_max = im.tagCount_;
    std::vector<bool> is_keeped(tag_max, false);

    //Building the vertices array
    for (auto edge : queueEdges)
    {
        int p1, p2;
        std::tie(p1, p2) = edge_to_vertices(edge, im.getWidth());
        vertices.push_back(p1);
        vertices.push_back(p2);
    }


    int idx_vertices = 0;
    int idx_v_thread = 0;
    std::vector<int> to_explore(nb_t, -1);

    while (idx_vertices < vertices.size())//we stop once all the vertices have been explored
    {
        //Selection of the nb_t vetices we are goign to explore
        while (idx_vertices < vertices.size() && idx_v_thread < nb_t)
        {
            int tag = im.segments_[vertices[idx_vertices]];
            //Check if the current lable as already bene chose by a CC
            if (tag < tag_max)
            {
                if (is_keeped[tag] == false)
                {
                    is_keeped[tag] = true;
                }
                else
                {
                    //adge vertex in queue
                    to_explore[idx_v_thread] = vertices[idx_vertices];
                    idx_v_thread++;
                }
            }
            else
            {
                //add vertex in queue
                to_explore[idx_v_thread] = vertices[idx_vertices];
                idx_v_thread++;
            }
            idx_vertices++;
        }

        auto tets = buffer[0].data();

        for(int run = 0; run < nb_t; run++)
        {
            breadthFirstSearchLabel_v2(im, &to_explore[run], buffer[run].data(), &idx_buffer[run]);
        }

        //Labeling the different CC
        for (int t = 0; t < nb_t; t++)
        {
            int oldTag = im.segments_[to_explore[t]];
            if (oldTag < tag_max) //If the CC has already been label we don't need to do anything
            {
                int newTag = im.tagCount_;
                im.tagCount_++;
                int count = 0;
                for (int i = 0; i < idx_buffer[t]; i++)
                {
                    int vertex = buffer[t][i];
                    im.segments_[vertex] = newTag;
                    count++;
                }
                im.sizePart_[newTag] = count;
                im.sizePart_[oldTag] -= count;
            }
        }

        memset(idx_buffer, 0, nb_t * sizeof(int)); //Empty the buffer arrays
        idx_v_thread = 0;
    }

    //Clean memory
    delete[] idx_buffer;

}

/**
 * Merge 2 segments
 * @param p1 Point on the first segment
 * @param p2 Point on the second segment
 * @param im
 */
void algorithms::mergeSegment(int edge, imageManager& im)
{
    int* seg = im.segments_;
    int* sizeSeg = im.sizePart_;
    int p1, p2;

    if (((edge) & (1 << (0))) == 1)
    {
        //if odd or even not the same formula
        //Copy and paste of line #22 to avoid doing jump into memory
        p1 = edge / 2;
        p2 = (edge / 2) + im.getWidth();
    }
    else
    {
        p1 = (edge + 1) / 2;
        p2 = ((edge + 1) / 2) + 1;
    }

    int tag1 = seg[p1];
    int tag2 = seg[p2];

    if (sizeSeg[tag1] < sizeSeg[tag2])
    {
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

void algorithms::removeMarker(imageManager& im, int* markers, int nbMarkers)
{
#ifdef PIXELS_COUNT
    nb_pixels_visited = 0;
#endif
    int marker, up = 0;
    QBT& qbt = im.getHierarchy().getQBT();
    int* parent = qbt.getParents();
    auto mstL = im.getGraph().getMst();

    for (int i = 0; i < nbMarkers; i++)
    {
        marker = markers[i];
        up = parent[marker];
        while (up != -1)
        {
            im.marks_[up]--;
            if (im.marks_[up] == 1)
            {
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

void algorithms::addMarker(imageManager& im, int* markers, int nbMarkers)
{
#ifdef PIXELS_COUNT
    nb_pixels_visited = 0;
#endif
    int marker, up = 0;
    QBT& qbt = im.getHierarchy().getQBT();
    int* parent = qbt.getParents();
    bool* historyVisited = new bool[im.getGraph().getNbVertex()]();

    std::vector<int> queueEdges;
    auto mstL = im.getGraph().getMst();

    for (int i = 0; i < nbMarkers; i++)
    {
        marker = markers[i];
        up = parent[marker];
        while (up != -1)
        {
            im.marks_[up]++;
            if (im.marks_[up] == 2)
            {
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
void algorithms::showSegmentation(imageManager& im, std::string nameOfImage)
{
    cv::Mat img(im.getHeight(), im.getWidth(),
                CV_8UC3, cv::Vec3b(255, 0, 0));
    // Verify if image is created or not
    if (img.empty())
    {
        std::cout << "Could not load image" << std::endl;
        std::cin.get();
    }

    img.at<cv::Vec3b>(0, 0) = cv::Vec3b(0, 0, 0);

    int cpt = 0, seed;
    for (int y = 0; y < im.getHeight(); y++)
    {
        for (int x = 0; x < im.getWidth(); x++)
        {
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

bool algorithms::get_vector_from_txt(const std::string& file_path, std::vector<int>& values)
{
    std::string file_add = file_path + "_add.txt";
    std::string file_remove = file_path + "_remove.txt";
    //std::cout << file_add << std::endl;

    std::string line; /* string to hold each line read from file  */
    std::ifstream f(file_add); /* file stream to read  */
    bool remove = false;

    if (!f.is_open())
    {
        f = std::ifstream(file_remove);
        remove = true;
    }


    while (getline(f, line))
    {
        /* read each line into line */
        /* if no digits in line - get next */
        if (line.find_first_of("0123456789") == std::string::npos)
            continue;
        int itmp; /* temporary int */
        std::stringstream ss(line); /* stringstream from line */
        while (ss >> itmp)
        {
            /* read int from stringstream */
            std::string tmpstr; /* temporary string to ',' */
            values.push_back(itmp); /* add int to tmp */
            if (!getline(ss, tmpstr, ',')) /* read to ',' w/tmpstr */
                break; /* done if no more ',' */
        }
    }

    return remove;

    /*std::cout << values.size() << std::endl;
    for (auto col : values)
        std::cout << "  " << col;
    std::cout << '\n';*/
}

void algorithms::get_tab_from_image(const std::string& file_path, std::vector<int>& values)
{
    cv::Mat marker_image;


    try
    {
        marker_image = cv::imread(file_path + "_add.png", CV_8UC1);
        if (marker_image.empty())
        {
            marker_image = cv::imread(file_path + "_remove.png",
                                      CV_8UC1);
        }
    }
    catch (cv::Exception e)
    {
        ;
        try
        {
            marker_image = cv::imread(file_path + "_remove.png",
                                      CV_8UC1);
        }
        catch (cv::Exception e)
        {
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

    for (int i = 0; i < nbmarkers; i++)
    {
        values.push_back(nonZeroCoordinates_flat.at<cv::Point>(i).x);
    }
}

void algorithms::get_DIFT_seed_from_image(const std::string& file_path, const int id)
{
    std::string fp_cur = file_path + std::to_string(id);
    std::string fp_prev = file_path + std::to_string(id - 1);
    cv::Mat marker_image_cur;
    cv::Mat marker_image_prev;
    std::string name_out;

    bool remove = false;

    try
    {
        marker_image_cur = cv::imread(fp_cur + "_add.png", CV_8UC1);
        name_out = fp_cur + "_add_DIFT.txt";
        if (marker_image_cur.empty())
        {
            marker_image_cur = cv::imread(fp_cur + "_remove.png",
                                          CV_8UC1);
            name_out = fp_cur + "_remove_DIFT.txt";
            remove = true;
        }
    }
    catch (cv::Exception e)
    {
        ;
        std::cout << e.msg << std::endl;
        std::exit(e.code);
    }

    try
    {
        marker_image_prev = cv::imread(fp_prev + "_add.png", CV_8UC1);
        if (marker_image_prev.empty())
        {
            marker_image_prev = cv::imread(fp_prev + "_remove.png",
                                           CV_8UC1);
        }
    }
    catch (cv::Exception e)
    {
        ;
        std::cout << e.msg << std::endl;
        std::exit(e.code);
    }

    cv::Mat marker_image;
    if (id == 0) marker_image = marker_image_cur;
    else if (remove == true)
    {
        subtract(marker_image_prev, marker_image_cur, marker_image);
    }
    else
    {
        subtract(marker_image_cur, marker_image_prev, marker_image);
    }

    cv::Mat nonZero;
    findNonZero(marker_image, nonZero);

    int nb_markers = nonZero.total();
    int width = marker_image.cols;
    int height = marker_image.rows;


    std::ofstream outputFile(name_out);

    outputFile << nb_markers << " " << width << " " << height << std::endl;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (int(marker_image.at<uchar>(y, x)) == 1)
            {
                outputFile << x << " " << y << " 0 " << 0 << std::endl;
            }
            else if (int(marker_image.at<uchar>(y, x)) == 2)
            {
                outputFile << x << " " << y << " 1 " << 1 << std::endl;
            }
        }
    }
    outputFile.close();
}

void algorithms::vector_to_csv(std::vector<double>& vector, std::string file_path)
{
    // Open an output filestream and create a CSV file
    std::ofstream outputFile(file_path);

    // Write each element of the vector to the CSV file, separated by a comma
    for (size_t i = 0; i < vector.size(); i++)
    {
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
