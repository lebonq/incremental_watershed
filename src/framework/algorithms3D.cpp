//
// Created by lebonq on 31/01/24.
//

#include "algorithms3D.h"

int nb_threads3D = 10;

bool split3D = false;

int nb_pixels_visited3D = 0;

int* counter3D = new int[4];

long time_update_h3D;

long* t_time_explo3D = new long[50];

pthread_t* thread3D; // global variable used to store the threads id
struct DistanceMap* D3D; // global variable used to easily pass arguments to threads
int* vertices3D = new int[2];
int* size_cc3D = new int[2];

volumeManager* vol_t_ptr;

int algorithms3D::from3Dto1D(int x, int y, int z, int w, int h)
{
    return x + y * w + z * w * h;
}

std::tuple<int, int, int> algorithms3D::from1Dto3D(int p, int w, int h, int d)
{
    int z = p / (w * h);
    int y = (p - z * w * h) / w;
    int x = p - z * w * h - y * w;
    return std::make_tuple(x, y, z);
}


/*-----------------------------------------------Partition Function----------------------------------------*/
void* parPartition3D(void* slice)
{
    int i, ji, p, n, i2;
    int start;
    int end;

    p = D3D->p; // number of threads
    n = D3D->indice; // number of data in E
    i = ((long)slice) + 1;

    if (i <= (n % p))
    {
        start = (i - 1) * ((n / p) + 1);
        end = start + (n / p);
        D3D->TailleEi[i - 1] = (n / p) + 1;
    }
    else
    {
        start = (n % p) * ((n / p) + 1) + (i - 1 - (n % p)) * (n / p);
        end = start + (n / p) - 1;
        D3D->TailleEi[i - 1] = n / p; // to know the size of each Ei
    }

    for (ji = start; ji <= end; ji++)
        D3D->Ei[i - 1][ji - start] = D3D->E[ji];

    pthread_exit(NULL);
}

void seqPartition3D(int slice)
{
    int i, ji, p, n, i2;
    int start;
    int end;

    p = D3D->p; // number of threads
    n = D3D->indice; // number of data in E
    i = (slice) + 1;

    if (i <= (n % p))
    {
        start = (i - 1) * ((n / p) + 1);
        end = start + (n / p);
        auto new_taille_ei = (n / p) + 1;
        D3D->TailleEi[i - 1] = new_taille_ei;
    }
    else
    {
        start = (n % p) * ((n / p) + 1) + (i - 1 - (n % p)) * (n / p);
        end = start + (n / p) - 1;
        auto new_taille_ei = n / p;
        D3D->TailleEi[i - 1] = new_taille_ei; // to know the size of each Ei
    }

    for (ji = start; ji <= end; ji++)
        D3D->Ei[i - 1][ji - start] = D3D->E[ji];
}

/*-----------------------------------------------Union Function ----------------------------------------------------*/

void* copyForUnion3D(void* slice)
{
    //auto start = std::chrono::high_resolution_clock::now();
    int i, j;
    i = (long)slice;

    for (j = 0; j < (D3D->TailleSi[i]); j++)
    {
        auto value = D3D->Si[i][j];
        D3D->E[D3D->start[i] + j] = value;
    }

    //auto end = std::chrono::high_resolution_clock::now();
    //t_time_explo3D[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    pthread_exit(NULL);
}

void copyForUnionSeq3D(int slice)
{
    //auto start = std::chrono::high_resolution_clock::now();
    int i;
    i = slice;

    for (int j = 0; j < D3D->TailleSi[i]; j++)
    {
        auto value = D3D->Si[i][j];
        D3D->E[D3D->start[i] + j] = value;
    }

    //auto end = std::chrono::high_resolution_clock::now();
    //t_time_explo[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

/************************************************************/
void setUnion3D()
{
    int i, j, cmp, p;
    p = D3D->p; // number of threads
    cmp = 0;

    for (i = 0; i < p; i++)
    {
        // to calcul start of each processor
        D3D->start[i] = cmp;
        cmp = cmp + D3D->TailleSi[i];
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

        for (j = 0; j < D3D->TailleSi[i]; j++)
        {
            auto value = D3D->Si[i][j];
            D3D->E[D3D->start[i] + j] = value;
        }
    }
    /*for (i = 0; i < p; i++)
        pthread_join(thread[i], NULL);*/

    D3D->indice = cmp; // new after union |n|
}

/*-------------------------------------------------creation level-sets---------------------------------------------------*/
void* parLevelSetTraversal3D(void* slice)
{
    //auto start = std::chrono::high_resolution_clock::now();
    int i, j, x, y, vertex;

    j = 0;
    i = (long)slice;

    int w = vol_t_ptr->getWidth();
    int h = vol_t_ptr->getHeight();
    int wh = w * h;


    /* fprintf(stderr,"Taille de E[%d] = %d\n", i, D3D->TailleEi[i]); */
    /* fprintf(stderr,"Elements de E[%d]\n", i); */
    /* for(x = 0; x < D3D->TailleEi[i]; x++) fprintf(stderr, "%d-eme element %d, ", x, D3D->Ei[i][x]); */
    /* fprintf(stderr,".\n"); */

    for (x = 0; x < D3D->TailleEi[i]; x++)
    {
        // for each x in Ei
        int v = D3D->Ei[i][x];
        int tag = vol_t_ptr->segments_[v];
        /* fprintf(stderr,"Exploration des successeurs de %d\n", vertex); */

        int vRight = v + 1;
        int vLeft = v - 1;
        int vUp = v - w;
        int vDown = v + w;

        if (vRight < wh && (v + 1) % w != 0)
        {
            //check if adjacent to v exist

            auto mst_edge = vol_t_ptr->map_graph_mst_[2 * v];
            if (mst_edge != -1)
            {
                if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vRight] != tag)
                {
                    vol_t_ptr->segments_[vRight] = tag;
                    D3D->Si[i][j] = vRight;
                    j++;
                }
            }
        }

        if (vLeft >= 0 && v % w != 0)
        {
            auto mst_edge = vol_t_ptr->map_graph_mst_[(2 * v) - 2];
            if (mst_edge != -1)
            {
                if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vLeft] != tag)
                {
                    vol_t_ptr->segments_[vLeft] = tag;
                    D3D->Si[i][j] = vLeft;
                    j++;
                }
            }
        }

        if (vDown < wh)
        {
            auto mst_edge = vol_t_ptr->map_graph_mst_[(2 * v) + 1];
            if (mst_edge != -1)
            {
                if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vDown] != tag)
                {
                    vol_t_ptr->segments_[vDown] = tag;
                    D3D->Si[i][j] = vDown;
                    j++;
                }
            }
        }

        if (vUp >= 0)
        {
            auto mst_edge = vol_t_ptr->map_graph_mst_[((2 * v) - (2 * w)) + 1];
            if (mst_edge != -1)
            {
                if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vUp] != tag)
                {
                    vol_t_ptr->segments_[vUp] = tag;
                    D3D->Si[i][j] = vUp;
                    j++;
                }
            }
        }
    }
    D3D->TailleSi[i] = j; // number of element in each Si
    //auto end = std::chrono::high_resolution_clock::now();
    //t_time_explo3D[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    pthread_exit(NULL);
}

