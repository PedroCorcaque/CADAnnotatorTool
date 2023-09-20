#include <iostream>

#include "Utils.hpp" // readStepFile

#include <AIS.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

int main() {
    std::string filename = "/home/user/Workspace/aeroscan/data/LS3DC/step/64.stp"; // Filename - Cannot use the $HOME as path (???)
    std::vector<TopoDS_Shape> shapes;

    shapes = Utils::readStepFile(filename);
    if (shapes.size() == 0)
    {
        std::cerr << "There is no shape to process." << std::endl;
        return 1;
    }
    
    std::cout << "All ok" << std::endl;
    return 0;
}