//
// Created by lebon on 17/05/2022.
//

#include "algorithms.h"

//#define PIXELS_COUNT

int nb_thread = 4;

bool split = false;

int nb_pixels_visited = 0;

int* counter = new int[4];

long* t_time_explo = new long[50];

pthread_t* thread; // global variable used to store the threads id
struct DistanceMap* D; // global variable used to easily pass arguments to threads
int* vertices = new int[2];
int* size_cc = new int[2];

imageManager* im_t_ptr;

/*-----------------------------------------------Partition Function----------------------------------------*/
void* parPartition(void* slice)
{
    int i, ji, p, n, i2;
    int start;
    int end;

    p = D->p; // number of threads
    n = D->indice; // number of data in E
    i = ((long)slice) + 1;

    if (i <= (n % p))
    {
        start = (i - 1) * ((n / p) + 1);
        end = start + (n / p);
        D->TailleEi[i - 1] = (n / p) + 1;
    }
    else
    {
        start = (n % p) * ((n / p) + 1) + (i - 1 - (n % p)) * (n / p);
        end = start + (n / p) - 1;
        D->TailleEi[i - 1] = n / p; // to know the size of each Ei
    }

    for (ji = start; ji <= end; ji++)
        D->Ei[i - 1][ji - start] = D->E[ji];

    pthread_exit(NULL);
}

void seqPartition(int slice)
{
    int i, ji, p, n, i2;
    int start;
    int end;

    p = D->p; // number of threads
    n = D->indice; // number of data in E
    i = (slice) + 1;

    if (i <= (n % p))
    {
        start = (i - 1) * ((n / p) + 1);
        end = start + (n / p);
        auto new_taille_ei = (n / p) + 1;
        D->TailleEi[i - 1] = new_taille_ei;
    }
    else
    {
        start = (n % p) * ((n / p) + 1) + (i - 1 - (n % p)) * (n / p);
        end = start + (n / p) - 1;
        auto new_taille_ei = n / p;
        D->TailleEi[i - 1] = new_taille_ei; // to know the size of each Ei
    }

    for (ji = start; ji <= end; ji++)
        D->Ei[i - 1][ji - start] = D->E[ji];
}

/*-----------------------------------------------Union Function ----------------------------------------------------*/