void parLevelSetTraversal_v2_3D(int slice, std::binary_semaphore& finish, std::binary_semaphore& start)

{
    while (split3D == true)
    {
        start.acquire();

        //Check if split3D is == false
        if (split3D == false) return;

        //auto start_t = std::chrono::high_resolution_clock::now();

        int i, j, x, y, vertex;

        j = 0;
        i = slice;

        int w = vol_t_ptr->getWidth();
        int h = vol_t_ptr->getHeight();
        int d = vol_t_ptr->getDepth();
        int wh = w * h;


        /* fprintf(stderr,"Taille de E[%d] = %d\n", i, D3D->TailleEi[i]); */
        /* fprintf(stderr,"Elements de E[%d]\n", i); */
        /* for(x = 0; x < D3D->TailleEi[i]; x++) fprintf(stderr, "%d-eme element %d, ", x, D3D->Ei[i][x]); */
        /* fprintf(stderr,".\n"); */

        for (x = 0; x < D3D->TailleEi[i]; x++)
        {
            // for each x in Ei
            int v = D3D->Ei[i][x];
                   int tag = vol_t_ptr->segments_[v];

                    std::tuple<int, int, int> v_3D = algorithms3D::from1Dto3D(v, w, h, d);
                    //vol_t_ptr->segments_[v] = tag;

                    int vRight_1D = v + 1;
                    int vLeft_1D = v - 1;
                    int vUp_1D = v - w;
                    int vDown_1D = v + w;
                    int vFoward_1D = v + wh;
                    int vBackward_1D = v - wh;


                    //Right
                    if (std::get<0>(v_3D) + 1 < w)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vRight_1D] != tag)
                            {
                                vol_t_ptr->segments_[vRight_1D] = tag;
                                D3D->Sipp[i][j] = vRight_1D;
                                j++;
                            }
                        }
                    }

                    //left
                    if (std::get<0>(v_3D)-1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * (v - 1)];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vLeft_1D] != tag)
                            {
                                vol_t_ptr->segments_[vLeft_1D] = tag;
                                D3D->Sipp[i][j] = vLeft_1D;
                                j++;
                            }
                        }

                    }

                    //Down
                    if (std::get<1>(v_3D) + 1 < h)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v + 1];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vDown_1D] != tag)
                            {
                                vol_t_ptr->segments_[vDown_1D] = tag;
                                D3D->Sipp[i][j] = vDown_1D;
                                j++;
                            }
                        }
                    }

                    //Up
                    if (std::get<1>(v_3D) - 1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[(v - w) * 3 + 1];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vUp_1D] != tag)
                            {
                                vol_t_ptr->segments_[vUp_1D] = tag;
                                D3D->Sipp[i][j] = vUp_1D;
                                j++;
                            }
                        }
                    }


                    //Foward
                    if (std::get<2>(v_3D) + 1 < d)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v + 2];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vFoward_1D] != tag)
                            {
                                vol_t_ptr->segments_[vFoward_1D] = tag;
                                D3D->Sipp[i][j] = vFoward_1D;
                                j++;
                            }
                        }
                    }

                    //backward
                    if (std::get<2>(v_3D) - 1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[(v - w * h) * 3 + 2];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vBackward_1D] != tag)
                            {
                                vol_t_ptr->segments_[vBackward_1D] = tag;
                                D3D->Sipp[i][j] = vBackward_1D;
                                j++;
                            }
                        }
                    }
        }
        D3D->TailleSipp[i] = j; // number of element in each Si

        j = 0;
        for (x = 0; x < D3D->TailleSipp[i]; x++)
        {
            // for each x in Ei
            int v = D3D->Sipp[i][x];
                   int tag = vol_t_ptr->segments_[v];

                    std::tuple<int, int, int> v_3D = algorithms3D::from1Dto3D(v, w, h, d);
                    vol_t_ptr->segments_[v] = tag;

                    int vRight_1D = v + 1;
                    int vLeft_1D = v - 1;
                    int vUp_1D = v - w;
                    int vDown_1D = v + w;
                    int vFoward_1D = v + wh;
                    int vBackward_1D = v - wh;


                    //Right
                    if (std::get<0>(v_3D) + 1 < w)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vRight_1D] != tag)
                            {
                                vol_t_ptr->segments_[vRight_1D] = tag;
                                D3D->Sip[i][j] = vRight_1D;
                                j++;
                            }
                        }
                    }

                    //left
                    if (std::get<0>(v_3D)-1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * (v - 1)];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vLeft_1D] != tag)
                            {
                                vol_t_ptr->segments_[vLeft_1D] = tag;
                                D3D->Sip[i][j] = vLeft_1D;
                                j++;
                            }
                        }

                    }

                    //Down
                    if (std::get<1>(v_3D) + 1 < h)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v + 1];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vDown_1D] != tag)
                            {
                                vol_t_ptr->segments_[vDown_1D] = tag;
                                D3D->Sip[i][j] = vDown_1D;
                                j++;
                            }
                        }
                    }

                    //Up
                    if (std::get<1>(v_3D) - 1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[(v - w) * 3 + 1];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vUp_1D] != tag)
                            {
                                vol_t_ptr->segments_[vUp_1D] = tag;
                                D3D->Sip[i][j] = vUp_1D;
                                j++;
                            }
                        }
                    }


                    //Foward
                    if (std::get<2>(v_3D) + 1 < d)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v + 2];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vFoward_1D] != tag)
                            {
                                vol_t_ptr->segments_[vFoward_1D] = tag;
                                D3D->Sip[i][j] = vFoward_1D;
                                j++;
                            }
                        }
                    }

                    //backward
                    if (std::get<2>(v_3D) - 1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[(v - w * h) * 3 + 2];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vBackward_1D] != tag)
                            {
                                vol_t_ptr->segments_[vBackward_1D] = tag;
                                D3D->Sip[i][j] = vBackward_1D;
                                j++;
                            }
                        }
                    }
        }
        D3D->TailleSip[i] = j; // number of element in each Si

        j = 0;
        for (x = 0; x < D3D->TailleSip[i]; x++)
        {

            int v = D3D->Sip[i][x];
                    int tag = vol_t_ptr->segments_[v];

                    std::tuple<int, int, int> v_3D = algorithms3D::from1Dto3D(v, w, h, d);
                    vol_t_ptr->segments_[v] = tag;

                    int vRight_1D = v + 1;
                    int vLeft_1D = v - 1;
                    int vUp_1D = v - w;
                    int vDown_1D = v + w;
                    int vFoward_1D = v + wh;
                    int vBackward_1D = v - wh;


                    //Right
                    if (std::get<0>(v_3D) + 1 < w)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vRight_1D] != tag)
                            {
                                vol_t_ptr->segments_[vRight_1D] = tag;
                                D3D->Si[i][j] = vRight_1D;
                                j++;
                            }
                        }
                    }

                    //left
                    if (std::get<0>(v_3D)-1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * (v - 1)];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vLeft_1D] != tag)
                            {
                                vol_t_ptr->segments_[vLeft_1D] = tag;
                                D3D->Si[i][j] = vLeft_1D;
                                j++;
                            }
                        }

                    }

                    //Down
                    if (std::get<1>(v_3D) + 1 < h)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v + 1];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vDown_1D] != tag)
                            {
                                vol_t_ptr->segments_[vDown_1D] = tag;
                                D3D->Si[i][j] = vDown_1D;
                                j++;
                            }
                        }
                    }

                    //Up
                    if (std::get<1>(v_3D) - 1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[(v - w) * 3 + 1];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vUp_1D] != tag)
                            {
                                vol_t_ptr->segments_[vUp_1D] = tag;
                                D3D->Si[i][j] = vUp_1D;
                                j++;
                            }
                        }
                    }


                    //Foward
                    if (std::get<2>(v_3D) + 1 < d)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v + 2];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vFoward_1D] != tag)
                            {
                                vol_t_ptr->segments_[vFoward_1D] = tag;
                                D3D->Si[i][j] = vFoward_1D;
                                j++;
                            }
                        }
                    }

                    //backward
                    if (std::get<2>(v_3D) - 1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[(v - w * h) * 3 + 2];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vBackward_1D] != tag)
                            {
                                vol_t_ptr->segments_[vBackward_1D] = tag;
                                D3D->Si[i][j] = vBackward_1D;
                                j++;
                            }
                        }
                    }
        }
        D3D->TailleSi[i] = j; // number of element in each Si

        //auto end = std::chrono::high_resolution_clock::now();
        //t_time_explo3D[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_t).count();
        finish.release();
    }
}

