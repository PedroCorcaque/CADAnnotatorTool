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

#include <Xw_Window.hxx>
#include <X11/Xlib.h>

int main() {
    std::string filename = "/home/user/Workspace/aeroscan/data/LS3DC/step/64.stp"; // Filename - Cannot use the $HOME as path (???)
    std::vector<TopoDS_Shape> shapes;   // A list of shapes

    shapes = Utils::readStepFile(filename);
    if (shapes.size() == 0)
    {
        std::cerr << "There is no shape to process." << std::endl;
        return 1;
    }
    
    for (TopoDS_Shape& shape : shapes)
    {        
        /**
         *  Window configuration
         *  https://unlimited3d.wordpress.com/2021/03/27/occt-minimal-viewer-setup/ 
         */

        Handle(Aspect_DisplayConnection) aDisplay = new Aspect_DisplayConnection();
        Handle(Aspect_Window) theWindow = new Xw_Window (aDisplay, "CADAnnotator", 0, 0, 512, 512);
        theWindow->Map();
        while(!theWindow->IsMapped())
        {
            std::cout << "Loading..." << std::endl;
        }

        Handle(OpenGl_GraphicDriver) aGlDriver = new OpenGl_GraphicDriver(aDisplay);
        aGlDriver->ChangeOptions().swapInterval = 1;
        Handle(Graphic3d_GraphicDriver) aDriver = aGlDriver;
        Handle(V3d_Viewer) v3dViewer = new V3d_Viewer (aDriver);    // Receive a opencascade::handle<Graphic3d_GraphicDriver>
        v3dViewer->SetDefaultLights();
        v3dViewer->SetLightOn();
        Handle(V3d_View) aView = new V3d_View (v3dViewer);
        aView->SetImmediateUpdate(false);
        aView->SetShadingModel(Graphic3d_TOSM_FRAGMENT);

        aView->SetWindow(theWindow);    // Causing a segmentation fault if the Window is a null pointer
        aView->SetBackgroundColor (Quantity_NOC_GRAY50);
        aView->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
        aView->TriedronDisplay();

        /**
         * Interactive Objects
         * 
         * 
        */
        Handle(AIS_InteractiveContext) aContext = new AIS_InteractiveContext (v3dViewer);
        Handle(AIS_InteractiveObject) aShape = new AIS_Shape (shape);  // Receive a shape
        aContext->Display(aShape, AIS_Shaded, 0, true);

        std::cout << "The window is ready" << std::endl;
        std::cin.ignore();
    }
    
    
    return 0;
}