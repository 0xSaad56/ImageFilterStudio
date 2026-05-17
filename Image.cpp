// STB implementations — defined ONCE here
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Image.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstring>

// ─── Helpers ────────────────────────────────────────────────────────────────

static void allocGrid(Pixel**& grid, int width, int height) {
    grid = new Pixel*[height];
    for (int r = 0; r < height; ++r)
        grid[r] = new Pixel[width];
}

static void freeGrid(Pixel**& grid, int height) {
    if (!grid) return;
    for (int r = 0; r < height; ++r)
        delete[] grid[r];
    delete[] grid;
    grid = nullptr;
}

// Constructors / Destructor

Image::Image(int w, int h) : grid(nullptr), width(w), height(h) {
    allocGrid(grid, width, height);
}

// Deep copy constructor for filter
Image::Image(const Image& other) : grid(nullptr), width(other.width), height(other.height) {
    allocGrid(grid, width, height);
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c)
            grid[r][c] = other.grid[r][c];
}

// Copy assignment operator
Image& Image::operator=(const Image& other) {
    if (this == &other) return *this;
    freeGrid(grid, height);
    width  = other.width;
    height = other.height;
    allocGrid(grid, width, height);
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c)
            grid[r][c] = other.grid[r][c];
    return *this;
}

Image::~Image() {
    freeGrid(grid, height);
}

// ─── Accessors ───────────────────────────────────────────────────────────────

Pixel& Image::at(int row, int col) {
    if (row < 0 || row >= height || col < 0 || col >= width)
        throw std::out_of_range("Image::at() index out of range");
    return grid[row][col];
}

const Pixel& Image::at(int row, int col) const {
    if (row < 0 || row >= height || col < 0 || col >= width)
        throw std::out_of_range("Image::at() const index out of range");
    return grid[row][col];
}

int Image::getWidth()  const { return width;  }
int Image::getHeight() const { return height; }

// ─── File I/O ────────────────────────────────────────────────────────────────

bool Image::loadFromFile(const std::string& path) {
    int w, h, ch;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 3);
    if (!data) {
        std::cerr << "[Image] stbi_load failed: " << stbi_failure_reason() << "\n";
        return false;
    }
    // Re-allocate grid with new dimensions
    freeGrid(grid, height);
    width  = w;
    height = h;
    allocGrid(grid, width, height);

    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c) {
            int idx = 3 * (r * width + c);
            grid[r][c].setRGB(data[idx], data[idx+1], data[idx+2]);
        }

    stbi_image_free(data);
    return true;
}

bool Image::save(const std::string& path) {
    // Build flat RGB buffer
    unsigned char* buf = new unsigned char[width * height * 3];
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c) {
            int idx = 3 * (r * width + c);
            buf[idx]   = static_cast<unsigned char>(grid[r][c].getR());
            buf[idx+1] = static_cast<unsigned char>(grid[r][c].getG());
            buf[idx+2] = static_cast<unsigned char>(grid[r][c].getB());
        }

    bool ok = false;
    // Choose format by extension
    std::string ext;
    auto dot = path.rfind('.');
    if (dot != std::string::npos) ext = path.substr(dot + 1);
    for (auto& ch : ext) ch = static_cast<char>(tolower(ch));

    if (ext == "jpg" || ext == "jpeg")
        ok = (stbi_write_jpg(path.c_str(), width, height, 3, buf, 95) != 0);
    else
        ok = (stbi_write_png(path.c_str(), width, height, 3, buf, width * 3) != 0);

    delete[] buf;
    return ok;
}

// ─── ASCII Display ───────────────────────────────────────────────────────────

void Image::displayASCII() const {
    // Scale down for display — max 60 cols wide
    const int MAX_COLS = 60;
    const int MAX_ROWS = 25;
    int stepC = std::max(1, width  / MAX_COLS);
    int stepR = std::max(1, height / MAX_ROWS);

    int dispCols = width  / stepC;
    int dispRows = height / stepR;

    std::cout << " Image Preview (" << width << " x " << height << ")";
    for (int r = 0; r < dispRows; ++r) {
        for (int c = 0; c < dispCols; ++c)
            std::cout << grid[r * stepR][c * stepC].toASCIIChar();
        std::cout << "\n";
    }
    std::cout << "Image size: " << width << " x " << height
              << " | Total pixels: " << (width * height) << "\n\n";
}

// ─── Test Pattern ────────────────────────────────────────────────────────────

void Image::generateTestPattern() {
    for (int r = 0; r < height; ++r)
        for (int c = 0; c < width; ++c) {
            int red   = (c * 255) / (width  - 1);
            int green = (r * 255) / (height - 1);
            int blue  = 128;
            grid[r][c].setRGB(red, green, blue);
        }
}
