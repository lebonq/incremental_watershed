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
    int nb_benchmarks = atoi(argv[4]);

    int nb_markers = atoi(argv[3]);

    std::vector<std::vector<double>> time;

    std::vector<double> init_time(nb_benchmarks);

    std::string path_volume = argv[1];
    std::string path_markers = argv[2];

    std::vector<std::string> paths_object;
    std::vector<std::string> paths_background;

    std::vector<std::vector<int>> markers_object_batched(nb_markers);
    std::vector<std::vector<int>> markers_background_batched(nb_markers);
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
    for (int j = 0; j < nb_benchmarks; j++)
    {
        std::cout << YELLOW << "Benchmark " << j + 1 << " / " << nb_benchmarks << std::endl;
        //Create the volumeManager
        auto volume_manager = new volumeManager();

        // Benchmark loadVolume
        auto start = std::chrono::high_resolution_clock::now();
        volume_manager->loadVolume(path_volume);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << MAGENTA << "loadVolume took " << diff.count() << " seconds" << std::endl;

        // Benchmark createGraph
        start = std::chrono::high_resolution_clock::now();
        volume_manager->createGraph();
        // Benchmark buildHierarchy
        volume_manager->buildHierarchy();
        end = std::chrono::high_resolution_clock::now();
        diff = end - start;
        std::cout << "Initialization took " << diff.count() << " seconds" << std::endl;

        init_time[j] = diff.count();

        time.emplace_back();

        volume_manager->threshold_ = 4000;
        std::cout << "Threshold set to " << volume_manager->threshold_ << std::endl;

        std::vector<int> markers_concat;
        for (int i = 0; i < nb_markers * 2; i++)
        {
            int cpt_obj = 0;
            int cpt_bg = 0;
            //Create a vector of markers


            for (int step = 0; step <= i; step++)
            {
                if (step % 2 == 0)
                {
                    int cpt_batch = 0;
                    for(int idx_step = step*30*100; idx_step < (step+1)*30*100; idx_step++)
                    {
                        auto value = markers_object_batched[cpt_obj][cpt_batch];
                        markers_concat.push_back(value);
                        cpt_batch++;
                    }
                    cpt_obj++;
                }
                else
                {
                    int cpt_batch = 0;
                    for(int idx_step = step*30*100; idx_step < (step+1)*30*100; idx_step++)
                    {
                        auto value = markers_background_batched[cpt_bg][cpt_batch];
                        markers_concat.push_back(value);
                        cpt_batch++;
                    }
                    cpt_bg++;
                }
            }

            //benchmark addMarkers
            start = std::chrono::high_resolution_clock::now();
            volume_manager->addMarkers(markers_concat);
            end = std::chrono::high_resolution_clock::now();
            diff = end - start;
            time[j].push_back(diff.count());

            std::cout << GREEN << "Add markers (object/bakcground) " << RESET << "     step " << i << " took " << GREEN << diff
                .count()
                << " seconds" << std::endl;

            volume_manager->resetPostprocessStructure();
            markers_concat.clear();
        }

        for (int batch = 0; batch < nb_markers; batch++)
        {
            volume_manager->dualisation_segmentation(markers_background_batched[batch], 2);
            volume_manager->dualisation_segmentation(markers_object_batched[batch], 1);
        }

        auto volume = volume_manager->getSegmentedVolume();
        for (int i = 0; i < volume.size(); i++)
        {
            cv::imwrite("test_volume/slice" + std::to_string(i) + ".png", volume[i]);
        }

        delete volume_manager;
    }

    // After the benchmarking loop, calculate average time for each step
    std::vector<double> avg_time(nb_markers*2);

    for (int i = 0; i < nb_markers*2; i++)
    {
        double sum_time = 0;

        for (int j = 0; j < nb_benchmarks; j++)
        {
            sum_time += time[j][i];
        }
        avg_time[i] = sum_time / nb_benchmarks;

    }

    //save it ina file
    algorithms::vector_to_csv(avg_time, path_markers + "/avg_time_niws.csv");
    algorithms::vector_to_csv(init_time, path_markers + "/init_time_niws.csv");
}
