#include "Filters.h"
#include "Image.h"
#include <algorithm>
#include <limits>
#include <string>

// ── Filter 01: Grayscale ──────────────────────────────────────────────────────
void GrayscaleFilter::apply(Image& img) {
    int h = img.getHeight(), w = img.getWidth();
    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c) {
            Pixel& p  = img.at(r, c);
            int avg = (p.getR() + p.getG() + p.getB()) / 3;
            p.setRGB(avg, avg, avg);
        }
}

// ── Filter 02: Invert ─────────────────────────────────────────────────────────
void InvertFilter::apply(Image& img) {
    int h = img.getHeight(), w = img.getWidth();
    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c) {
            Pixel& p = img.at(r, c);
            p.setRGB(255 - p.getR(), 255 - p.getG(), 255 - p.getB());
        }
}

// ── Filter 03: Brightness Adjust ─────────────────────────────────────────────
std::string BrightnessFilter::getDescription() const {
    return std::string("Adjusts brightness by ") +
           (amount >= 0 ? "+" : "") + std::to_string(amount);
}

void BrightnessFilter::apply(Image& img) {
    int h = img.getHeight(), w = img.getWidth();
    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c) {
            Pixel& p = img.at(r, c);
            p.setRGB(
                Pixel::clamp(p.getR() + amount),
                Pixel::clamp(p.getG() + amount),
                Pixel::clamp(p.getB() + amount)
            );
        }
}

// ── Filter 04: Contrast Stretch ───────────────────────────────────────────────
void ContrastStretchFilter::apply(Image& img) {
    int h = img.getHeight(), w = img.getWidth();

    // Find per-channel min and max over the whole image
    int minR = 255, maxR = 0;
    int minG = 255, maxG = 0;
    int minB = 255, maxB = 0;

    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c) {
            const Pixel& p = img.at(r, c);
            minR = std::min(minR, p.getR());
             maxR = std::max(maxR, p.getR());
            minG = std::min(minG, p.getG()); 
            maxG = std::max(maxG, p.getG());
            minB = std::min(minB, p.getB()); 
            maxB = std::max(maxB, p.getB());
        }

    // Avoid division by zero
    int rangeR = (maxR == minR) ? 1 : maxR - minR;
    int rangeG = (maxG == minG) ? 1 : maxG - minG;
    int rangeB = (maxB == minB) ? 1 : maxB - minB;

    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c) {
            Pixel& p = img.at(r, c);
            int newR = (p.getR() - minR) * 255 / rangeR;
            int newG = (p.getG() - minG) * 255 / rangeG;
            int newB = (p.getB() - minB) * 255 / rangeB;
            p.setRGB(newR, newG, newB);
        }
}

// ── Filter 05: Red Channel Only ───────────────────────────────────────────────
void RedChannelFilter::apply(Image& img) {
    int h = img.getHeight(), w = img.getWidth();
    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c) {
            Pixel& p = img.at(r, c);
            p.setRGB(p.getR(), 0, 0);
        }
}

// ── Filter 06: Green Channel Only ─────────────────────────────────────────────
void GreenChannelFilter::apply(Image& img) {
    int h = img.getHeight(), w = img.getWidth();
    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c) {
            Pixel& p = img.at(r, c);
            p.setRGB(0, p.getG(), 0);
        }
}

// ── Filter 07: Blue Channel Only ──────────────────────────────────────────────
void BlueChannelFilter::apply(Image& img) {
    int h = img.getHeight(), w = img.getWidth();
    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w; ++c) {
            Pixel& p = img.at(r, c);
            p.setRGB(0, 0, p.getB());
        }
}

// ── Filter 08: Box Blur (3×3) ─────────────────────────────────────────────────
// IMPORTANT: operates on a COPY so modified pixels don't affect neighbours
void BoxBlurFilter::apply(Image& img) {
    Image copy(img);   // deep copy via copy constructor

    int h = img.getHeight(), w = img.getWidth();
    for (int r = 0; r < h; ++r) {
        for (int c = 0; c < w; ++c) {
            int sumR = 0, sumG = 0, sumB = 0, count = 0;
            // Visit 3×3 neighbourhood
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int nr = r + dr, nc = c + dc;
                    if (nr >= 0 && nr < h && nc >= 0 && nc < w) {
                        const Pixel& p = copy.at(nr, nc);
                        sumR += p.getR();
                        sumG += p.getG();
                        sumB += p.getB();
                        ++count;
                    }
                }
            }
            img.at(r, c).setRGB(sumR / count, sumG / count, sumB / count);
        }
    }
}

// ── Filter 09: Flip Horizontal ────────────────────────────────────────────────
void FlipHorizontalFilter::apply(Image& img) {
    int h = img.getHeight(), w = img.getWidth();
    for (int r = 0; r < h; ++r)
        for (int c = 0; c < w / 2; ++c) {
            Pixel tmp          = img.at(r, c);
            img.at(r, c)       = img.at(r, w - 1 - c);
            img.at(r, w-1-c)   = tmp;
        }
}

// ── Filter 10: Flip Vertical ──────────────────────────────────────────────────
void FlipVerticalFilter::apply(Image& img) {
    int h = img.getHeight(), w = img.getWidth();
    for (int r = 0; r < h / 2; ++r)
        for (int c = 0; c < w; ++c) {
            Pixel tmp              = img.at(r, c);
            img.at(r, c)           = img.at(h - 1 - r, c);
            img.at(h - 1 - r, c)   = tmp;
        }
}