/*--------------------------------------Initialization---------------------------------------------------------------*/
void* parInit3D(void* param)
/* Parallel part of the initialization of the parallel distance map algorithm, ie creation of the 0-level set of the diastance map */
{
    int proc = (long)param;


    if (proc <= 1) //For the first 2 thread
    {
        if (vertices3D[proc] != -1)
        {
            D3D->Si[proc][0] = vertices3D[proc];
            //pthread_mutex_lock(&D3D->Traversed[vertices[proc]]);
            D3D->TailleSi[proc] = 1; // number of element in each Si
        }
    }
    else
    {
        D3D->TailleSi[proc] = 0;
    }

    pthread_exit(NULL);
}

void dMapInit3D(int p)
/* Initialize the structure used during parallel distance map
   computation */
{
    int i;

    for (i = 0; i < p; i++)
    {
        if (pthread_create(&thread3D[i], NULL, parInit3D, (void*)(long)i) != 0)
        {
            perror("Can't create thread");
            free(thread3D);
            exit(-1);
        }
    }
    for (i = 0; i < p; i++)
        pthread_join(thread3D[i], NULL); // threads join

    //print TailleSi

    setUnion3D();
}

void allocate_distancemap3D(int p)
{
    auto& g = vol_t_ptr->getGraph();

    int i;

    D3D->E = (int*)malloc(g.getNbVertex() * sizeof(int));
    memset(D3D->E, 0, g.getNbVertex() * sizeof(int));

    D3D->Si = (int**)malloc(p * sizeof(int*));
    D3D->Sip = (int**)malloc(p * sizeof(int*));
    D3D->Sipp = (int**)malloc(p * sizeof(int*));
    D3D->Ei = (int**)malloc(p * sizeof(int*));

    for (i = 0; i < p; i++)
    {
        D3D->Ei[i] = (int*)malloc(g.getNbVertex() * sizeof(int));
        memset(D3D->Ei[i], 0, g.getNbVertex() * sizeof(int));
        D3D->Si[i] = (int*)malloc(g.getNbVertex() * sizeof(int));
        D3D->Sip[i] = (int*)malloc(g.getNbVertex() * sizeof(int));
        D3D->Sipp[i] = (int*)malloc(g.getNbVertex() * sizeof(int));
    }

    D3D->TailleSi = (int*)malloc(p * sizeof(int));
    D3D->TailleSip = (int*)malloc(p * sizeof(int));
    D3D->TailleSipp = (int*)malloc(p * sizeof(int));

    D3D->TailleEi = (int*)malloc(p * sizeof(int));

    D3D->start = (int*)malloc(p * sizeof(int));
    memset(D3D->start, 0, p * sizeof(int));

    thread3D = (pthread_t*)malloc(p * sizeof(pthread_t)); //  threads array
}

