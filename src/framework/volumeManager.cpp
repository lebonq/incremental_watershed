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

    SI::natural::sort(filenames);

    for (const auto& filename : filenames)
    {
        DicomImage* image = this->loadDicomFile(filename);
        if (image != nullptr)
        {
            volume_.push_back(image);
        }
        //std::cerr << "Image " << filename << " loaded" << std::endl;
    }

    this->width_ = volume_[0]->getWidth();
    this->height_ = volume_[0]->getHeight();
    this->depth_ = volume_.size();

    this->hierarchy_ = std::make_unique<QEBT>(this->width_ * this->height_ * this->depth_);
}

void volumeManager::createGraph()
{
    this->graph_ = std::make_unique<graph>(this->width_ * this->height_ * this->depth_, 256,
                         this->width_ * this->height_ * this->depth_ * 3);
    int index = 0;

    for (int z = 0; z < this->depth_; z++)
    {
        auto image = (Uint16*)(this->volume_[z]->getOutputData(16));
        Uint16* image_next;
        if (z + 1 < this->depth_)
        {
            image_next = (Uint16*)(this->volume_[z + 1]->getOutputData(16));
        }

        for (int y = 0; y < this->height_; y++)
        {
            for (int x = 0; x < this->width_; x++)
            {
                //Conver to 8bits and normalize
                Uint8 pixel = (image[x + y * this->width_] - 31744) / 8;

                if (pixel > 255)
                {
                    std::cerr << "Pixel out of 8bits range  value is : " << pixel << std::endl;
                }

                if (x + 1 < this->width_)
                {
                    // Element right
                    Uint8 x_value = (image[(x + 1) + y * this->width_] - 31744) / 8;
                    this->graph_->setWeight(index, std::abs(pixel - x_value));
                }
                else
                {
                    //For the ghost edges
                    this->graph_->setWeight(index, -1);
                }
                index++; //We go the index of the next edge
                if (y + 1 < this->height_)
                {
                    //Element below
                    Uint8 y_value = (image[x + (y + 1) * this->width_] - 31744) / 8;
                    this->graph_->setWeight(index, std::abs(pixel - y_value));
                }
                else
                {
                    this->graph_->setWeight(index, -1);
                }
                index++; //We go to the next edge
                if (z + 1 < this->depth_)
                {
                    //Element in next slice
                    Uint8 pixel_z_next = (image_next[x + y * this->width_] - 31744) / 8;
                    this->graph_->setWeight(index, std::abs(pixel - pixel_z_next));
                }
                else
                {
                    this->graph_->setWeight(index, -1);
                }
                index++;
            }
        }
    }
    std::cerr << "Graph created" << std::endl;
    std::cerr << "Graph has " << this->graph_->getNbVertex() << " vertex" << std::endl;
    std::cerr << "Graph has " << this->graph_->getNbEdge() << " edges" << std::endl;
    std::cerr << "Graph should have " << this->graph_->getNbVertex() - 1<< " edges in the MST" << std::endl;
    this->graph_->init_sortedEdges();
    std::cerr << "Graph sorted" << std::endl;

    this->map_graph_mst_ = std::make_unique<std::vector<int>>(this->graph_->getNbEdge());
}

void volumeManager::buildHierarchy()
{
    algorithms::kruskal3D(*this->graph_, *this->hierarchy_, this->width_, this->height_, this->map_graph_mst_->data());
}

volumeManager::~volumeManager()
{
    for (auto image : this->volume_)
    {
        delete image;
    }
}