void* copyForUnion(void* slice)
{
    auto start = std::chrono::high_resolution_clock::now();
    int i, j;
    i = (long)slice;

    for (j = 0; j < (D->TailleSi[i]); j++)
    {
        auto value = D->Si[i][j];
        D->E[D->start[i] + j] = value;
    }

    auto end = std::chrono::high_resolution_clock::now();
    t_time_explo[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    pthread_exit(NULL);
}

void copyForUnionSeq(int slice)
{
    //auto start = std::chrono::high_resolution_clock::now();
    int i;
    i = slice;

    for (int j = 0; j < D->TailleSi[i]; j++)
    {
        auto value = D->Si[i][j];
        D->E[D->start[i] + j] = value;
    }

    //auto end = std::chrono::high_resolution_clock::now();
    //t_time_explo[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

}

/************************************************************/
void setUnion()
{
    int i, j, cmp, p;
    p = D->p; // number of threads
    cmp = 0;

    for (i = 0; i < p; i++)
    {
        // to calcul start of each processor
        D->start[i] = cmp;
        cmp = cmp + D->TailleSi[i];
    }


    for (i = 0; i < p; i++)
    {
        /*if (pthread_create(&thread[i], NULL, copyForUnion, (void*)(long)i) != 0)
        {
            perror("Can't create thread");
            free(thread);
            exit(-1);
        }*/
        //copyForUnionSeq(i);

        for (j = 0; j < D->TailleSi[i]; j++)
        {
            auto value = D->Si[i][j];
            D->E[D->start[i] + j] = value;
        }
    }
    /*for (i = 0; i < p; i++)
        pthread_join(thread[i], NULL);*/

    D->indice = cmp; // new after union |n|
}

/*-------------------------------------------------creation level-sets---------------------------------------------------*/
void* parLevelSetTraversal(void* slice)
{
    auto start = std::chrono::high_resolution_clock::now();
    int i, j, x, y, vertex;

    j = 0;
    i = (long)slice;

    int w = im_t_ptr->getWidth();
    int h = im_t_ptr->getHeight();
    int wh = w * h;


    /* fprintf(stderr,"Taille de E[%d] = %d\n", i, D->TailleEi[i]); */
    /* fprintf(stderr,"Elements de E[%d]\n", i); */
    /* for(x = 0; x < D->TailleEi[i]; x++) fprintf(stderr, "%d-eme element %d, ", x, D->Ei[i][x]); */
    /* fprintf(stderr,".\n"); */

    for (x = 0; x < D->TailleEi[i]; x++)
    {
        // for each x in Ei
        int v = D->Ei[i][x];
        int tag = im_t_ptr->segments_[v];
        /* fprintf(stderr,"Exploration des successeurs de %d\n", vertex); */

        int vRight = v + 1;
        int vLeft = v - 1;
        int vUp = v - w;
        int vDown = v + w;

        if (vRight < wh && (v + 1) % w != 0)
        {
            //check if adjacent to v exist

            auto mst_edge = im_t_ptr->map_graph_mst[2 * v];
            if (mst_edge != -1)
            {
                if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vRight] != tag)
                {
                    im_t_ptr->segments_[vRight] = tag;
                    D->Si[i][j] = vRight;
                    j++;
                }
            }
        }

        if (vLeft >= 0 && v % w != 0)
        {
            auto mst_edge = im_t_ptr->map_graph_mst[(2 * v) - 2];
            if (mst_edge != -1)
            {
                if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vLeft] != tag)
                {
                    im_t_ptr->segments_[vLeft] = tag;
                    D->Si[i][j] = vLeft;
                    j++;
                }
            }
        }

        if (vDown < wh)
        {
            auto mst_edge = im_t_ptr->map_graph_mst[(2 * v) + 1];
            if (mst_edge != -1)
            {
                if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vDown] != tag)
                {
                    im_t_ptr->segments_[vDown] = tag;
                    D->Si[i][j] = vDown;
                    j++;
                }
            }
        }

        if (vUp >= 0)
        {
            auto mst_edge = im_t_ptr->map_graph_mst[((2 * v) - (2 * w)) + 1];
            if (mst_edge != -1)
            {
                if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vUp] != tag)
                {
                    im_t_ptr->segments_[vUp] = tag;
                    D->Si[i][j] = vUp;
                    j++;
                }
            }
        }
    }
    D->TailleSi[i] = j; // number of element in each Si
    auto end = std::chrono::high_resolution_clock::now();
    t_time_explo[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    pthread_exit(NULL);
}

void parLevelSetTraversal_v2(int slice, std::binary_semaphore& finish, std::binary_semaphore& start)

{
    while(split == true)
    {

        start.acquire();

        //Check if split is == false
        if (split == false) return;


        auto start = std::chrono::high_resolution_clock::now();
        int i, j, x, y, vertex;

        j = 0;
        i = slice;

        int w = im_t_ptr->getWidth();
        int h = im_t_ptr->getHeight();
        int wh = w * h;


        /* fprintf(stderr,"Taille de E[%d] = %d\n", i, D->TailleEi[i]); */
        /* fprintf(stderr,"Elements de E[%d]\n", i); */
        /* for(x = 0; x < D->TailleEi[i]; x++) fprintf(stderr, "%d-eme element %d, ", x, D->Ei[i][x]); */
        /* fprintf(stderr,".\n"); */

        for (x = 0; x < D->TailleEi[i]; x++)
        {
            // for each x in Ei
            int v = D->Ei[i][x];
            int tag = im_t_ptr->segments_[v];
            /* fprintf(stderr,"Exploration des successeurs de %d\n", vertex); */

            int vRight = v + 1;
            int vLeft = v - 1;
            int vUp = v - w;
            int vDown = v + w;

            if (vRight < wh && (v + 1) % w != 0)
            {
                //check if adjacent to v exist

                auto mst_edge = im_t_ptr->map_graph_mst[2 * v];
                if (mst_edge != -1)
                {
                    if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vRight] != tag)
                    {
                        im_t_ptr->segments_[vRight] = tag;
                        D->Si[i][j] = vRight;
                        j++;
                    }
                }
            }

            if (vLeft >= 0 && v % w != 0)
            {
                auto mst_edge = im_t_ptr->map_graph_mst[(2 * v) - 2];
                if (mst_edge != -1)
                {
                    if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vLeft] != tag)
                    {
                        im_t_ptr->segments_[vLeft] = tag;
                        D->Si[i][j] = vLeft;
                        j++;
                    }
                }
            }

            if (vDown < wh)
            {
                auto mst_edge = im_t_ptr->map_graph_mst[(2 * v) + 1];
                if (mst_edge != -1)
                {
                    if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vDown] != tag)
                    {
                        im_t_ptr->segments_[vDown] = tag;
                        D->Si[i][j] = vDown;
                        j++;
                    }
                }
            }

            if (vUp >= 0)
            {
                auto mst_edge = im_t_ptr->map_graph_mst[((2 * v) - (2 * w)) + 1];
                if (mst_edge != -1)
                {
                    if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vUp] != tag)
                    {
                        im_t_ptr->segments_[vUp] = tag;
                        D->Si[i][j] = vUp;
                        j++;
                    }
                }
            }
        }
        D->TailleSi[i] = j; // number of element in each Si
        auto end = std::chrono::high_resolution_clock::now();
        t_time_explo[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        finish.release();
    }
}