/*----------------------------------------------------------------------------------------------------------------------*/

void clean_distancemap3D()
{
    // Free the memory allocated for each array in D3D->Ei and D3D->Si
    for (int i = 0; i < D3D->p; i++)
    {
        free(D3D->Ei[i]);
        free(D3D->Si[i]);
        free(D3D->Sip[i]);
        free(D3D->Sipp[i]);
    }

    // Free the memory allocated for the arrays in the DistanceMap structure
    //free(D3D->Traversed);
    free(D3D->E);
    free(D3D->Si);
    free(D3D->Sip);
    free(D3D->Sipp);
    free(D3D->Ei);
    free(D3D->TailleSi);
    free(D3D->TailleSip);
    free(D3D->TailleSipp);
    free(D3D->TailleEi);
    free(D3D->start);
    free(thread3D);

    // Set all the pointers to NULL to avoid dangling pointers
    //D3D->Traversed = NULL;
    D3D->E = NULL;
    D3D->Si = NULL;
    D3D->Sip = NULL;
    D3D->Sipp = NULL;
    D3D->Ei = NULL;
    D3D->TailleSi = NULL;
    D3D->TailleEi = NULL;
    D3D->start = NULL;
}

/***************************************** Parallel Distance Map **********************************************/
void Distance_Maps3D(struct DistanceMap* D, int p)
{
    int i, j, nb_level, r, som, d;

    // Run the code you want to benchmark
    dMapInit3D(p);

    /* fprintf(stderr, "Allocation des structures necessaires a la carte de distance ok\n"); */

    while (D3D->indice != 0)
    {
        /* fprintf(stderr, "D�but du traitement de niveau %d\n", nb_level); */
        // Partition Function
        for (i = 0; i < p; i++)
        {
            if (pthread_create(&thread3D[i], NULL, parPartition3D, (void*)(long)i) != 0)
            {
                perror("Can't create thread");
                free(thread3D);
                exit(-1);
            }
        }
        for (i = 0; i < p; i++)
            pthread_join(thread3D[i], NULL); // threads join

        /* fprintf(stderr, "Niveau %d: partition ok\n", nb_level); */

        // Creation of level-set
        for (i = 0; i < p; i++)
        {
            // for each processor i
            if (pthread_create(&thread3D[i], NULL, parLevelSetTraversal3D, (void*)(long)i) != 0)
            {
                perror("Can't create thread");
                free(thread3D);
                exit(-1);
            }
        }
        for (i = 0; i < p; i++)
            pthread_join(thread3D[i], NULL); // threads join

        // Union of the sets traversed by the threads to form th next level set stored in D3D->E
        setUnion3D();

        /*for (i = 0; i < p; i++)
        {
            fprintf(stderr,"Taille de Si[%d] = %d\n", i, D3D->TailleSi[i]);
        }*/
    }

    //fprintf(stderr, "Carte de Distance calculee\n");
}

void parLabelisation3D(struct DistanceMap* D, int p)
{
    int i, j, nb_level, r, som, d;

    /* fprintf(stderr, "Niveau %d: partition ok\n", nb_level); */

    // Creation of level-set
    for (i = 0; i < p; i++)
    {
        // for each processor i
        if (pthread_create(&thread3D[i], NULL, parLevelSetTraversal3D, (void*)(long)i) != 0)
        {
            perror("Can't create thread");
            free(thread3D);
            exit(-1);
        }
    }
    for (i = 0; i < p; i++)
        pthread_join(thread3D[i], NULL); // threads join
}


