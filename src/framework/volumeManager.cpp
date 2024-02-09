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

void volumeManager::createToyGraph()
{

    this->width_ = 3;
    this->height_ = 2;
    this->depth_ = 2;

    this->hierarchy_ = std::make_unique<QEBT>(this->width_ * this->height_ * this->depth_);

    this->graph_ = std::make_unique<graph>(this->width_ * this->height_ * this->depth_, 20,
                     this->width_ * this->height_ * this->depth_ * 3);

//0
    this->graph_->setWeight(0, 10);
    this->graph_->setWeight(1, 16);
    this->graph_->setWeight(2, 8);
//1
    this->graph_->setWeight(3, 18);
    this->graph_->setWeight(4, 11);
    this->graph_->setWeight(5, 20);
//2
    this->graph_->setWeight(6, -1);
    this->graph_->setWeight(7, 14);
    this->graph_->setWeight(8, 13);
//3
    this->graph_->setWeight(9, 5);
    this->graph_->setWeight(10, -1);
    this->graph_->setWeight(11, 1);
//4
    this->graph_->setWeight(12, 4);
    this->graph_->setWeight(13, -1);
    this->graph_->setWeight(14, 2);
//5
    this->graph_->setWeight(15, -1);
    this->graph_->setWeight(16, -1);
    this->graph_->setWeight(17, 3);
//6
    this->graph_->setWeight(18, 9);
    this->graph_->setWeight(19, 7);
    this->graph_->setWeight(20, -1);
//7
    this->graph_->setWeight(21, 12);
    this->graph_->setWeight(22, 15);
    this->graph_->setWeight(23, -1);
//8
    this->graph_->setWeight(24, -1);
    this->graph_->setWeight(25, 19);
    this->graph_->setWeight(26, -1);
//9
    this->graph_->setWeight(27, 6);
    this->graph_->setWeight(28, -1);
    this->graph_->setWeight(29, -1);
//10
    this->graph_->setWeight(30, 17);
    this->graph_->setWeight(31, -1);
    this->graph_->setWeight(32, -1);
//11
    this->graph_->setWeight(33, -1);
    this->graph_->setWeight(34, -1);
    this->graph_->setWeight(35, -1);

    std::cerr << "Graph created" << std::endl;
    std::cerr << "Graph has " << this->graph_->getNbVertex() << " vertex" << std::endl;
    std::cerr << "Graph has " << this->graph_->getNbEdge() << " edges" << std::endl;
    std::cerr << "Graph should have " << this->graph_->getNbVertex() - 1<< " edges in the MST" << std::endl;
    this->graph_->init_sortedEdges();
    std::cerr << "Graph sorted" << std::endl;

    this->map_graph_mst_ = std::make_unique<std::vector<int>>(this->graph_->getNbEdge(),-1);
}

void volumeManager::buildHierarchy()
{
    std::cout << (*this->map_graph_mst_)[1] << std::endl;
    algorithms3D::kruskal(*this->graph_, *this->hierarchy_, this->width_, this->height_, this->map_graph_mst_->data());
    std::cout << (*this->map_graph_mst_)[1] << std::endl;
    this->initPostprocessStructure();
}

void volumeManager::initPostprocessStructure()
{
    this->segments_ = std::make_unique<std::vector<int>>(this->graph_->getNbVertex(),0);
    this->marks_ = std::make_unique<std::vector<int>>(this->hierarchy_->getQBT().getSize(),0);
    std::cout << "Marks is of size : " << this->marks_->size() << std::endl;
    this->sizePart_ = std::make_unique<std::vector<int>>(this->graph_->getNbVertex(),0);
    this->sizePart_->at(0) = this->graph_->getNbVertex();
    this->ws_ = std::make_unique<std::vector<bool>>(this->hierarchy_->getQBT().getSize(),false);
    this->mstEdit_ = std::make_unique<std::vector<bool>>(this->graph_->getMst().size(), true);

}
int volumeManager::getEdge(int n)
{
    if (n < this->graph_->getNbVertex()) {
        std::cout << "Error : n is not a valid node" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return n - this->graph_->getNbVertex();
};

cv::Mat volumeManager::getSegmentedSlice(int z)
{
    cv::Mat img(this->getHeight(), this->getWidth(),
                    CV_8UC3, cv::Vec3b(255, 0, 0));

    img.at<cv::Vec3b>(0, 0) = cv::Vec3b(0, 0, 0);

    int cpt = 0+(z*this->getWidth()*this->getHeight()), seed;
    for (int y = 0; y < this->getHeight(); y++) {
        for (int x = 0; x < this->getWidth(); x++) {
            seed = this->segments_->at(cpt);
            srand(seed);
            img.at<cv::Vec3b>(y, x) = cv::Vec3b(rand() % 255, rand() % 255, rand() % 255);
            cpt++;
        }
    }
    std::cout << "Segmented slice " << z << " rendered" << std::endl;
    return img;
}


std::vector<cv::Mat> volumeManager::getSegmentedVolume()
{
    std::vector<cv::Mat> volume(this->getDepth());


    for( int z = 0; z < this->getDepth(); z++)
    {
        volume[z] = this->getSegmentedSlice(z);
    }

    return volume;
}

void volumeManager::addMarkers(std::vector<int>& markers, int nbMarkers)
{
    algorithms3D::addMarker(std::ref(*this), markers, nbMarkers);
}

void volumeManager::removeMarkers(std::vector<int>& markers, int nbMarkers)
{
    algorithms3D::removeMarker(std::ref(*this), markers, nbMarkers);
}


volumeManager::~volumeManager()
{
    for (auto image : this->volume_)
    {
        delete image;
    }
}
