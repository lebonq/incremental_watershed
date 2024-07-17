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
    if (argc != 8)
    {
        std::cout << "Usage: " << argv[0] <<
            " <path_to_volume> <path_to_markers> <nb_markers> <nb_benchmarks> <name_patient> <threshold> <nb_threads>"
            << std::endl;
        return 1;
    }

    int num_threads = atoi(argv[7]);
    int threshold_param = atoi(argv[6]);
    int nb_benchmarks = atoi(argv[4]);

    std::string namepatient = argv[5];

    int nb_markers = atoi(argv[3]);

    std::vector<std::vector<double>> object_time(nb_benchmarks, std::vector<double>(nb_markers));
    std::vector<std::vector<double>> background_time(nb_benchmarks, std::vector<double>(nb_markers));
    std::vector<std::vector<double>> time_all(nb_benchmarks, std::vector<double>(nb_markers * 2));
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

    std::vector<std::vector<long>> CCL_times(nb_benchmarks);
    std::vector<std::vector<long>> par_times(nb_benchmarks);
    std::vector<std::vector<long>> seq_times(nb_benchmarks);
    std::vector<std::vector<std::vector<long>>> real_thread_times(nb_benchmarks);

    //loop for benchmarking
    for (int j = 0; j < nb_benchmarks; j++)
    {
        std::cout << YELLOW << "Benchmark " << j + 1 << " / " << nb_benchmarks << std::endl;
        //Create the volumeManager
        auto volume_manager = new volumeManager();

        volume_manager->threshold_ = threshold_param;
        std::cout << "Threshold set to " << volume_manager->threshold_ << std::endl;
        volume_manager->nb_threads_ = num_threads;
        std::cout << "Number of threads set to " << volume_manager->nb_threads_ << std::endl;

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


        int cpt = 0;
        for (int i = 0; i < nb_markers; i++)
        {
            //benchmark addMarkers object
            start = std::chrono::high_resolution_clock::now();
            volume_manager->addMarkers(markers_object_batched[i]);
            end = std::chrono::high_resolution_clock::now();
            diff = end - start;
            object_time[j][i] = diff.count(); // Store time taken
            time_all[j][cpt] = diff.count();
            cpt++;
            std::cout << GREEN << "addMarkers object" << RESET << "     step " << i << " took " << GREEN << diff.count()
                << " seconds" << std::endl;
            std::cout << GREEN << "labeling took " << volume_manager->CCL_times_.at(i * 2) / 1e9 << " seconds" <<
                std::endl;

            //bencmark addMarkers background
            start = std::chrono::high_resolution_clock::now();
            volume_manager->addMarkers(markers_background_batched[i]);
            end = std::chrono::high_resolution_clock::now();
            diff = end - start;
            background_time[j][i] = diff.count(); // Store time taken
            time_all[j][cpt] = diff.count();
            cpt++;
            std::cout << RED << "addMarkers background" << RESET << " step " << i << " took " << RED << diff.count() <<
                " seconds" << std::endl;
            std::cout << RED << "labeling took " << volume_manager->CCL_times_.at(i * 2 + 1) / 1e9 << " seconds"
                << std::endl;
        }
        for (int batch = 0; batch < nb_markers; batch++)
        {
            volume_manager->dualisation_segmentation(markers_background_batched[batch], 2);
            volume_manager->dualisation_segmentation(markers_object_batched[batch], 1);
        }

        /*auto volume = volume_manager->getSegmentedVolume();
        for (int i = 0; i < volume.size(); i++)
        {
            cv::imwrite("test_volume/slice" + std::to_string(i) + ".png", volume[i]);
        }*/


        //volume_manager->write_CCL_times(path_markers, j, namepatient);
        CCL_times[j] = volume_manager->get_CCL_times();
        par_times[j] = volume_manager->time_par_;
        seq_times[j] = volume_manager->time_seq_;
        real_thread_times[j] = volume_manager->time_real_thread_;
        //volume_manager->write_real_thread_times(path_markers, j, namepatient);
        //volume_manager->write_par_times(path_markers, j, namepatient);
        //volume_manager->write_seq_times(path_markers, j, namepatient);
        //volume_manager->write_size_front(path_markers, j, namepatient);
        //volume_manager->write_par_times(path_markers, j);
        delete volume_manager;
    }

    // After the benchmarking loop, calculate average time for each step
    std::vector<double> avg_object_time(nb_markers);
    std::vector<double> avg_background_time(nb_markers);
    std::vector<long> avg_ccl_times(nb_markers * 2);


    for (int i = 0; i < nb_markers; i++)
    {
        double sum_object_time = 0;
        double sum_background_time = 0;

        for (int j = 0; j < nb_benchmarks; j++)
        {
            sum_object_time += object_time[j][i];
            sum_background_time += background_time[j][i];
        }

        avg_object_time[i] = sum_object_time / nb_benchmarks;
        avg_background_time[i] = sum_background_time / nb_benchmarks;
    }

    for (int i = 0; i < nb_markers * 2; i++)
    {
        long sum_ccl_time = 0;

        for (int j = 0; j < nb_benchmarks; j++)
        {
            sum_ccl_time += CCL_times[j][i];
        }

        avg_ccl_times[i] = sum_ccl_time / nb_benchmarks;
    }



    // Print average time for each step
    for (int i = 0; i < nb_markers; i++)
    {
        std::cout << "Average time for object step " << i << ": " << avg_object_time[i] << " seconds" << std::endl;
        std::cout << "Average time for background step " << i << ": " << avg_background_time[i] << " seconds" <<
            std::endl;
    }

     //save it ina file patient_sizefrint_nb_threads
     // algorithms::vector_to_csv(avg_object_time,
     //                           path_markers + "/avg_object_time_iws_" + namepatient + "_" + argv[6] + "_" + argv[7] +
     //                           ".csv");
     // algorithms::vector_to_csv(avg_background_time,
     //                           path_markers + "/avg_background_time_iws_" + namepatient + "_" + argv[6] + "_" + argv[7] +
     //                           ".csv");
     //algorithms::vector_to_csv(avg_ccl_times,                               path_markers + "/avg_ccl_times_iws_" + namepatient +                              ".csv");
     // algorithms::vector_to_csv(init_time, path_markers + "/init_time_iws_" + namepatient  +".csv");

    algorithms::vector_to_csv(avg_object_time, path_markers + "/avg_object_time_iws_" + namepatient + "_par.csv");
    algorithms::vector_to_csv(avg_background_time, path_markers + "/avg_background_time_iws_" + namepatient + "_par.csv");
    algorithms::vector_to_csv(init_time, path_markers + "/init_time_iws_" + namepatient  +"_par.csv");
    algorithms::vector_to_csv(avg_ccl_times, path_markers + "/avg_ccl_times_iws_" + namepatient + "_par.csv");

    //    for (int i = 0; i < nb_benchmarks; i++)
    //    {
    //        for (int thread = 0; thread < num_threads; thread++)
    //        {
    //            algorithms::vector_to_csv(real_thread_times[i][thread],
    //                                      path_markers + "/par_detailled/real_thread_times_iws_" + namepatient + "_" + argv[
    //                                          6] + "_" + argv[7] + "_bench_" + std::to_string(i) + "_thread_" +
    //                                      std::to_string(thread) + ".csv");
    //        }
    //        algorithms::vector_to_csv(par_times[i],
    //                                  path_markers + "/par_detailled/par_times_iws_" + namepatient + "_" + argv[6] + "_" +
    //                                  argv[7] + "_bench_" + std::to_string(i) + ".csv");
    //        algorithms::vector_to_csv(seq_times[i],
    //                                  path_markers + "/par_detailled/seq_times_iws_" + namepatient + "_" + argv[6] + "_" +
    //                                  argv[7] + "_bench_" + std::to_string(i) + ".csv");
    //        algorithms::vector_to_csv(time_all[i],
    //                                  path_markers + "/par_detailled/all_time_iws_" + namepatient + "_" + argv[6] +
    //                                  "_" + argv[7] + "_bench_" + std::to_string(i) + ".csv");
    //    }
}