int algorithms3D::breadthFirstSearchLabel(volumeManager& vol, int tag, int p)
{
    std::vector<int> queue;
    queue.push_back(p);
    int count = 1;
    int w = vol.getWidth();
    int h = vol.getHeight();
    int d = vol.getDepth();
    int wh = w * h;

    //print wich id of MST EDIT is true
    while (!queue.empty())
    {
        int v_1D = queue.back();
        std::tuple<int, int, int> v_3D = from1Dto3D(v_1D, w, h, d);
        queue.pop_back();
        vol.segments_[v_1D] = tag;

        int vRight_1D = v_1D + 1;
        int vLeft_1D = v_1D - 1;
        int vUp_1D = v_1D - w;
        int vDown_1D = v_1D + w;
        int vFoward_1D = v_1D + wh;
        int vBackward_1D = v_1D - wh;


        //Right
        if (std::get<0>(v_3D) + 1 < w)
        {
            //check if adjacent to v exist
            if (vol.isInMStEdit(vol.map_graph_mst_[3 * v_1D]) == true &&
                vol.segments_[vRight_1D] != tag)
            {
                //If yes we check if the edge is revealnt and present in MST
                queue.push_back(vRight_1D);
                vol.segments_[vRight_1D] = tag;
                count++;
            }
        }

        //left
        if (std::get<0>(v_3D)-1 >= 0)
        {
            if (vol.isInMStEdit(vol.map_graph_mst_[3 * (v_1D - 1)]) == true && vol.segments_[vLeft_1D] != tag)
            {
                queue.push_back(vLeft_1D);
                vol.segments_[vLeft_1D] = tag;
                count++;
            }
        }

        //Down
        if (std::get<1>(v_3D) + 1 < h)
        {
            if (vol.isInMStEdit(vol.map_graph_mst_[3 * v_1D + 1]) == true && vol.segments_[vDown_1D] != tag)
            {
                queue.push_back(vDown_1D);
                vol.segments_[vDown_1D] = tag;
                count++;
            }
        }

        //Up
        if (std::get<1>(v_3D) - 1 >= 0)
        {
            if (vol.isInMStEdit(vol.map_graph_mst_[(v_1D - w) * 3 + 1]) == true && vol.segments_[vUp_1D] != tag)
            {
                queue.push_back(vUp_1D);
                vol.segments_[vUp_1D] = tag;
                count++;
            }
        }


        //Foward
        if (std::get<2>(v_3D) + 1 < d)
        {
            if (vol.isInMStEdit(vol.map_graph_mst_[3 * v_1D + 2]) == true && vol.segments_[vFoward_1D] != tag)
            {
                queue.push_back(vFoward_1D);
                vol.segments_[vFoward_1D] = tag;
                count++;
            }
        }

        //backward
        if (std::get<2>(v_3D) - 1 >= 0)
        {

            if (vol.isInMStEdit(vol.map_graph_mst_[(v_1D - w * h) * 3 + 2]) == true && vol.segments_[
                vBackward_1D] != tag)
            {
                queue.push_back(vBackward_1D);
                vol.segments_[vBackward_1D] = tag;
                count++;
            }
        }
    }
    return count;
}

void algorithms3D::kruskal(graph& G, Q& Q, int w, int h, int* temp)
{
    //Creation of sigleton for each vertex
    for (int i = 0; i < G.getNbVertex(); i++)
    {
        Q.makeSet(i);
    }

    std::vector<int>& mst = G.getMst();
    int count = 0;

    int prev = 1;

    // Loop over all possible pixel intensity values from 0 to 255.
    // This is done because the algorithm is processing the graph edges based on their weights,

    int sum = 0;

    for (int j = 0; j < G.getHistSize(); j++)
    {
        for (int i = 0; i < G.count_[j]; i++)
        {
            int cx, cy = 0;
            int edge = G.sortedEdges_[j][i];

            //std::cerr << "edge : " << edge << std::endl;

            int dir = edge % 3; //Compute which direction the edge is

            cx = (edge - dir) / 3; //Compute the orign of the edge

            switch (dir)
            {
            //Compute the destination of the edge
            case 0:
                cy = cx + 1;
                break;
            case 1:
                cy = cx + w;
                break;
            case 2:
                cy = cx + w * h;
                break;
            default:
                std::cout << "Should not happen" << std::endl;
                exit(-1);
            }

            //std::cerr << "cx : " << cx << " cy : " << cy << std::endl;

            cx = Q.findCannonical(cx);
            cy = Q.findCannonical(cy);
            //std::cout << "Canonical : cx : " << cx << " cy : " << cy << std::endl;

            if (cx != cy)
            {
                Q.makeUnion(cx, cy);
                mst.push_back(edge); //add edge in mst

                sum+=j;

                temp[edge] = count; // Allow us to know where an edge is in the MST
                count++;
            }
        }
    }
    //std::cerr << "There is " << count << " edges in the hierarchy" << std::endl;
}

