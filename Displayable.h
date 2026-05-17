#pragma once

// Interface: anything that can be displayed as ASCII art
class Displayable {
public:
    virtual void displayASCII() const = 0;
    virtual ~Displayable() = default;
};
