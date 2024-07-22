# incremental_watershed
Implementation of incremental watershed algorithm in C++.

## Compilation

To compile you will need openCV availibale on your systeme.

## Usage on 3D

The file bench_iws.cpp is a good example of how you can use this repo.

First you need to build a volume manager and set 2 parameter :
- thrashold_ which is MIN_BREADTH in the paper
- And the number of threads 
```
  auto volume_manager = new volumeManager();
  volume_manager->threshold_ = threshold_param;
  volume_manager->nb_threads_ = num_threads;
```

Then load the volume in memory (check in data_3D folder to see the format used). Then create the graph and then the hierarchie.
```
  volume_manager->loadVolume(path_volume);
  volume_manager->createGraph();
  volume_manager->buildHierarchy();
```

To add or remove marker you need to give the program a array where there is the marker's 1D coordinates 

  ```
  volume_manager->addMarkers(markers_object_batched[i]);
  volume_manager->removeMarkers(markers_object_batched[i]);
```

To reset the hierarchy and the MST use : ```volume_manager->resetPostprocessStructure();```

## Extension

This repo is an extension of the original to be submitted to [PRL](https://www.journals.elsevier.com/pattern-recognition-letters) journal.

