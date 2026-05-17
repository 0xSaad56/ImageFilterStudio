#include "Pixel.h"

// ─── Constructors ────────────────────────────────────────────────────────────

Pixel::Pixel() : r(0), g(0), b(0) {}

Pixel::Pixel(int r, int g, int b) {
    this->r = Pixel::clamp(r);   // explicit this pointer usage
    this->g = Pixel::clamp(g);
    this->b = Pixel::clamp(b);
}

// ─── Getters ─────────────────────────────────────────────────────────────────

int Pixel::getR() const { return r; }
int Pixel::getG() const { return g; }
int Pixel::getB() const { return b; }

// ─── Setters ─────────────────────────────────────────────────────────────────

void Pixel::setR(int r)  { this->r = Pixel::clamp(r); }
void Pixel::setG(int g)  { this->g = Pixel::clamp(g); }
void Pixel::setB(int b)  { this->b = Pixel::clamp(b); }

void Pixel::setRGB(int r, int g, int b) {
    this->r = Pixel::clamp(r);
    this->g = Pixel::clamp(g);
    this->b = Pixel::clamp(b);
}

// ─── Static Utility ──────────────────────────────────────────────────────────

int Pixel::clamp(int val, int lo, int hi) {
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

// ─── Brightness ──────────────────────────────────────────────────────────────

int Pixel::brightness() const {
    return (r + g + b) / 3;
}

char Pixel::toASCIIChar() const {
    int br = brightness();
    // Map [0,255] to characters from dark to light
    if (br < 32)  return ' ';
    if (br < 64)  return '.';
    if (br < 96)  return ':';
    if (br < 128) return '-';
    if (br < 160) return '=';
    if (br < 192) return '+';
    if (br < 224) return '*';
    if (br < 240) return '#';
    return '@';
}

// ─── Operators ───────────────────────────────────────────────────────────────

Pixel Pixel::operator+(const Pixel& other) const {
    return Pixel(
        Pixel::clamp((r + other.r) / 2),
        Pixel::clamp((g + other.g) / 2),
        Pixel::clamp((b + other.b) / 2)
    );
}

Pixel& Pixel::operator+=(const Pixel& other) {
    r = Pixel::clamp(r + other.r);
    g = Pixel::clamp(g + other.g);
    b = Pixel::clamp(b + other.b);
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Pixel& p) {
    os << "RGB(" << p.r << ", " << p.g << ", " << p.b << ")";
    os << p.toASCIIChar();
    return os;
}