/*--------------------------------------Initialization---------------------------------------------------------------*/
void* parInit(void* param)
/* Parallel part of the initialization of the parallel distance map algorithm, ie creation of the 0-level set of the diastance map */
{
    int proc = (long)param;


    if (proc <= 1) //For the first 2 thread
    {
        if (vertices[proc] != -1)
        {
            D->Si[proc][0] = vertices[proc];
            //pthread_mutex_lock(&D->Traversed[vertices[proc]]);
            D->TailleSi[proc] = 1; // number of element in each Si
        }
    }
    else
    {
        D->TailleSi[proc] = 0;
    }

    pthread_exit(NULL);
}

void dMapInit(int p)
/* Initialize the structure used during parallel distance map
   computation */
{
    int i;

    for (i = 0; i < p; i++)
    {
        if (pthread_create(&thread[i], NULL, parInit, (void*)(long)i) != 0)
        {
            perror("Can't create thread");
            free(thread);
            exit(-1);
        }
    }
    for (i = 0; i < p; i++)
        pthread_join(thread[i], NULL); // threads join

    //print TailleSi

    setUnion();
}

void allocate_distancemap(int p)
{
    auto& g = im_t_ptr->getGraph();

    int i;

    D->E = (int*)malloc(g.getNbVertex() * sizeof(int));
    memset(D->E, 0, g.getNbVertex() * sizeof(int));

    D->Si = (int**)malloc(p * sizeof(int*));
    D->Ei = (int**)malloc(p * sizeof(int*));

    for (i = 0; i < p; i++)
    {
        D->Ei[i] = (int*)malloc(g.getNbVertex() * sizeof(int));
        memset(D->Ei[i], 0, g.getNbVertex() * sizeof(int));
        D->Si[i] = (int*)malloc(g.getNbVertex() * sizeof(int));
    }

    D->TailleSi = (int*)malloc(p * sizeof(int));

    D->TailleEi = (int*)malloc(p * sizeof(int));

    D->start = (int*)malloc(p * sizeof(int));
    memset(D->start, 0, p * sizeof(int));

    thread = (pthread_t*)malloc(p * sizeof(pthread_t)); //  threads array
}

/*----------------------------------------------------------------------------------------------------------------------*/

void clean_distancemap()
{
    // Free the memory allocated for each array in D->Ei and D->Si
    for (int i = 0; i < D->p; i++)
    {
        free(D->Ei[i]);
        free(D->Si[i]);
    }

    // Free the memory allocated for the arrays in the DistanceMap structure
    //free(D->Traversed);
    free(D->E);
    free(D->Si);
    free(D->Ei);
    free(D->TailleSi);
    free(D->TailleEi);
    free(D->start);
    free(thread);

    // Set all the pointers to NULL to avoid dangling pointers
    //D->Traversed = NULL;
    D->E = NULL;
    D->Si = NULL;
    D->Ei = NULL;
    D->TailleSi = NULL;
    D->TailleEi = NULL;
    D->start = NULL;
}

