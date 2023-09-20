#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include <STEPControl_Reader.hxx>
#include <IFSelect_ReturnStatus.hxx>

namespace Utils {
    void readStepFile(std::string stepFilePath);
}

#endif