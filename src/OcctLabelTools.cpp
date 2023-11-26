#include "OcctLabelTools.hpp"

std::vector<OcctLabelTools::Label> OcctLabelTools::createLabelsFromFile(json aLabelArray)
{
    std::vector<Label> theLabels;
    std::vector<std::tuple<double, double, double>> distributedColors = getEvenlyDistributedColors(aLabelArray.size());
    int counter = 0;
    for (auto& theLabel : aLabelArray)
    {
        std::string className = theLabel["class"];

        Handle(TDataStd_Name) theClassName = new TDataStd_Name;
        theClassName->Set(className.c_str());

        Handle_TDF_Attribute theClassName_Attr = theClassName;

        Quantity_Color theColor;
        std::tuple<double, double, double> aColor = distributedColors[counter];
        Standard_Real r = std::get<0>(aColor);
        Standard_Real g = std::get<1>(aColor);
        Standard_Real b = std::get<2>(aColor);
        theColor.SetValues(
            r / 255.0,
            g / 255.0,
            b / 255.0,
            Quantity_TOC_RGB
        );
        
        counter++;

        OcctLabelTools::Label aLabel (theClassName_Attr, theColor);
        theLabels.push_back(aLabel);
    }

    return theLabels;
}

std::tuple<double, double, double> OcctLabelTools::hsvToRgb(double h, double s, double v)
{
    float r, g, b;
    int i = int(h*6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
        case 0: r = v * 255, g = t * 255, b = p * 255; break;
        case 1: r = q * 255, g = v * 255, b = p * 255; break;
        case 2: r = p * 255, g = v * 255, b = t * 255; break;
        case 3: r = p * 255, g = q * 255, b = v * 255; break;
        case 4: r = t * 255, g = p * 255, b = v * 255; break;
        case 5: r = v * 255, g = p * 255, b = q * 255; break;
    }

    return std::make_tuple(r, g, b);
}

std::vector<std::tuple<double, double, double>> OcctLabelTools::getEvenlyDistributedColors(const int count)
{
    std::vector<std::tuple<double, double, double>> colors;

    for (int i = 0; i < count; i++)
    {
        double h = static_cast<double>(i) / count;
        double s = 1.0;
        double v = 1.0;
        colors.push_back(hsvToRgb(h, s, v));
    }

    std::random_shuffle(colors.begin(), colors.end());

    return colors;
}