/***************************************** Parallel Distance Map **********************************************/
void Distance_Maps(struct DistanceMap* D, int p)
{
    int i, j, nb_level, r, som, d;

    // Run the code you want to benchmark
    dMapInit(p);

    /* fprintf(stderr, "Allocation des structures necessaires a la carte de distance ok\n"); */

    while (D->indice != 0)
    {
        /* fprintf(stderr, "D�but du traitement de niveau %d\n", nb_level); */
        // Partition Function
        for (i = 0; i < p; i++)
        {
            if (pthread_create(&thread[i], NULL, parPartition, (void*)(long)i) != 0)
            {
                perror("Can't create thread");
                free(thread);
                exit(-1);
            }
        }
        for (i = 0; i < p; i++)
            pthread_join(thread[i], NULL); // threads join

        /* fprintf(stderr, "Niveau %d: partition ok\n", nb_level); */

        // Creation of level-set
        for (i = 0; i < p; i++)
        {
            // for each processor i
            if (pthread_create(&thread[i], NULL, parLevelSetTraversal, (void*)(long)i) != 0)
            {
                perror("Can't create thread");
                free(thread);
                exit(-1);
            }
        }
        for (i = 0; i < p; i++)
            pthread_join(thread[i], NULL); // threads join

        // Union of the sets traversed by the threads to form th next level set stored in D->E
        setUnion();

        /*for (i = 0; i < p; i++)
        {
            fprintf(stderr,"Taille de Si[%d] = %d\n", i, D->TailleSi[i]);
        }*/
    }

    //fprintf(stderr, "Carte de Distance calculee\n");
}

void parLabelisation(struct DistanceMap* D, int p)
{
    int i, j, nb_level, r, som, d;

    /* fprintf(stderr, "Niveau %d: partition ok\n", nb_level); */

    // Creation of level-set
    for (i = 0; i < p; i++)
    {
        // for each processor i
        if (pthread_create(&thread[i], NULL, parLevelSetTraversal, (void*)(long)i) != 0)
        {
            perror("Can't create thread");
            free(thread);
            exit(-1);
        }
    }
    for (i = 0; i < p; i++)
        pthread_join(thread[i], NULL); // threads join


}

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

        if (vRight < wh && (v + 1) % w != 0)
        {
            //check if adjacent to v exist
            auto mst_edge = im.map_graph_mst[2 * v];
            if (mst_edge != -1)
            {
                if (im.mstEdit_[mst_edge] && im.segments_[vRight] != tag)
                {
                    //If yes we check if the edge is revealnt and present in MST
                    queue.push_back(vRight);
                    im.segments_[vRight] = tag;
                    count++;
                }
            }
        }

        if (vLeft >= 0 && v % w != 0)
        {
            auto mst_edge = im.map_graph_mst[(2 * v) - 2];
            if (mst_edge != -1)
            {
                if (im.mstEdit_[mst_edge] && im.segments_[vLeft] != tag)
                {
                    queue.push_back(vLeft);
                    im.segments_[vLeft] = tag;
                    count++;
                }
            }
        }

        if (vDown < wh)
        {
            auto mst_edge = im.map_graph_mst[(2 * v) + 1];
            if (mst_edge != -1)
            {
                if (im.mstEdit_[mst_edge] && im.segments_[vDown] != tag)
                {
                    queue.push_back(vDown);
                    im.segments_[vDown] = tag;
                    count++;
                }
            }
        }

        if (vUp >= 0)
        {
            auto mst_edge = im.map_graph_mst[((2 * v) - (2 * w)) + 1];
            if (mst_edge != -1)
            {
                if (im.mstEdit_[mst_edge] && im.segments_[vUp] != tag)
                {
                    queue.push_back(vUp);
                    im.segments_[vUp] = tag;
                    count++;
                }
            }
        }
    }
    return count;
}

void algorithms::breadthFirstSearchLabel_v2(imageManager& im, int* p, int* buffer, int* n,
                                            std::binary_semaphore& finish, std::binary_semaphore& start, int id)
{
    std::vector<int> queue;
    std::vector explored(im.getGraph().getNbVertex(), false);
    int w = im.getWidth();
    int h = im.getHeight();
    int wh = w * h;

    int v, vRight, vLeft, vUp, vDown;

    while (split == true)
    {
        start.acquire();

        //Check if split is == false
        if (split == false) return;

        queue.push_back(*p);
        counter[id] += 1;
        explored[*p] = true;
        buffer[*n] = *p;
        *n += 1;

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
                    counter[id] += 1;
                    explored[vRight] = true;
                    buffer[*n] = vRight;
                    *n += 1;
                }
            }

            if (vLeft >= 0 && v % w != 0)
            {
                if (im.mstEdit_[im.map_graph_mst[(2 * v) - 2]] == true && explored[vLeft] != true)
                {
                    queue.push_back(vLeft);
                    counter[id] += 1;
                    explored[v] = true;
                    buffer[*n] = vLeft;
                    *n += 1;
                }
            }

            if (vDown < wh)
            {
                if (im.mstEdit_[im.map_graph_mst[(2 * v) + 1]] == true && explored[vDown] != true)
                {
                    queue.push_back(vDown);
                    counter[id] += 1;
                    explored[vDown] = true;
                    buffer[*n] = vDown;
                    *n += 1;
                }
            }

            if (vUp >= 0)
            {
                if (im.mstEdit_[im.map_graph_mst[((2 * v) - (2 * w)) + 1]] == true && explored[vUp] != true)
                {
                    queue.push_back(vUp);
                    counter[id] += 1;
                    explored[vUp] = true;
                    buffer[*n] = vUp;
                    *n += 1;
                }
            }
        }

        finish.release();

        //Reset data structure
        //queue.clear();
        //std::fill(explored.begin(), explored.end(), false);
    }
    return;
}

