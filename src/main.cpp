#include <iostream>

#include "Utils.hpp" // readStepFile

#include <AIS.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>



int main() {
    std::string filename = "/home/user/Workspace/aeroscan/data/LS3DC/step/64.stp"; // Filename - Cannot use the $HOME as path (???)

    Utils::readStepFile(filename);

    return 0;
}