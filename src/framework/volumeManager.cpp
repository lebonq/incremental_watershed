//
// Created by lebonq on 29/01/24.
//

#include "volumeManager.h"


DicomImage* volumeManager::loadDicomFile(const std::string& filename)
{
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(filename.c_str());

    if (status.bad())
    {
        std::cerr << "Error: cannot read DICOM file (" << status.text() << ")" << std::endl;
        return nullptr;
    }

    auto* image = new DicomImage(&fileformat, EXS_Unknown, CIF_MayDetachPixelData);
    auto status_image = image->getStatus();
    if (status_image != EIS_Normal)
    {
        std::cerr << "Error: cannot load DICOM image (" << DicomImage::getString(image->getStatus()) << ")" <<
            std::endl;
        delete image;
        image = nullptr;
        exit(-1);
    }

    return image;
}

void volumeManager::loadVolume(const std::string& folder_name)
{
    std::vector<std::string> filenames;
    for (const auto& entry : std::filesystem::directory_iterator(folder_name))
    {
        filenames.push_back(entry.path().string());
    }

    for (const auto& filename : filenames)
    {
        DicomImage* image = this->loadDicomFile(filename);
        if (image != nullptr)
        {
            volume_.push_back(image);
        }
    }

}

volumeManager::~volumeManager()
{
    for (auto image : this->volume_)
    {
        delete image;
    }
}
