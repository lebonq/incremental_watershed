//
// Created by lebonq on 29/01/24.
//

#include "volumeManager.h"
#include <chrono>

int main(int argc, char* argv[])
{
    auto volume_manager = new volumeManager();

    // Benchmark loadVolume
    auto start = std::chrono::high_resolution_clock::now();
    volume_manager->loadVolume(argv[1]);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end-start;
    std::cout << "loadVolume took " << diff.count() << " seconds" << std::endl;

    // Benchmark createGraph
    start = std::chrono::high_resolution_clock::now();
    volume_manager->createGraph();
    //volume_manager->createToyGraph();
    end = std::chrono::high_resolution_clock::now();
    diff = end-start;
    std::cout << "createGraph + sort took " << diff.count() << " seconds" << std::endl;

    // Benchmark buildHierarchy
    start = std::chrono::high_resolution_clock::now();
    volume_manager->buildHierarchy();
    end = std::chrono::high_resolution_clock::now();
    diff = end-start;
    std::cout << "buildHierarchy took " << diff.count() << " seconds" << std::endl;


    auto mst = volume_manager->getGraph().getMst();
    auto mst_map = volume_manager->getMapGraphMst();
    auto edges = volume_manager->getGraph().getEdges();

    /*int cpt = 0;
    for( auto edge : mst)
    {

        std::cout <<  "Edge weight " << edges[edge] << std::endl;
    }

    std::vector<int> markers = {4,6,7};
    volume_manager->addMarkers(markers,markers.size());
     auto segments = volume_manager->getSegments();

    for(int i = 0; i < segments.size(); i++)
    {
        std::cout << "Segment " << i << " : " << segments[i] << std::endl;
    }*/

    std::srand(std::time(nullptr)); // use current time as seed for random generator
    std::vector<int> randomNumbers;
    int upperLimit = 512*512*260;


    randomNumbers.push_back(0);
    for(int i = 0; i < 100000; i++) {
        randomNumbers.push_back(std::rand() % upperLimit); // generate random number between 0 and upperLimit
    }

    std::cout << "CAdd" << std::endl;
    volume_manager->addMarkers(randomNumbers, randomNumbers.size());

    //write the volume in 260 slices in files
    std::vector<cv::Mat> volume = volume_manager->getSegmentedVolume();
    for (int i = 0; i < volume.size(); i++)
    {
        std::string filename = "test_volume/slice" + std::to_string(i) + ".png";
        cv::imwrite(filename, volume[i]);
    }

    delete volume_manager;
}