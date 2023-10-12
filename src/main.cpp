#include <iostream>

#include "Utils.hpp" // readStepFile

#include <AIS.hxx>
#include <AIS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Aspect_Window.hxx>

#include <fstream>
#include <Standard_OStream.hxx>

#include <Xw_Window.hxx>
#include <X11/Xlib.h>

unsigned int WIDTH = 1280;
unsigned int HEIGHT = 768;

int main() {
    std::string filename = "/home/user/Workspace/aeroscan/data/LS3DC/step/64.stp"; // Filename - Cannot use the $HOME as path (???)
    Handle_TDocStd_Document anXdeDoc;

    anXdeDoc = Utils::readStepCafFile(filename);

    Handle(Aspect_DisplayConnection) aDisplay = new Aspect_DisplayConnection();
    Handle(Aspect_Window) aWindow = new Xw_Window (aDisplay, "CADAnnotator", 0, 0, WIDTH, HEIGHT);
    aWindow->Map();
    while (!aWindow->IsMapped()) {}

    Handle(OpenGl_GraphicDriver) aGlDriver = new OpenGl_GraphicDriver(aDisplay);
    aGlDriver->ChangeOptions().swapInterval = 1;
    Handle(Graphic3d_GraphicDriver) aDriver = aGlDriver;
    Handle(V3d_Viewer) aViewer = new V3d_Viewer (aDriver);
    aViewer->SetDefaultLights();
    aViewer->SetLightOn();
    Handle(V3d_View) theView  = new V3d_View (aViewer);
    theView->SetImmediateUpdate(false);
    theView->SetShadingModel(Graphic3d_TOSM_FRAGMENT);
    
    theView->SetWindow(aWindow);    // Causing a segmentation fault if the Window is a null pointer
    theView->SetBackgroundColor (Quantity_NOC_GRAY50);
    theView->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
    theView->TriedronDisplay();
    theView->Zoom(200, 200, 10, 10);

    Handle(AIS_InteractiveContext) theCtx = new AIS_InteractiveContext (aViewer);
    for (XCAFPrs_DocumentExplorer aDocExp (anXdeDoc, XCAFPrs_DocumentExplorerFlags_None);
         aDocExp.More(); 
         aDocExp.Next())
    {
        const XCAFPrs_DocumentNode& aNode = aDocExp.Current();
        if (aNode.IsAssembly) { continue; }
        Handle(XCAFPrs_AISObject) aPrs = new XCAFPrs_AISObject (aNode.RefLabel);
        aPrs->SetLocalTransformation (aNode.Location);
        aPrs->SetOwner (new TCollection_HAsciiString (aNode.Id));
        theCtx->Display (aPrs, AIS_Shaded, 0, false); // segmentation fault
        theCtx->SetColor (aPrs, Quantity_NOC_RED, true);

        std::cin.ignore();

        theCtx->Erase (aPrs, true);
    }
    theView->FitAll(0.01, false);
    theView->Redraw();
    
    return 0;
}