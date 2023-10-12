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

#include <STEPCAFControl_Reader.hxx>
#include <TDocStd_Document.hxx>
#include <fstream>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <TDataStd_Name.hxx>

#include <STEPCAFControl_Controller.hxx>
#include <STEPControl_Controller.hxx>
#include <Message.hxx>
#include <TDocStd_Application.hxx>
#include <BinXCAFDrivers.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_LabelSequence.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_ChildIterator.hxx>
#include <XCAFPrs_AISObject.hxx>

namespace Utils 
{
    TopTools_ListOfShape readStepFile(std::string stepFilePath);
    Handle_TDocStd_Document readStepCafFile(std::string stepFilePath);
    TopTools_ListOfShape splitShapeIntoShells(const TopoDS_Shape& shape);
}   

#endif