int algorithms::breadthFirstSearchLabel_v2_seq(imageManager& im, int* p, int* buffer, int* n)
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


void algorithms::splitSegment(imageManager &im, bool *historyVisited,
                              std::vector<int>& queueEdges) {

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
    memset(counter, 0, 4 * sizeof(int));
    split = true;
    int nb_t = 4;

    //Creating threads
    std::vector<std::thread> threads(nb_t);

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

    std::random_device rd;
    std::mt19937 g(rd());

    // Shuffle the vector
    std::shuffle(vertices.begin(), vertices.end(), g);


    int idx_vertices = 0;
    int idx_v_thread = 0;
    std::vector<int> to_explore(nb_t, -1);

    //Creating buffers for thread
    std::vector<std::vector<int>> buffer(nb_t);
    //Vector has to be initialized with the size of the buffer
    for (int i = 0; i < nb_t; i++)
    {
        buffer[i].resize(im.getGraph().getNbVertex());
    }

    int* idx_buffer = new int[nb_t]; //nb of value in a buffer
    memset(idx_buffer, 0, nb_t * sizeof(int));

    //auto start = std::chrono::high_resolution_clock::now();

    //std::vector<std::binary_semaphore> finish_s(nb_thread,std::binary_semaphore(0));
    //std::vector<std::binary_semaphore> start_s(nb_thread,std::binary_semaphore(0));

    std::binary_semaphore finish_1{0}, start_1{0};
    std::binary_semaphore finish_2{0}, start_2{0};
    std::binary_semaphore finish_3{0}, start_3{0};
    std::binary_semaphore finish_4{0}, start_4{0};
    // std::binary_semaphore finish_5{0}, start_5{0};
    // std::binary_semaphore finish_6{0}, start_6{0};
    // std::binary_semaphore finish_7{0}, start_7{0};
    // std::binary_semaphore finish_8{0}, start_8{0};
    // std::binary_semaphore finish_9{0}, start_9{0};
    // std::binary_semaphore finish_10{0}, start_10{0};
    // std::binary_semaphore finish_11{0}, start_11{0};
    // std::binary_semaphore finish_12{0}, start_12{0};
    // std::binary_semaphore finish_13{0}, start_13{0};


    //creation of threads

    threads[0] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data() + 0, buffer[0].data(),
                             &idx_buffer[0], std::ref(finish_1), std::ref(start_1), 0);
    threads[1] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data() + 1, buffer[1].data(),
                             &idx_buffer[1], std::ref(finish_2), std::ref(start_2), 1);
    threads[2] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data() + 2, buffer[2].data(),
                             &idx_buffer[2], std::ref(finish_3), std::ref(start_3), 2);
    threads[3] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data() + 3, buffer[3].data(),
                             &idx_buffer[3], std::ref(finish_4), std::ref(start_4), 3);
    // threads[4] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data()+4, buffer[4].data(), &idx_buffer[4], std::ref(finish_5), std::ref(start_5));
    // threads[5] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data()+5, buffer[5].data(), &idx_buffer[5], std::ref(finish_6), std::ref(start_6));
    // threads[6] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data()+6, buffer[6].data(), &idx_buffer[6], std::ref(finish_7), std::ref(start_7));
    // threads[7] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data()+7, buffer[7].data(), &idx_buffer[7], std::ref(finish_8), std::ref(start_8));
    // threads[8] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data()+8, buffer[8].data(), &idx_buffer[8], std::ref(finish_9), std::ref(start_9));
    // threads[9] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data()+9, buffer[9].data(), &idx_buffer[9], std::ref(finish_10), std::ref(start_10));
    // threads[10] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data()+10, buffer[10].data(), &idx_buffer[10], std::ref(finish_11), std::ref(start_11));
    // threads[11] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data()+11, buffer[11].data(), &idx_buffer[11], std::ref(finish_12), std::ref(start_12));
    // threads[12] = std::thread(breadthFirstSearchLabel_v2, std::ref(im), to_explore.data()+12, buffer[12].data(), &idx_buffer[12], std::ref(finish_13), std::ref(start_13));

    // Get the end time
    // auto end = std::chrono::high_resolution_clock::now();

    // Calculate the difference
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    //std::cout << "Time taken by thread creation: "
    //        << duration.count() << " microseconds" << std::endl;

    int* nb_explo = new int[4];
    memset(nb_explo, 0, 4 * sizeof(int));

    while (idx_vertices < vertices.size()) //we stop once all the vertices have been explored
    {
        //Selection of the nb_t vetices we are goign to explore
        //std::cout << idx_buffer[0] << " " << idx_buffer[1] << std::endl;

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
                //to_explore[idx_v_thread] = vertices[idx_vertices];
                //idx_v_thread++;
            }
            idx_vertices++;
        }

        //Exploring the CC by unlcking all thread
        /*for(int run = 0; run < idx_v_thread; run++)
        {
            breadthFirstSearchLabel_v2_seq(im, &(to_explore[run]), buffer[run].data(), &idx_buffer[run]);
            //threads[run] = std::thread(breadthFirstSearchLabel_v2_seq, std::ref(im), &to_explore[run], buffer[run].data(), &idx_buffer[run]);
        }*/
        start_1.release();
        nb_explo[0]++;
        if (idx_v_thread >= 2)
        {
            start_2.release();
            nb_explo[1]++;
        }
        if (idx_v_thread >= 3)
        {
            start_3.release();
            nb_explo[2]++;
        }
        if (idx_v_thread == 4)
        {
            start_4.release();
            nb_explo[3]++;
        }
        // start_5.release();
        // start_6.release();
        // start_7.release();
        // start_8.release();
        // start_9.release();
        // start_10.release();
        // start_11.release();
        // start_12.release();
        // start_13.release();


        finish_1.acquire();
        if (idx_v_thread >= 2) finish_2.acquire();
        if (idx_v_thread >= 3)finish_3.acquire();
        if (idx_v_thread == 4)finish_4.acquire();
        // finish_5.acquire();
        // finish_6.acquire();
        // finish_7.acquire();
        // finish_8.acquire();
        // finish_9.acquire();
        // finish_10.acquire();
        // finish_11.acquire();
        // finish_12.acquire();
        // finish_13.acquire();


        //waiting for thread to finish
        /*for(int run = 0; run < idx_v_thread; run++)
        {

            //threads[run].join();
        }*/

        //Labeling the different CC
        for (int t = 0; t < nb_t; t++)
        {
            int oldTag = im.segments_[to_explore[t]];
            if (oldTag < tag_max) //If the CC has already been label we don't need to do anything
            {
                int newTag = im.tagCount_;
                im.tagCount_++;
                for (int i = 0; i < idx_buffer[t]; i++)
                {
                    int vertex = buffer[t][i];
                    im.segments_[vertex] = newTag;
                }
                im.sizePart_[newTag] = idx_buffer[t];
                im.sizePart_[oldTag] -= idx_buffer[t];
            }
        }

        memset(idx_buffer, 0, nb_t * sizeof(int)); //Empty the buffer arrays
        idx_v_thread = 0;
    }

    split = false;

    start_1.release();
    start_2.release();
    start_3.release();
    start_4.release();


    for (int i = 0; i < nb_t; i++)
    {
        threads[i].join();
    }

    //print the counter of each thread
    for (int i = 0; i < nb_t; i++)
    {
        std::cout << "Thread " << i << " : " << counter[i] << " pixels" << std::endl;
    }

    for (int i = 0; i < nb_t; i++)
    {
        std::cout << "Thread " << i << " : " << nb_explo[i] << " exploration" << std::endl;
    }

    //Clean memory
    delete[] idx_buffer;
}

