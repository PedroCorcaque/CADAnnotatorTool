/**
 * https://github.com/gkv311/occt-hello/blob/master/occt-xcaf-shape/OcctXCafShape.cpp
*/

#pragma once

#include <AIS_ViewController.hxx>
#include <TCollection_AsciiString.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <OSD.hxx>
#include <OSD_Environment.hxx>
#include <STEPControl_Controller.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <XCAFPrs_AISObject.hxx>
#include <BinXCAFDrivers.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Tool.hxx>
#include <TopLoc_Location.hxx>
#include <XCAFPrs.hxx>
#include <XCAFPrs_Style.hxx>
#include <XCAFPrs_IndexedDataMapOfShapeStyle.hxx>

// Display imports
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>

// Viewer imports
#include <V3d_Viewer.hxx>

// Window imports
#include <Xw_Window.hxx>
#include <X11/Xlib.h>

class Viewer : public AIS_ViewController
{
    private:
        bool createXCafApp();
        void newDocument();
        static TCollection_AsciiString getXCafNodePathNames(const XCAFPrs_DocumentExplorer& theExp, const bool theIsInstanceName, const int theLowerDepth = 0);

        virtual void OnSelectionChanged(const Handle(AIS_InteractiveContext)& theCtx, const Handle(V3d_View)& theView) override;
        virtual void ProcessExpose() override;
        virtual void ProcessConfigure(bool theIsResized) override; 
        virtual void ProcessInput() override;

        Handle(AIS_InteractiveContext) myContext;
        Handle(V3d_View) myView;
        Handle(TDocStd_Application) myXdeApp;
        Handle(TDocStd_Document) myXdeDoc;

    public:
        Viewer();

        const Handle(AIS_InteractiveContext)& Context() const;
        const Handle(V3d_View)& View() const;

        bool SaveXBF(const TCollection_AsciiString& theFilePath);
        bool OpenXBF(const TCollection_AsciiString& theFilePath);
        bool OpenSTEP(const TCollection_AsciiString& theFilePath);
        void DumpXCafDocumentTree();
        void DisplayXCafDocument(bool theToExplode);
};