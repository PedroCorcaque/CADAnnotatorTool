#pragma once

#include <string>
#include <vector>
#include <tuple>
#include "json.hpp"
using json = nlohmann::json;

#include <TDataStd_Name.hxx>
#include <Quantity_ColorRGBA.hxx>

namespace OcctLabelTools
{
    struct Label
    {
        std::string nameAsString;
        Handle_TDF_Attribute label;
        Quantity_Color color;

        Label(Handle_TDF_Attribute aLabel, Quantity_Color aColor, std::string aName) : label(aLabel), color(aColor), nameAsString(aName) {};

        friend std::ostream& operator<<(std::ostream& os, const Label& theLabel)
        {
            os << "Class: ";

            os << theLabel.nameAsString;

            os << "\nColor: ";
            theLabel.color.DumpJson(std::cout);
            return os;
        }
    };

    std::tuple<double, double, double> hsvToRgb(double h, double s, double v);
    std::vector<std::tuple<double, double, double>> getEvenlyDistributedColors(const int count);

    std::vector<OcctLabelTools::Label> createLabelsFromFile(json aLabelArray);
}