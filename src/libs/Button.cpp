#include "Button.hpp"

Button::Button(int x, int y, unsigned int width, unsigned int height, const std::string& label)
    : x(x), y(y), width(width), height(height), label(label) {}

void Button::Draw(Display* display, Window window, GC gc) const {
    XDrawRectangle(display, window, gc, x, y, width, height);
    XDrawString(display, window, gc, x + 10, y + 20, label.c_str(), label.length());
}

bool Button::IsClicked(int mouseX, int mouseY) const {
    return mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}

std::string Button::GetLabel() const {
    return label;
}