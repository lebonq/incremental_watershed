import os
import pandas as pd
import matplotlib.pyplot as plt

def scan_and_plot(base_dir):
    # Dictionary to hold data for each thread configuration
    benchmark_data = {}

    # Walk through the base directory
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if file.endswith(".csv"):
                # Extract the number of threads and the slice from the directory structure
                path_parts = root.split(os.sep)
                nb_threads = int(path_parts[-2].split('_')[-1])
                thread_id = int(path_parts[-1].split('_')[-1])

                # Read the CSV file
                file_path = os.path.join(root, file)
                data = pd.read_csv(file_path)

                # Ensure the data is sorted by the step (optional, but good practice)
                data = data.sort_values(by="Step")

                # Add to the dictionary
                if nb_threads not in benchmark_data:
                    benchmark_data[nb_threads] = {}
                benchmark_data[nb_threads][thread_id] = data

    # Plot the data for each thread configuration
    for nb_threads, threads_data in benchmark_data.items():
        plt.figure(figsize=(10, 6))

        for thread_id, data in threads_data.items():
            #plt.plot(data['FrontSize'], data['Time(ns)'], label=f'Thread {thread_id}')
            plt.scatter(data['FrontSize'], data['Time(ns)'], label=f'Thread {thread_id}')

        plt.xlabel('Pixels Explored')
        plt.ylabel('Time (ns)')
        plt.title(f'Benchmark with {nb_threads} Threads')
        plt.legend()
        plt.grid(True)

        # Save the plot
        plot_filename = os.path.join(base_dir, f'benchmark_{nb_threads}_threads.png')
        plt.savefig(plot_filename)
        plt.close()

    # Plot summary graph comparing total time across different thread configurations
    plt.figure(figsize=(10, 6))
    total_times = []

    for nb_threads, threads_data in benchmark_data.items():
        total_time = sum(data['Time(ns)'].sum() for data in threads_data.values())
        total_times.append((nb_threads, total_time))

    # Sort by the number of threads
    total_times.sort()

    nb_threads_list, total_time_list = zip(*total_times)
    #plt.plot(nb_threads_list, total_time_list, marker='o')
    plt.scatter(nb_threads_list, total_time_list, alpha=0.5)
    plt.xlabel('Number of Threads')
    plt.ylabel('Total Time (ns)')
    plt.title('Total Computation Time vs Number of Threads')
    plt.grid(True)

    # Save the summary plot
    summary_plot_filename = os.path.join(base_dir, 'summary_total_time_vs_threads.png')
    plt.savefig(summary_plot_filename)
    plt.close()


if __name__ == "__main__":
    base_directory = "/home/lebonq/CLionProjects/incremental_watershed/data_exec_manual"
    scan_and_plot(base_directory)
