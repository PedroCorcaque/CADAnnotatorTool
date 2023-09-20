#include "Utils.hpp"

void Utils::readStepFile(std::string stepFilePath)
{
    STEPControl_Reader stepControlReader;
    IFSelect_ReturnStatus ifSelectReturnStatusEnum;

    ifSelectReturnStatusEnum = stepControlReader.ReadFile(stepFilePath.c_str());

    switch (ifSelectReturnStatusEnum)
    {
    case 0: // IFSelect_RetVoid
        std::cout << "normal execution which created nothing, or no data to process" << std::endl;
        break;
    
    case 1: // IFSelect_RetDone
        std::cout << "normal execution with a result" << std::endl;
        break;

    case 2: // IFSelect_RetError
        std::cout << "error in command or input data, no execution" << std::endl;
        break;

    case 3: // IFSelect_RetFail
        std::cout << "execution was run and has failed" << std::endl;
        break;

    case 4: // IFSelect_RetStop
        std::cout << "indicates end or stop (such as Raise)" << std::endl;
        break;
    
    default:
        break;
    }
}