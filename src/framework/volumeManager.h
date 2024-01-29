//
// Created by lebonq on 29/01/24.
//

#ifndef VOLUMEMANAGER_H
#define VOLUMEMANAGER_H

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include <vector>


class volumeManager {
private:
    std::vector<DicomImage*> volume_;

public:
    DicomImage* loadDicomFile(const std::string& filename);
    void loadVolume(const std::vector<std::string>& filenames);
    ~volumeManager();
};


#endif //VOLUMEMANAGER_H
