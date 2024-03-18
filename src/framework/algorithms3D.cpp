//
// Created by lebonq on 31/01/24.
//

#include "algorithms3D.h"

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
    int v_1D, vRight_1D, vLeft_1D, vUp_1D, vDown_1D, vFoward_1D, vBackward_1D;
    std::tuple<int, int, int> v_3D;
    while (!queue.empty())
    {
        v_1D = queue.back();
        v_3D = from1Dto3D(v_1D, w, h, d);
        queue.pop_back();
        vol.getSegments()[v_1D] = tag;

        vRight_1D = v_1D + 1;
        vLeft_1D = v_1D - 1;
        vUp_1D = v_1D - w;
        vDown_1D = v_1D + w;
        vFoward_1D = v_1D + wh;
        vBackward_1D = v_1D - wh;


        //Right
        if (std::get<0>(v_3D) + 1 < w)
        {
            //check if adjacent to v exist
            if (vol.isInMStEdit(vol.getMapGraphMst()[3 * v_1D]) == true &&
                vol.getSegments()[vRight_1D] != tag)
            {
                //If yes we check if the edge is revealnt and present in MST
                queue.push_back(vRight_1D);
                vol.modifySegments(vRight_1D, tag);
                count++;
            }
        }

        //left
        if (std::get<0>(v_3D)-1 >= 0)
        {
            if (vol.isInMStEdit(vol.getMapGraphMst()[3 * (v_1D - 1)]) == true && vol.getSegments()[vLeft_1D] != tag)
            {
                queue.push_back(vLeft_1D);
                vol.modifySegments(vLeft_1D, tag);
                count++;
            }
        }

        //Down
        if (std::get<1>(v_3D) + 1 < h)
        {
            if (vol.isInMStEdit(vol.getMapGraphMst()[3 * v_1D + 1]) == true && vol.getSegments()[vDown_1D] != tag)
            {
                queue.push_back(vDown_1D);
                vol.modifySegments(vDown_1D, tag);
                count++;
            }
        }

        //Up
        if (std::get<1>(v_3D) - 1 >= 0)
        {
            if (vol.isInMStEdit(vol.getMapGraphMst()[(v_1D - w) * 3 + 1]) == true && vol.getSegments()[vUp_1D] != tag)
            {
                queue.push_back(vUp_1D);
                vol.modifySegments(vUp_1D, tag);
                count++;
            }
        }


        //Foward
        if (std::get<2>(v_3D) + 1 < d)
        {
            if (vol.isInMStEdit(vol.getMapGraphMst()[3 * v_1D + 2]) == true && vol.getSegments()[vFoward_1D] != tag)
            {
                queue.push_back(vFoward_1D);
                vol.modifySegments(vFoward_1D, tag);
                count++;
            }
        }

        //backward
        if (std::get<2>(v_3D) - 1 >= 0)
        {

            if (vol.isInMStEdit(vol.getMapGraphMst()[(v_1D - w * h) * 3 + 2]) == true && vol.getSegments()[
                vBackward_1D] != tag)
            {
                queue.push_back(vBackward_1D);
                vol.modifySegments(vBackward_1D, tag);
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


        tag1 = vol.getSegments()[p1];
        tag2 = vol.getSegments()[p2];

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

/**
 * Merge 2 segments
 * @param p1 Point on the first segment
 * @param p2 Point on the second segment
 * @param vol
 */
void algorithms3D::mergeSegment(int edge, volumeManager& vol)
{
    std::vector<int> seg = vol.getSegments();
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
    auto mstEdit = vol.getMstEdit();

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
                vol.modifyMstEdit(vol.getEdge(up),false);
                queueEdges.push_back(vol.getGraph().getMst()[vol.getEdge(up)]);
                break;
            }
            up = parent[up];
        }
    }


    // Get the start time
    auto start = std::chrono::high_resolution_clock::now();

    // Run the code you want to benchmark
    splitSegment(vol, historyVisited, queueEdges);

    // Get the end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the difference
    auto duration = end - start;

    vol.add_CCL_time(duration.count());
}