void algorithms3D::splitSegment(volumeManager& vol, std::vector<bool>& historyVisited,
                                std::vector<int>& queueEdges)
{
    int p1, p2, tag1, tag2, newTag; //Are being erased
    int w = vol.getWidth();
    int h = vol.getHeight();

    for (int edge : queueEdges)
    {
        int dir = edge % 3; //Compute which direction the edge is

        p1 = (edge - dir) / 3; //Compute the orign of the edge

        switch (dir)
        {
        //Compute the destination of the edge
        case 0:
            p2 = p1 + 1;
            break;
        case 1:
            p2 = p1 + w;
            break;
        case 2:
            p2 = p1 + w * h;
            break;
        }


        tag1 = vol.segments_[p1];
        tag2 = vol.segments_[p2];

        if (!historyVisited[tag2])
        {
            newTag = vol.getTagCount();
            vol.setTagCount(newTag + 1);

            vol.getSizePart()[newTag] = breadthFirstSearchLabel(vol, newTag, p2);
            vol.getSizePart()[tag2] -= vol.getSizePart()[newTag];

            historyVisited[newTag] = true;
        }

        if (!historyVisited[tag1])
        {
            newTag = vol.getTagCount();
            vol.setTagCount(newTag + 1);

            vol.getSizePart()[newTag] = breadthFirstSearchLabel(vol, newTag, p1);
            vol.getSizePart()[tag1] -= vol.getSizePart()[newTag];

            historyVisited[newTag] = true;
        }
    }

}

std::tuple<int, int> algorithms3D::edge_to_vertices(const int edge, const int w, const int h)
{
    int p1, p2;
    int dir = edge % 3; //Compute which direction the edge is

    p1 = (edge - dir) / 3; //Compute the orign of the edge

    switch (dir)
    {
        //Compute the destination of the edge
    case 0:
        p2 = p1 + 1;
        break;
    case 1:
        p2 = p1 + w;
        break;
    case 2:
        p2 = p1 + w * h;
        break;
    }


    return std::make_tuple(p1, p2);
}

void algorithms3D::init_dmap(volumeManager* vol)
{
    vol_t_ptr = vol;
    D3D = (DistanceMap*)malloc(sizeof(struct DistanceMap)); // Distance map structure
    D3D->p = nb_threads3D;
    allocate_distancemap3D(nb_threads3D);
}

void algorithms3D::deinit_dmap()
{
    clean_distancemap3D();
    free(D3D);
}



