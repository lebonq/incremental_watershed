//
// Created by lebonq on 29/01/24.
//

#include "volumeManager.h"
#include <chrono>

std::vector<int> load_marker_from_txt(std::vector<std::string>& files_path)
{
    std::vector<int> markers;
    for (auto file_path : files_path)
    {
        std::ifstream file(file_path);

        if (file.is_open()) {
            std::istream_iterator<std::string> fileIterator(file);
            std::istream_iterator<std::string> endIterator;

            while (fileIterator != endIterator) {
                markers.push_back(std::stoi(*fileIterator));
                ++fileIterator;
            }

            file.close();
        }
    }
    return markers;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> paths;
    //do it in aloop
    for (int i = 0; i < 29; i++)
    {
        paths.push_back("data_3d/markers_background_" + std::to_string(i) + ".txt");
        paths.push_back("data_3d/markers_object_" + std::to_string(i) + ".txt");
    }
    auto markers = load_marker_from_txt(paths);

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

    volume_manager->addMarkers(markers, markers.size());

    //write the volume in 260 slices in files
    std::vector<cv::Mat> volume = volume_manager->getSegmentedVolume();
    for (int i = 0; i < volume.size(); i++)
    {
        std::string filename = "test_volume/slice" + std::to_string(i) + ".png";
        cv::imwrite(filename, volume[i]);
    }

    delete volume_manager;
}