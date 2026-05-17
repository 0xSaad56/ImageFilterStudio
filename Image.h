#pragma once
#include "Pixel.h"
#include "Saveable.h"
#include "Displayable.h"
#include <string>

// Forward declaration for friend relationship
class FilterSession;

// Image inherits from BOTH Saveable and Displayable — Multiple Inheritance
class Image : public Saveable, public Displayable {
private:
    Pixel** grid;   // 2D dynamically-allocated pixel array (heap)
    int     width;
    int     height;

    // Declare FilterSession as a friend so it can access the grid directly
    friend class FilterSession;

public:
    // ── Constructors / Destructor ──────────────────────────────────────────
    Image(int width, int height);           // Allocates blank (black) grid
    Image(const Image& other);              // Deep copy constructor
    Image& operator=(const Image& other);   // Copy assignment
    ~Image();                               // Frees heap memory

    // ── Accessors ─────────────────────────────────────────────────────────
    Pixel&       at(int row, int col);
    const Pixel& at(int row, int col) const;
    int          getWidth()  const;
    int          getHeight() const;

    // ── File I/O ──────────────────────────────────────────────────────────
    bool loadFromFile(const std::string& path);          // stbi_load
    bool save(const std::string& path) override;         // stbi_write_png/jpg

    // ── Display ───────────────────────────────────────────────────────────
    void displayASCII() const override;

    // ── Test pattern ──────────────────────────────────────────────────────
    void generateTestPattern();
};
