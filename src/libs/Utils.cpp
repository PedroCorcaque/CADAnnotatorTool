#include "Utils.hpp"

std::vector<TopoDS_Shape> Utils::readStepFile(std::string stepFilePath)
{
    STEPControl_Reader stepControlReader;
    IFSelect_ReturnStatus ifSelectReturnStatusEnum;
    Standard_Integer result, numberShapes;
    std::vector<TopoDS_Shape> shapes;

    ifSelectReturnStatusEnum = stepControlReader.ReadFile(stepFilePath.c_str());

    switch (ifSelectReturnStatusEnum)
    {
        case 0: // IFSelect_RetVoid
            {
                std::cout << "normal execution which created nothing, or no data to process" << std::endl;
                break;
            }
        
        case 1: // IFSelect_RetDone
            {
                /* Logger */
                bool failsonly = false;
                stepControlReader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity);
                stepControlReader.PrintCheckTransfer(failsonly, IFSelect_ItemsByEntity);

                //Translates all translatable roots and returns the number of successful translations. Warning - This function clears existing output shapes first.
                result = stepControlReader.TransferRoots();
                if (!result)
                {
                    std::cerr << "Transfer failed" << std::endl;
                    break;
                }

                // Returns the number of shapes produced by translation.
                numberShapes = stepControlReader.NbShapes(); 
                if (numberShapes == 0)
                {
                    std::cerr << "No shape to transfer" << std::endl;
                    break;
                }
                else if (numberShapes == 1)
                {
                    TopoDS_Shape shape = stepControlReader.Shape(1);
                    shapes.push_back(shape);
                    break;
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
                            shapes.push_back(shape_k);
                        }
                    }
                    std::cout << "Returning a list of shapes" << std::endl;
                }

                break;
            }

        case 2: // IFSelect_RetError
            {
                std::cout << "error in command or input data, no execution" << std::endl;
                break;
            }

        case 3: // IFSelect_RetFail
            {
                std::cout << "execution was run and has failed" << std::endl;
                break;
            }

        case 4: // IFSelect_RetStop
            {
                std::cout << "indicates end or stop (such as Raise)" << std::endl;
                break;
            }
    }

    return shapes;
}