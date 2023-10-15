#include "Viewer.hpp"

Viewer::Viewer()
{
    Handle(Aspect_DisplayConnection) aDisplay = new Aspect_DisplayConnection();
    Handle(Graphic3d_GraphicDriver) aDriver = new OpenGl_GraphicDriver (aDisplay);

    Handle(V3d_Viewer) aViewer = new V3d_Viewer (aDriver);
    aViewer->SetDefaultLights();
    aViewer->SetLightOn();

    myView = new V3d_View(aViewer);

    Handle(Xw_Window) aWindow = new Xw_Window (aDisplay, "CAD Annotator", 100, 100, 512, 512);
    Display* anXDisplay = (Display* )aDisplay->GetDisplayAspect();
    XSelectInput (anXDisplay, (Window )aWindow->NativeHandle(),
                    ExposureMask | KeyPressMask | KeyReleaseMask | FocusChangeMask | StructureNotifyMask
                    | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | Button1MotionMask | Button2MotionMask | Button3MotionMask);
    Atom aDelWinAtom = aDisplay->GetAtom (Aspect_XA_DELETE_WINDOW);
    XSetWMProtocols (anXDisplay, (Window)aWindow->NativeHandle(), &aDelWinAtom, 0.1);

    myView->SetWindow (aWindow);
    myView->SetBackgroundColor (Quantity_NOC_GRAY50);
    myView->TriedronDisplay (Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1);
    myView->ChangeRenderingParams().RenderResolutionScale = 2.0f;

    myContext = new AIS_InteractiveContext (aViewer);

    aWindow->Map();
    myView->Redraw();
}

const Handle(AIS_InteractiveContext)& Viewer::Context() const
{
    return myContext;
}

const Handle(V3d_View)& Viewer::View() const
{
    return myView;
}

bool Viewer::SaveXBF(const TCollection_AsciiString& theFilePath)
{
    if (myXdeDoc.IsNull()) {return false;}
    const PCDM_StoreStatus aStatus = myXdeApp->SaveAs(myXdeDoc, TCollection_ExtendedString (theFilePath));
    if (aStatus != PCDM_SS_OK)
    {
        Message::SendFail() << "Error occured during XBF export: " << (int )aStatus << ".\n" << theFilePath;
        return false;
    }
    return true;
}

bool Viewer::OpenXBF(const TCollection_AsciiString& theFilePath)
{
    createXCafApp();
    newDocument();

    const PCDM_ReaderStatus aReaderStatus = myXdeApp->Open (theFilePath, myXdeDoc);
    if (aReaderStatus != PCDM_RS_OK)
    {
        Message::SendFail() << "Error occurred during XBF import: " << (int )aReaderStatus << ".\n" << theFilePath;
        return false;
    }
    return true;
}

bool Viewer::OpenSTEP(const TCollection_AsciiString& theFilePath)
{
    createXCafApp();
    newDocument();

    STEPCAFControl_Controller::Init();
    STEPControl_Controller::Init();

    STEPCAFControl_Reader aReader;
    OSD_Timer aTimer;
    aTimer.Start();
    try
    {
        if (aReader.ReadFile (theFilePath.ToCString()) != IFSelect_RetDone)
        {
            Message::SendFail() << "Error occured reading STEP file\n" << theFilePath;
            return false;
        }
        if (!aReader.Transfer (myXdeDoc))
        {
            Message::SendFail() << "Error occurred transferring STEP file\n" << theFilePath;
            return false;
        }

        Message::SendInfo() << "File '" << theFilePath << "' opened in " << aTimer.ElapsedTime() << " s"; 
    }
    catch (Standard_Failure const& theFailure)
    {
        Message::SendFail() << "Exception raised during STEP import\n[" << theFailure.GetMessageString() << "]\n" << theFilePath;
        return false; 
    }
    return true;
}

void Viewer::DumpXCafDocumentTree()
{
    if (myXdeDoc.IsNull()) { return; }
    for (XCAFPrs_DocumentExplorer aDocExp (myXdeDoc, XCAFPrs_DocumentExplorerFlags_None); aDocExp.More(); aDocExp.Next())
    {
        TCollection_AsciiString aName = getXCafNodePathNames (aDocExp, false, aDocExp.CurrentDepth());
        aName = TCollection_AsciiString (aDocExp.CurrentDepth() * 2, ' ') + aName;
        std::cout << aName << "\n";
    }
    std::cout << "\n";
}

void Viewer::DisplayXCafDocument(bool theToExplode)
{
    if (myXdeDoc.IsNull()) { return; }
    for (XCAFPrs_DocumentExplorer aDocExp (myXdeDoc, XCAFPrs_DocumentExplorerFlags_None); aDocExp.More(); aDocExp.Next())
    {
        const XCAFPrs_DocumentNode& aNode = aDocExp.Current();
        if (theToExplode)
        {
            if (aNode.IsAssembly) { continue; }
        }
        else
        {
            if (aDocExp.CurrentDepth() != 0) { continue; }
        }

        Handle(XCAFPrs_AISObject) aPrs = new XCAFPrs_AISObject (aNode.RefLabel);
        if (!aNode.Location.IsIdentity()) { aPrs->SetLocalTransformation (aNode.Location); }

        aPrs->SetOwner (new TCollection_HAsciiString (aNode.Id));

        myContext->Display (aPrs, AIS_Shaded, 0, false);
    }

    myView->FitAll(0.01, false);
    AIS_ViewController::ProcessExpose();
}

