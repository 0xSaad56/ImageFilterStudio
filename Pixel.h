#pragma once
#include <iostream>

class Pixel {
private:
    int r, g, b;

public:
    // Constructors
    Pixel();                        // Default: black pixel
    Pixel(int r, int g, int b);     // Parameterised constructor

    // Getters (const correctness)
    int getR() const;
    int getG() const;
    int getB() const;

    // Setters (uses this pointer explicitly)
    void setR(int r);
    void setG(int g);
    void setB(int b);
    void setRGB(int r, int g, int b);

    // Static utility: clamp a value to [lo, hi]
    static int clamp(int val, int lo = 0, int hi = 255);

    // Brightness (average of channels)
    int brightness() const;

    // Map brightness to ASCII character
    char toASCIIChar() const;

    // Operator overloads
    Pixel operator+(const Pixel& other) const;      // Blend two pixels (average)
    Pixel& operator+=(const Pixel& other);

    friend std::ostream& operator<<(std::ostream& os, const Pixel& p);
};
