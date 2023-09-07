#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_Viewer.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Xw_Window.hxx>
#include <X11/Xlib.h>

int main() {
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        std::cerr << "Failed to open X display." << std::endl;
        return 1;
    }

    int screen = DefaultScreen(display);
    Window rootWindow = RootWindow(display, screen);

    Window window = XCreateSimpleWindow(display, rootWindow, 0, 0, 800, 600, 0, 0, 0);

    XMapWindow(display, window);
    XFlush(display);

    Handle(Aspect_DisplayConnection) displayConnection = new Aspect_DisplayConnection();
    
    Handle(OpenGl_GraphicDriver) graphicDriver = new OpenGl_GraphicDriver(displayConnection);
    graphicDriver->ChangeOptions().buffersNoSwap = 1;

    Handle(V3d_Viewer) viewer = new V3d_Viewer(graphicDriver);

    Handle(AIS_InteractiveContext) context = new AIS_InteractiveContext(viewer);

    TopoDS_Shape cylinder = BRepPrimAPI_MakeCylinder(20.0, 100.0).Shape();

    Handle(AIS_Shape) aisCylinder = new AIS_Shape(cylinder);

    context->Display(aisCylinder, Standard_True);

    aisCylinder->SetColor(Quantity_NOC_RED);

    XEvent event;
    while (true) {
        XNextEvent(display, &event);
        if (event.type == KeyPress) {
            break;
        }

        viewer->Redraw();
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}