void algorithms3D::splitSegment_v3(volumeManager& vol, std::vector<bool>& historyVisited, std::vector<int>& queueEdges)
{
    split3D = true;

    //auto start_alloc = std::chrono::high_resolution_clock::now();
    int thres = vol.threshold_; // threshold for parallel exploration
    int par_explo = 0;
    int seq_explo = 0;
    //D3D = (DistanceMap*)malloc(sizeof(struct DistanceMap)); // Distance map structure
    //num_thrd = 10;
    std::vector<std::thread> threads(nb_threads3D);

    memset(t_time_explo3D, 0, 50 * sizeof(long));
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

    /*std::binary_semaphore** finish_sem = (std::binary_semaphore**)malloc(14 * sizeof(std::binary_semaphore));
    std::binary_semaphore** start_sem = (std::binary_semaphore**)malloc(14 * sizeof(std::binary_semaphore));


    for(int sem = 0; sem < 14; sem++)
    {
        auto temp_start = std::binary_semaphore(0);
        finish_sem[sem] = &temp_start;
        auto temp_fin = std::binary_semaphore(0);
        start_sem[sem] = &temp_fin;
    }*/


    threads[0] = std::thread(parLevelSetTraversal_v2_3D, 0, std::ref(finish_1), std::ref(start_1));
    threads[1] = std::thread(parLevelSetTraversal_v2_3D, 1, std::ref(finish_2), std::ref(start_2));
    threads[2] = std::thread(parLevelSetTraversal_v2_3D, 2, std::ref(finish_3), std::ref(start_3));
    threads[3] = std::thread(parLevelSetTraversal_v2_3D, 3, std::ref(finish_4), std::ref(start_4));
    threads[4] = std::thread(parLevelSetTraversal_v2_3D, 4, std::ref(finish_5), std::ref(start_5));
    threads[5] = std::thread(parLevelSetTraversal_v2_3D, 5, std::ref(finish_6), std::ref(start_6));
    threads[6] = std::thread(parLevelSetTraversal_v2_3D, 6, std::ref(finish_7), std::ref(start_7));
    threads[7] = std::thread(parLevelSetTraversal_v2_3D, 7, std::ref(finish_8), std::ref(start_8));
    threads[8] = std::thread(parLevelSetTraversal_v2_3D, 8, std::ref(finish_9), std::ref(start_9));
    threads[9] = std::thread(parLevelSetTraversal_v2_3D, 9, std::ref(finish_10), std::ref(start_10));
    /*threads[10] = std::thread(parLevelSetTraversal_v2, 10, std::ref(finish_11), std::ref(start_11));
    threads[11] = std::thread(parLevelSetTraversal_v2, 11, std::ref(finish_12), std::ref(start_12));
    threads[12] = std::thread(parLevelSetTraversal_v2, 12, std::ref(finish_13), std::ref(start_13));
    /*threads[13] = std::thread(parLevelSetTraversal_v2, 13, std::ref(finish_14), std::ref(start_14));*/

    /*for(int t = 0; t < nb_threads3D; t++)
    {
        threads[t] = std::thread(parLevelSetTraversal_v2_3D, t, std::ref(*finish_sem[t]), std::ref(*start_sem[t]));
    }*/



    //auto end_alloc = std::chrono::high_resolution_clock::now();
    //auto diff_alloc = std::chrono::duration_cast<std::chrono::nanoseconds>(end_alloc - start_alloc).count();

    //auto start = std::chrono::high_resolution_clock::now();

    long test = 0;

    for (auto edge : queueEdges)
    {
        int p1, p2;
        std::tie(p1, p2) = edge_to_vertices(edge, vol.getWidth(), vol.getHeight());

        D3D->indice = 0;

        if (historyVisited[vol.segments_[p1]] == false)
        {
            vol.segments_[p1] = vol.tagCount_;
            vol.tagCount_++;
            historyVisited[vol.segments_[p1]] = true;
            vertices3D[0] = p1;
            D3D->E[D3D->indice] = vertices3D[0];
            D3D->indice++;
        }
        else
        {
            vertices3D[0] = -1;
        }

        if (historyVisited[vol.segments_[p2]] == false)
        {
            vol.segments_[p2] = vol.tagCount_;
            vol.tagCount_++;
            historyVisited[vol.segments_[p2]] = true;
            vertices3D[1] = p2;
            D3D->E[D3D->indice] = vertices3D[1];
            D3D->indice++;
        }
        else
        {
            vertices3D[1] = -1;
        }

        D3D->start[0] = 0;
        D3D->start[1] = 1;
        for (int i = 2; i < nb_threads3D; i++)
        {
            D3D->start[i] = 2;
        }



        //Main loop for exploring
        while (D3D->indice != 0)
        {
            if (D3D->indice > thres) //If propagation is above 50 vertex we // the process
            {

                //while( D3D->indice < thres)
                //{
                    //auto start_test = std::chrono::high_resolution_clock::now();
                    // Partition Function
                    for (int i = 0; i < nb_threads3D; i++)
                    {
                        seqPartition3D(i);
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
                    /*start_11.release();
                    start_12.release();
                    start_13.release();*/


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
                    /*finish_11.acquire();
                    finish_12.acquire();
                    finish_13.acquire();*/

                    //parLabelisation(D, num_thrd);
                    // Union of the sets traversed by the threads to form th next level set stored in D3D->E

                    setUnion3D();

                    //auto end_t = std::chrono::high_resolution_clock::now();
                    //auto diff_t = std::chrono::duration_cast<std::chrono::nanoseconds>(end_t - start_test).count();

                    //test += diff_t;
                    par_explo++;
                //}
            }
            else //else we don't
            {
                //auto start_seq = std::chrono::high_resolution_clock::now();
                //std::cout << "Sequential exploration" << std::endl;
                int i, x, y, vertex;

                i = 0;

                int j = 0;

                //Exploration séquentiel

                int w = vol_t_ptr->getWidth();
                int h = vol_t_ptr->getHeight();
                int d = vol_t_ptr->getDepth();
                int wh = w * h;

                for (x = 0; x < D3D->indice; x++)
                {
                    int v = D3D->E[x];
                    int tag = vol_t_ptr->segments_[v];

                    std::tuple<int, int, int> v_3D = from1Dto3D(v, w, h, d);
                    vol.segments_[v] = tag;

                    int vRight_1D = v + 1;
                    int vLeft_1D = v - 1;
                    int vUp_1D = v - w;
                    int vDown_1D = v + w;
                    int vFoward_1D = v + wh;
                    int vBackward_1D = v - wh;


                    //Right
                    if (std::get<0>(v_3D) + 1 < w)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vRight_1D] != tag)
                            {
                                vol_t_ptr->segments_[vRight_1D] = tag;
                                D3D->Si[i][j] = vRight_1D;
                                j++;
                            }
                        }
                    }

                    //left
                    if (std::get<0>(v_3D)-1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * (v - 1)];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vLeft_1D] != tag)
                            {
                                vol_t_ptr->segments_[vLeft_1D] = tag;
                                D3D->Si[i][j] = vLeft_1D;
                                j++;
                            }
                        }

                    }

                    //Down
                    if (std::get<1>(v_3D) + 1 < h)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v + 1];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vDown_1D] != tag)
                            {
                                vol_t_ptr->segments_[vDown_1D] = tag;
                                D3D->Si[i][j] = vDown_1D;
                                j++;
                            }
                        }
                    }

                    //Up
                    if (std::get<1>(v_3D) - 1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[(v - w) * 3 + 1];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vUp_1D] != tag)
                            {
                                vol_t_ptr->segments_[vUp_1D] = tag;
                                D3D->Si[i][j] = vUp_1D;
                                j++;
                            }
                        }
                    }


                    //Foward
                    if (std::get<2>(v_3D) + 1 < d)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[3 * v + 2];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vFoward_1D] != tag)
                            {
                                vol_t_ptr->segments_[vFoward_1D] = tag;
                                D3D->Si[i][j] = vFoward_1D;
                                j++;
                            }
                        }
                    }

                    //backward
                    if (std::get<2>(v_3D) - 1 >= 0)
                    {
                        auto mst_edge = vol_t_ptr->map_graph_mst_[(v - w * h) * 3 + 2];
                        if (mst_edge != -1)
                        {
                            if (vol_t_ptr->mstEdit_[mst_edge] && vol_t_ptr->segments_[vBackward_1D] != tag)
                            {
                                vol_t_ptr->segments_[vBackward_1D] = tag;
                                D3D->Si[i][j] = vBackward_1D;
                                j++;
                            }
                        }
                    }
                }


                D3D->TailleSi[i] = j; // number of element in each Si


                //================== Set Union ===================
                D3D->indice = j; // new after union |n|

                memcpy(D3D->E, D3D->Si[i], D3D->TailleSi[i] * sizeof(int));
                seq_explo++;
                //auto end_seq = std::chrono::high_resolution_clock::now();
                //auto duration_seq = std::chrono::duration_cast<std::chrono::nanoseconds>(end_seq - start_seq);
                //seq_time += duration_seq.count();
            }
        }

        //if(cpt == 562) exit(42);
        //memset_distance_map(num_thrd);
        //std::cout << "Edge " << cpt << " / " << queueEdges.size() << std::endl;
        //if (vertices[0] != -1 || vertices[1] != -1)
        //Distance_Maps(D, num_thrd);
    }

    //auto end = std::chrono::high_resolution_clock::now();

    //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);



    /*auto all_explo = seq_time;

    long max = LONG_MIN;

    //print exploration time for each thread
    for (int i = 0; i < nb_threads3D; i++)
    {
        std::cout << "Thread " << i << " exploration time is : " << t_time_explo3D[i] / 1000000 << " ms" << std::endl;
        if (t_time_explo3D[i] > max)
        {
            max = t_time_explo3D[i];
        }
    }

    all_explo += max;

    std::cout << "Time for sequential exploration : " << seq_time / 1000000 << " ms" << std::endl;

    std::cout << "Time for all exploration : " << all_explo / 1000000 << " ms" << std::endl;

    std::cout << "Time for eveything except exploration : " << (duration.count() - all_explo) / 1000000 << " ms" <<
        std::endl;

    std::cout << "Total time w/o alloc : " << (duration.count() + time_update_h3D) / 1000000 << " ms" << std::endl;

    vol.time_wo_alloc_.emplace_back((duration.count() + time_update_h3D));

    std::cout << "Total time with alloc : " << (duration.count() + time_update_h3D + diff_alloc) / 1000000 << " ms" <<
        std::endl;

    std::cout << "Number of step of labelisation in par : " << par_explo << std::endl;

    std::cout << "Number of step of labelisation in seq : " << seq_explo << std::endl;

    std::cout << "Threshold for par : " << thres << std::endl;

    std::cout << "Time when thread are doing other than exploring is : " << (test-max) / 1000000 << " ms" << std::endl;

    std::cout << "Time thread + sync time : " << (test) / 1000000 << " ms" << std::endl;

    vol.time_thread_sync_.emplace_back(test/1000000);
    vol.time_seq_.emplace_back(seq_time/1000000);
    vol.time_total_.emplace_back((duration.count() + time_update_h3D + diff_alloc)/1000000);*/

    split3D = false;

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
    /*start_11.release();
    start_12.release();
    start_13.release();*/

    for (int tr = 0; tr < nb_threads3D; tr++)
    {
        threads[tr].join();
    }

    //end_alloc = std::chrono::high_resolution_clock::now();

    //diff_alloc += std::chrono::duration_cast<std::chrono::nanoseconds>(end_alloc - start_alloc).count();

}

