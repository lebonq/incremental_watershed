//
// Created by lebonq on 29/01/24.
//

#include "volumeManager.h"
#include <chrono>

#include "algorithms.h"

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

std::vector<int> load_marker_from_txt(std::string& file_path)
{
    std::vector<int> markers;

    std::ifstream file(file_path);

    if (file.is_open())
    {
        std::istream_iterator<std::string> fileIterator(file);
        std::istream_iterator<std::string> endIterator;

        while (fileIterator != endIterator)
        {
            markers.push_back(std::stoi(*fileIterator));
            ++fileIterator;
        }

        file.close();
    }

    return markers;
}

int main(int argc, char* argv[])
{
    int nb_markers = atoi(argv[3]);

    std::string path_volume = argv[1];
    std::string path_markers = argv[2];

    std::vector<std::string> paths_object;
    std::vector<std::string> paths_background;

    std::vector<std::vector<int>> markers_object_batched(nb_markers);
    std::vector<std::vector<int>> markers_background_batched(nb_markers);

    std::vector<double> seq_times;
    std::vector<double> max_thread_times;
    std::vector<double> thread_sync_times;

    //generate paths
    for (int i = 0; i < nb_markers; i++)
    {
        paths_background.push_back(path_markers + "/markers_background_" + std::to_string(i) + ".txt");
        paths_object.push_back(path_markers + "/markers_object_" + std::to_string(i) + ".txt");
    }

    //load markers
    for (int i = 0; i < nb_markers; i++)
    {
        markers_object_batched.at(i) = load_marker_from_txt(paths_object.at(i));
        markers_background_batched.at(i) = load_marker_from_txt(paths_background.at(i));
    }

    //loop for benchmarking
    for (int j = 0; j <= 80; j++)
    {
        auto volume_manager = new volumeManager();

        volume_manager->loadVolume(path_volume);
        std::cout << MAGENTA << "loadVolume" << std::endl;

        // Benchmark createGraph
        volume_manager->createGraph();
        // Benchmark buildHierarchy
        volume_manager->buildHierarchy();

        std::cout << YELLOW << "Benchmark " << j+1 << " / " << 41 << std::endl;
        //Create the volumeManager

        volume_manager->threshold_ = j*100;

        //benchmark addMarkers object
        volume_manager->addMarkers(markers_object_batched[0]);
        std::cout << GREEN << "With threshold of " << volume_manager->threshold_ << RESET << std::endl;

        seq_times.emplace_back(volume_manager->time_seq_[0]);
        max_thread_times.emplace_back(volume_manager->time_total_[0]);
        thread_sync_times.emplace_back(volume_manager->time_thread_sync_[0]);

        delete volume_manager;
    }

    algorithms::vector_to_csv(seq_times, path_markers + "/seq_times.csv");
    algorithms::vector_to_csv(max_thread_times, path_markers + "/total_times.csv");
    algorithms::vector_to_csv(thread_sync_times, path_markers + "/thread_sync_times.csv");

}