bool Viewer::createXCafApp()
{
    if (!myXdeApp.IsNull()) { return true; }
    try
    {
        OCC_CATCH_SIGNALS
        myXdeApp = new TDocStd_Application();
        BinXCAFDrivers::DefineFormat (myXdeApp);
        return true;
    }
    catch(Standard_Failure const& theFailure)
    {
        Message::SendFail() << "Error in creating XCAF application " << theFailure.GetMessageString();
        return false;
    }    
}

void Viewer::newDocument()
{
    if (!myXdeDoc.IsNull())
    {
        if (myXdeDoc->HasOpenCommand()) { myXdeDoc->AbortCommand(); }
        myXdeDoc->Main().Root().ForgetAllAttributes (true);
        myXdeApp->Close (myXdeDoc);
        myXdeDoc.Nullify();
    }

    if (!myXdeApp.IsNull()) { myXdeApp->NewDocument (TCollection_ExtendedString ("BinXCAF"), myXdeDoc); }
    if (!myXdeDoc.IsNull()) { myXdeDoc->SetUndoLimit(10); }
}

TCollection_AsciiString Viewer::getXCafNodePathNames(const XCAFPrs_DocumentExplorer& theExp, const bool theIsInstanceName, const int theLowerDepth)
{
    TCollection_AsciiString aPath;
    for (int aDepth = theLowerDepth; aDepth <= theExp.CurrentDepth(); aDepth++)
    {
        const XCAFPrs_DocumentNode& aNode = theExp.Current (aDepth);
        TCollection_AsciiString aName;
        Handle(TDataStd_Name) aNodeName;
        if (theIsInstanceName)
        {
            if (aNode.Label.FindAttribute (TDataStd_Name::GetID(), aNodeName))
            {
                aName = aNodeName->Get();
            }
        }
        else
        {
            if (aNode.RefLabel.FindAttribute (TDataStd_Name::GetID(), aNodeName))
            {
                aName = aNodeName->Get();
            }
        }

        if (aName.IsEmpty()) { TDF_Tool::Entry (aNode.Label, aName); }
        if (aNode.IsAssembly) { aName += "/"; }
        aPath += aName;
    }
    return aPath;
}

void Viewer::OnSelectionChanged(const Handle(AIS_InteractiveContext)& theCtx, const Handle(V3d_View)& theView)
{
    for (const Handle(SelectMgr_EntityOwner)& aSelIter : theCtx->Selection()->Objects())
    {
        Handle(XCAFPrs_AISObject) anXCafPrs = Handle(XCAFPrs_AISObject)::DownCast (aSelIter->Selectable());
        if (anXCafPrs.IsNull()) { continue; }
        
        {
            Handle(TCollection_HAsciiString) anId = Handle(TCollection_HAsciiString)::DownCast (anXCafPrs->GetOwner());
            std::cout << "Selected Id: '" << (!anId.IsNull() ? anId->String() : "") << "'\n";
        }
        
        {
            Handle(TDataStd_Name) aNodeName;
            if (anXCafPrs->GetLabel().FindAttribute (TDataStd_Name::GetID(), aNodeName))
            {
                std::cout << "      Name: '" << aNodeName->Get() << "'\n";
            }
        }

        {
            TopLoc_Location aLoc;
            XCAFPrs_IndexedDataMapOfShapeStyle aStyles;
            XCAFPrs::CollectStyleSettings (anXCafPrs->GetLabel(), aLoc, aStyles);
            NCollection_Map<Quantity_ColorRGBA, Quantity_ColorRGBAHasher> aColorFilter;
            std::cout << "      Colors:";
            for (XCAFPrs_IndexedDataMapOfShapeStyle::Iterator aStyleIter (aStyles); aStyleIter.More(); aStyleIter.Next())
            {
                const XCAFPrs_Style& aStyle = aStyleIter.Value();
                if (aStyle.IsSetColorSurf() && aColorFilter.Add (aStyle.GetColorSurfRGBA()))
                {
                    std::cout << " " << Quantity_ColorRGBA::ColorToHex (aStyle.GetColorSurfRGBA());
                }
            }
            std::cout << "\n";
        }
    }
}

void Viewer::ProcessExpose()
{
    if (!myView.IsNull())
    {
        FlushViewEvents (myContext, myView, true);
    }
}

void Viewer::ProcessConfigure(bool theIsResized)
{
    if (!myView.IsNull() && theIsResized)
    {
        myView->Window()->DoResize();
        myView->MustBeResized();
        myView->Invalidate();
        FlushViewEvents (myContext, myView, true);
    }
}

void Viewer::ProcessInput()
{
    if (!myView.IsNull())
    {
        ProcessExpose();
    }
}

