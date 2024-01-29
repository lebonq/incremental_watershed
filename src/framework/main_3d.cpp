//
// Created by lebonq on 29/01/24.
//

#include "volumeManager.h"

int main(int argc, char* argv[])
{
//create  volumùe manager
    auto volume_manager = new volumeManager();
//load volume
    volume_manager->loadVolume(argv[1]);

    delete volume_manager;
}