/**
 * Merge 2 segments
 * @param p1 Point on the first segment
 * @param p2 Point on the second segment
 * @param vol
 */
void algorithms3D::mergeSegment(int edge, volumeManager& vol)
{
    std::vector<int> seg = vol.segments_;
    std::vector<int> sizeSeg = vol.getSizePart();
    int p1, p2;

    if (((edge) & (1 << (0))) == 1)
    {
        //if odd or even not the same formula
        //Copy and paste of line #22 to avoid doing jump into memory
        p1 = edge / 2;
        p2 = (edge / 2) + vol.getWidth();
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

    int count = breadthFirstSearchLabel(vol, tag1, p2);
}

void algorithms3D::removeMarker(volumeManager& vol, std::vector<int>& markers, int nbMarkers)
{
    int marker, up = 0;
    QBT& qbt = vol.getHierarchy().getQBT();
    int* parent = qbt.getParents();
    auto mstL = vol.getGraph().getMst();
    auto marks = vol.getMarks();
    auto ws = vol.getWs();
    auto mstEdit = vol.mstEdit_;

    for (int i = 0; i < nbMarkers; i++)
    {
        marker = markers[i];
        up = parent[marker];
        while (up != -1)
        {
            marks[up]--;
            if (marks[up] == 1)
            {
                ws[vol.getEdge(up)] = false;
                mstEdit[vol.getEdge(up)] = true;
                mergeSegment(mstL[vol.getEdge(up)], vol);
                break;
            }
            up = parent[up];
        }
    }
}

void algorithms3D::addMarker(volumeManager& vol, std::vector<int>& markers)
{
    //auto start_h = std::chrono::high_resolution_clock::now();
    int up = 0;
    QBT& qbt = vol.getHierarchy().getQBT();
    int* parent = qbt.getParents();
    std::vector<bool> historyVisited(vol.getGraph().getNbVertex());

    std::vector<int> queueEdges;


    for (auto marker : markers)
    {
        up = parent[marker];
        while (up != -1)
        {
            vol.modifyMarks(up, vol.getMarks()[up] + 1);
            if (vol.getMarks()[up] == 2)
            {
                vol.modifyWs(vol.getEdge(up),true);
                vol.mstEdit_[vol.getEdge(up)]  = false;
                queueEdges.push_back(vol.getGraph().getMst()[vol.getEdge(up)]);
                break;
            }
            up = parent[up];
        }
    }
    //auto end_h = std::chrono::high_resolution_clock::now();
    //time_update_h3D = std::chrono::duration_cast<std::chrono::nanoseconds>(end_h - start_h).count();


    // Get the start time
    auto start = std::chrono::high_resolution_clock::now();

    // Run the code you want to benchmark
    splitSegment_v3(vol, historyVisited, queueEdges);

    // Get the end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the difference
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    vol.CCL_times_.emplace_back(duration.count());
}
