#pragma once

#include <vector>
#include <TCollection_AsciiString.hxx>

namespace Utils
{
    void fillAppArguments(std::vector<TCollection_AsciiString>& theArgsOut, int argc, char** argv)
    {
        for (int anArgIter = 0; anArgIter < argc; anArgIter++)
        {
            theArgsOut.push_back(argv[anArgIter]);
        }
    }
}