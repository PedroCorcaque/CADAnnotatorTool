#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

#include <STEPControl_Reader.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS.hxx>

namespace Utils 
{
    TopTools_ListOfShape readStepFile(std::string stepFilePath);
    TopTools_ListOfShape splitShapeIntoShells(const TopoDS_Shape& shape);
}   

#endif