void algorithms::splitSegment_v3(imageManager& im, bool* historyVisited, std::vector<int> queueEdges)
{
    split = true;

    int num_thrd; // number of threads
    int thres = 450; // threshold for parallel exploration
    im_t_ptr = &im;
    int par_explo = 0;
    int seq_explo = 0;
    D = (DistanceMap*)malloc(sizeof(struct DistanceMap)); // Distance map structure
    num_thrd = 10;
    std::vector<std::thread> threads(num_thrd);
    D->p = num_thrd;
    allocate_distancemap(num_thrd);

    memset(t_time_explo, 0, 50 * sizeof(long));
    long seq_time = 0;

    std::binary_semaphore finish_1{0}, start_1{0};
    std::binary_semaphore finish_2{0}, start_2{0};
    std::binary_semaphore finish_3{0}, start_3{0};
    std::binary_semaphore finish_4{0}, start_4{0};
    std::binary_semaphore finish_5{0}, start_5{0};
    std::binary_semaphore finish_6{0}, start_6{0};
    std::binary_semaphore finish_7{0}, start_7{0};
    std::binary_semaphore finish_8{0}, start_8{0};
    std::binary_semaphore finish_9{0}, start_9{0};
    std::binary_semaphore finish_10{0}, start_10{0};
    std::binary_semaphore finish_11{0}, start_11{0};
    std::binary_semaphore finish_12{0}, start_12{0};
    std::binary_semaphore finish_13{0}, start_13{0};
    std::binary_semaphore finish_14{0}, start_14{0};

    threads[0] = std::thread(parLevelSetTraversal_v2, 0, std::ref(finish_1), std::ref(start_1));
    threads[1] = std::thread(parLevelSetTraversal_v2, 1, std::ref(finish_2), std::ref(start_2));
    threads[2] = std::thread(parLevelSetTraversal_v2, 2, std::ref(finish_3), std::ref(start_3));
    threads[3] = std::thread(parLevelSetTraversal_v2, 3, std::ref(finish_4), std::ref(start_4));
    threads[4] = std::thread(parLevelSetTraversal_v2, 4, std::ref(finish_5), std::ref(start_5));
    threads[5] = std::thread(parLevelSetTraversal_v2, 5, std::ref(finish_6), std::ref(start_6));
    threads[6] = std::thread(parLevelSetTraversal_v2, 6, std::ref(finish_7), std::ref(start_7));
    threads[7] = std::thread(parLevelSetTraversal_v2, 7, std::ref(finish_8), std::ref(start_8));
    threads[8] = std::thread(parLevelSetTraversal_v2, 8, std::ref(finish_9), std::ref(start_9));
    threads[9] = std::thread(parLevelSetTraversal_v2, 9, std::ref(finish_10), std::ref(start_10));

    auto start = std::chrono::high_resolution_clock::now();

    for (auto edge : queueEdges)
    {
        int p1, p2;
        std::tie(p1, p2) = edge_to_vertices(edge, im.getWidth());

        D->indice = 0;

        if (historyVisited[im.segments_[p1]] == false)
        {
            im.segments_[p1] = im.tagCount_;
            im.tagCount_++;
            historyVisited[im.segments_[p1]] = true;
            vertices[0] = p1;
            D->E[D->indice] = vertices[0];
            D->indice++;
        }
        else
        {
            vertices[0] = -1;
        }

        if (historyVisited[im.segments_[p2]] == false)
        {
            im.segments_[p2] = im.tagCount_;
            im.tagCount_++;
            historyVisited[im.segments_[p2]] = true;
            vertices[1] = p2;
            D->E[D->indice] = vertices[1];
            D->indice++;
        }
        else
        {
            vertices[1] = -1;
        }

        D->start[0] = 0;
        D->start[1] = 1;
        for (int i = 2; i < num_thrd; i++)
        {
            D->start[i] = 2;
        }


        //Main loop for exploring
        while (D->indice != 0)
        {
            if (D->indice >= thres) //If propagation is above 50 vertex we // the process
            {
                // Partition Function
                for (int i = 0; i < num_thrd; i++)
                {
                    seqPartition(i);
                }

                // Start of the threads
                start_1.release();
                start_2.release();
                start_3.release();
                start_4.release();
                start_5.release();
                start_6.release();
                start_7.release();
                start_8.release();
                start_9.release();
                start_10.release();

                // Waiting for the threads to finish
                finish_1.acquire();
                finish_2.acquire();
                finish_3.acquire();
                finish_4.acquire();
                finish_5.acquire();
                finish_6.acquire();
                finish_7.acquire();
                finish_8.acquire();
                finish_9.acquire();
                finish_10.acquire();

                //parLabelisation(D, num_thrd);
                // Union of the sets traversed by the threads to form th next level set stored in D->E
                setUnion();
                par_explo++;
            }
            else //else we don't
            {
                auto start_seq = std::chrono::high_resolution_clock::now();
                //std::cout << "Sequential exploration" << std::endl;
                int i, x, y, vertex;

                i = 0;

                int j = 0;

                //Exploration séquentiel

                int w = im_t_ptr->getWidth();
                int h = im_t_ptr->getHeight();
                int wh = w * h;

                for (x = 0; x < D->indice; x++)
                {
                    int v = D->E[x];
                    int tag = im_t_ptr->segments_[v];

                    int vRight = v + 1;
                    int vLeft = v - 1;
                    int vUp = v - w;
                    int vDown = v + w;

                    if (vRight < wh && (v + 1) % w != 0)
                    {
                        //check if adjacent to v exist

                        auto mst_edge = im_t_ptr->map_graph_mst[2 * v];
                        if (mst_edge != -1)
                        {
                            if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vRight] != tag)
                            {
                                im_t_ptr->segments_[vRight] = tag;
                                D->Si[i][j] = vRight;
                                j++;
                            }
                        }
                    }

                    if (vLeft >= 0 && v % w != 0)
                    {
                        auto mst_edge = im_t_ptr->map_graph_mst[(2 * v) - 2];
                        if (mst_edge != -1)
                        {
                            if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vLeft] != tag)
                            {
                                im_t_ptr->segments_[vLeft] = tag;
                                D->Si[i][j] = vLeft;
                                j++;
                            }
                        }
                    }

                    if (vDown < wh)
                    {
                        auto mst_edge = im_t_ptr->map_graph_mst[(2 * v) + 1];
                        if (mst_edge != -1)
                        {
                            if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vDown] != tag)
                            {
                                im_t_ptr->segments_[vDown] = tag;
                                D->Si[i][j] = vDown;
                                j++;
                            }
                        }
                    }

                    if (vUp >= 0)
                    {
                        auto mst_edge = im_t_ptr->map_graph_mst[((2 * v) - (2 * w)) + 1];
                        if (mst_edge != -1)
                        {
                            if (im_t_ptr->mstEdit_[mst_edge] && im_t_ptr->segments_[vUp] != tag)
                            {
                                im_t_ptr->segments_[vUp] = tag;
                                D->Si[i][j] = vUp;
                                j++;
                            }
                        }
                    }
                }
                D->TailleSi[i] = j; // number of element in each Si


                //================== Set Union ===================
                D->indice = j; // new after union |n|

                memcpy(D->E, D->Si[i], D->TailleSi[i] * sizeof(int));
                seq_explo++;
                auto end_seq = std::chrono::high_resolution_clock::now();
                auto duration_seq = std::chrono::duration_cast<std::chrono::nanoseconds>(end_seq - start_seq);
                seq_time += duration_seq.count();
            }
        }

        //if(cpt == 562) exit(42);
        //memset_distance_map(num_thrd);
        //std::cout << "Edge " << cpt << " / " << queueEdges.size() << std::endl;
        //if (vertices[0] != -1 || vertices[1] != -1)
        //Distance_Maps(D, num_thrd);
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    auto all_explo = seq_time;

    long max = LONG_MIN;

    //print exploration time for each thread
    for (int i = 0; i < num_thrd; i++)
    {
        std::cout << "Thread " << i << " exploration time is : " << t_time_explo[i] / 1000000 << " ms" << std::endl;
        if (t_time_explo[i] > max)
        {
            max = t_time_explo[i];
        }
    }

    all_explo += max;

    std::cout << "Time for sequential exploration : " << seq_time / 1000000 << " ms" << std::endl;

    std::cout << "Time for all exploration : " << all_explo / 1000000 << " ms" << std::endl;

    std::cout << "Time for eveything except exploration : " << (duration.count() - all_explo) / 1000000 << " ms" <<
        std::endl;

    std::cout << "Total time : " << duration.count() / 1000000 << " ms" << std::endl;

    std::cout << "Number of step of labelisation in par : " << par_explo << std::endl;

    std::cout << "Number of step of labelisation in seq : " << seq_explo << std::endl;

    std::cout << "Threshold for par : " << thres << std::endl;

    clean_distancemap();
    free(D);

    split = false;

    start_1.release();
    start_2.release();
    start_3.release();
    start_4.release();
    start_5.release();
    start_6.release();
    start_7.release();
    start_8.release();
    start_9.release();
    start_10.release();

    for(int tr = 0; tr < num_thrd; tr++)
    {
        threads[tr].join();
    }
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

    splitSegment_v3(im, historyVisited, queueEdges);

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

