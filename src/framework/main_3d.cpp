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
    end = std::chrono::high_resolution_clock::now();
    diff = end-start;
    std::cout << "createGraph + sort took " << diff.count() << " seconds" << std::endl;

    // Benchmark buildHierarchy
    start = std::chrono::high_resolution_clock::now();
    volume_manager->buildHierarchy();
    end = std::chrono::high_resolution_clock::now();
    diff = end-start;
    std::cout << "buildHierarchy took " << diff.count() << " seconds" << std::endl;
}