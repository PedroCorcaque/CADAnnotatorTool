#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

#include <STEPControl_Reader.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <TopoDS_Shape.hxx>

namespace Utils 
{
    std::vector<TopoDS_Shape> readStepFile(std::string stepFilePath);
}

#endif