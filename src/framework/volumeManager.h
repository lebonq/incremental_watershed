//
// Created by lebonq on 29/01/24.
//

#ifndef VOLUMEMANAGER_H
#define VOLUMEMANAGER_H

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include <vector>
#include <filesystem>

class volumeManager {
private:
    std::vector<DicomImage*> volume_;

public:
    DicomImage* loadDicomFile(const std::string& filename);
    void loadVolume(const std::string& folder_name);
    ~volumeManager();
};


#endif //VOLUMEMANAGER_H
