#include <Viewer.hpp>
#include <Utils.hpp>

int main(int argc, char** argv) 
{
    OSD::SetSignal (false);

    std::vector<TCollection_AsciiString> anArgs;
    Utils::fillAppArguments(anArgs, argc, argv);

    TCollection_AsciiString aModelPath;
    if (anArgs.size() > 2)
    {
        Message::SendFail() << "Syntax error: wrong number of arguments";
        return 1;
    }

    if (anArgs.size() == 2)
    {
        aModelPath = anArgs[1];
    }
    else
    {
        OSD_Environment aVarModDir ("STEP_DIR");
        if (!aVarModDir.Value().IsEmpty())
        {
            aModelPath = aVarModDir.Value() + "64.stp";
        }
        else
        {
            Message::SendFail() << "Warning: variable STEP_DIR not set";
        }
    }

    Viewer aViewer;
    if (!aModelPath.IsEmpty())
    {
        TCollection_AsciiString aNameLower = aModelPath;
        aNameLower.LowerCase();
        if (aNameLower.EndsWith (".xbf"))
        {
            aViewer.OpenXBF (aModelPath);
        }
        else if (aNameLower.EndsWith (".stp") || aNameLower.EndsWith (".step"))
        {
            aViewer.OpenSTEP (aModelPath);
        }
        else
        {
            std::cout << "There is no step in '" << aModelPath << "'\n";
            return 1;
        }

        aViewer.DumpXCafDocumentTree();
        aViewer.DisplayXCafDocument (true);

        // Iterate button
        const char* buttonIterate = "Iterate by entity";
        aViewer.DrawNewButton(10, 10, 120, 30, buttonIterate);

        // View all button
        const char* buttonViewAll = "View all entities";
        aViewer.DrawNewButton(140, 10, 120, 30, buttonViewAll);
    }

    Handle(Xw_Window) aWindow = aViewer.GetWindow();
    Handle(Aspect_DisplayConnection) aDispConn = aViewer.View()->Viewer()->Driver()->GetDisplayConnection();
    Display* anXDisplay = aViewer.GetDisplay();
    for (;;)
    {
        XEvent anXEvent;
        XNextEvent (anXDisplay, &anXEvent);
        aWindow->ProcessMessage (aViewer, anXEvent);
        if (anXEvent.type == ClientMessage && (Atom)anXEvent.xclient.data.l[0] == aDispConn->GetAtom(Aspect_XA_DELETE_WINDOW))
        {
            return 0;
        }

        // Handle button click events
        if (anXEvent.type == ButtonPress) {
            int mouseX = anXEvent.xbutton.x;
            int mouseY = anXEvent.xbutton.y;
            aViewer.HandleButtonClick(mouseX, mouseY);
        }

        // // Iterate button
        // const char* buttonIterate = "Iterate by entity";
        // aViewer.DrawNewButton(10, 10, 120, 40, buttonIterate);

        // // View all button
        // const char* buttonViewAll = "View all entities";
        // aViewer.DrawNewButton(140, 10, 120, 40, buttonViewAll);
    }

    return 0;
}