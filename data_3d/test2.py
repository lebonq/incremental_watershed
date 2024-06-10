import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
file_path = '/home/lebonq/CLionProjects/incremental_watershed/data_3d/patient_2/patient_2_liver_markers/size_front_0_patient2.csv'
df = pd.read_csv(file_path)

#number of values
print(len(df))

# Count the occurrences of each value


# Plot the histogram
plt.figure(figsize=(10, 6))
plt.plot(range(0, len(df), 1), df[0], label='Value')
plt.xlabel('Values')
plt.ylabel('Frequency')
plt.title('Histogram of Value Occurrences')

# Show the plot
plt.show()
plt.savefig('histogram.png')
