#include "Utils.hpp"

TopTools_ListOfShape Utils::readStepFile(std::string stepFilePath)
{
    STEPControl_Reader stepControlReader;
    Standard_Integer result, numberShapes;
    TopTools_ListOfShape shapes;

    auto status = stepControlReader.ReadFile(stepFilePath.c_str());
    if (status == 1)
    {
        /* Logger */
        bool failsonly = false;
        stepControlReader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity);
        stepControlReader.PrintCheckTransfer(failsonly, IFSelect_ItemsByEntity);

        //Translates all translatable roots and returns the number of successful translations. Warning - This function clears existing output shapes first.
        result = stepControlReader.TransferRoots();
        if (!result)
        {
            throw std::runtime_error("Transfer failed");
        }

        // Returns the number of shapes produced by translation.
        numberShapes = stepControlReader.NbShapes(); 
        if (numberShapes == 0)
        {
            throw std::runtime_error("No shape to transfer");
        }
        else if (numberShapes == 1)
        {
            TopoDS_Shape shape = stepControlReader.Shape(1);
            shapes.Append(shape);
        }
        else
        {
            std::cout << "There are " 
                    << numberShapes 
                    << " shapes." << std::endl;

            for (Standard_Integer k = 1; k < numberShapes+1; k++)
            {
                TopoDS_Shape shape_k = stepControlReader.Shape(k);
                if (!shape_k.IsNull())
                {
                    shapes.Append(shape_k);
                }
            }
            std::cout << "Returning a list of shapes" << std::endl;
        }
    }

    return shapes;
}

Handle_TDocStd_Document Utils::readStepCafFile(std::string stepFilePath)
{
    /**
     * https://unlimited3d.wordpress.com/2022/09/09/colored-step-model-in-occt-viewer/
     * 
     * 
    */
    // create a document
    Handle(TDocStd_Application) anXdeApp = new TDocStd_Application();
    BinXCAFDrivers::DefineFormat (anXdeApp);
    Handle(TDocStd_Document) anXdeDoc;
    anXdeApp->NewDocument(TCollection_ExtendedString("BinXCAF"), anXdeDoc);

    // Reading step
    STEPCAFControl_Controller::Init();
    STEPControl_Controller::Init();

    STEPCAFControl_Reader aReader;
    try
    {
        if (aReader.ReadFile(stepFilePath.c_str()) != IFSelect_RetDone)
        {
            Message::SendFail() << "Step reader failed";
        }

        if (!aReader.Transfer(anXdeDoc))
        {
            Message::SendFail() << "Step translator failed";
        }
    }
    catch(Standard_Failure const & theFail)
    {
        Message::SendFail() << "Step import exception\n["
                            << theFail.GetMessageString() << "]";
    }

    // // exploring assembly structure
    // for (XCAFPrs_DocumentExplorer aDocExp (anXdeDoc, XCAFPrs_DocumentExplorerFlags_None);
    //      aDocExp.More(); 
    //      aDocExp.Next())
    // {
    //     const XCAFPrs_DocumentNode& aNode = aDocExp.Current();
    //     TCollection_AsciiString aName (aDocExp.CurrentDepth() * 2, ' ');
    //     Handle(TDataStd_Name) aNodeName;
    //     if (aNode.RefLabel.FindAttribute (TDataStd_Name::GetID(), aNodeName)
    //         || aNode.Label.FindAttribute (TDataStd_Name::GetID(), aNodeName))
    //     {
    //         aName += aNodeName->Get();
    //     }
    //     std::cout << aName << "[id: " << aNode.Id << "]\n";
    // }

    return anXdeDoc;
}

TopTools_ListOfShape Utils::splitShapeIntoShells(const TopoDS_Shape& shape)
{
    TopTools_ListOfShape shells;

    TopoDS_Compound compound;
    BRep_Builder builder;
    builder.MakeCompound(compound);
    builder.Add(compound, shape);

    TopExp_Explorer explorer(compound, TopAbs_SOLID);
    while (explorer.More())
    {
        const TopoDS_Solid& solid = TopoDS::Solid(explorer.Current());
        // TopExp_Explorer shellExplorer(solid, TopAbs_SHELL);
        // while (shellExplorer.More())
        // {
        //     const TopoDS_Shell& shell = TopoDS::Shell(shellExplorer.Current());
        //     shells.Append(shell);

        //     shellExplorer.Next();
        // }
        shells.Append(solid);

        explorer.Next();
    }

    return shells;
}