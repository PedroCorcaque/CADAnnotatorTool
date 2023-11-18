#pragma once

#include <X11/Xlib.h>
#include <string>

class Button {
public:
    Button(int x, int y, unsigned int width, unsigned int height, const std::string& label);

    void Draw(Display* display, Window window, GC gc) const;
    bool IsClicked(int mouseX, int mouseY) const;
    std::string GetLabel() const;

private:
    int x, y;
    unsigned int width, height;
    std::string label;
};

