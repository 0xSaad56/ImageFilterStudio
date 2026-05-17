#pragma once
#include "Filter.h"
#include <string>

// ══════════════════════════════════════════════════════════════════════════════
// Filter 01 — Grayscale
// ══════════════════════════════════════════════════════════════════════════════
class GrayscaleFilter : public Filter {
public:
    GrayscaleFilter() : Filter("01", "Grayscale", "Pixel Transform") {}
    void apply(Image& img) override;
    std::string getDescription() const override { return "Converts to grayscale (R=G=B=avg)"; }
    Filter* clone() const override { return new GrayscaleFilter(*this); }
};

// ══════════════════════════════════════════════════════════════════════════════
// Filter 02 — Invert / Negative
// ══════════════════════════════════════════════════════════════════════════════
class InvertFilter : public Filter {
public:
    InvertFilter() : Filter("02", "Invert", "Pixel Transform") {}
    void apply(Image& img) override;
    std::string getDescription() const override { return "Inverts colours (255-R, 255-G, 255-B)"; }
    Filter* clone() const override { return new InvertFilter(*this); }
};

// ══════════════════════════════════════════════════════════════════════════════
// Filter 03 — Brightness Adjust
// ══════════════════════════════════════════════════════════════════════════════
class BrightnessFilter : public Filter {
private:
    int amount;   // -255 to +255
public:
    explicit BrightnessFilter(int amount = 30)
        : Filter("03", "Brightness Adjust", "Pixel Transform"), amount(amount) {}
    int  getAmount() const { return amount; }
    void setAmount(int a)  { this->amount = a; }
    void apply(Image& img) override;
    std::string getDescription() const override;
    Filter* clone() const override { return new BrightnessFilter(*this); }
};

// ══════════════════════════════════════════════════════════════════════════════
// Filter 04 — Contrast Stretch
// ══════════════════════════════════════════════════════════════════════════════
class ContrastStretchFilter : public Filter {
public:
    ContrastStretchFilter() : Filter("04", "Contrast Stretch", "Pixel Transform") {}
    void apply(Image& img) override;
    std::string getDescription() const override { return "Linearly stretches contrast to full range"; }
    Filter* clone() const override { return new ContrastStretchFilter(*this); }
};

// ══════════════════════════════════════════════════════════════════════════════
// Filter 05 — Red Channel Only
// ══════════════════════════════════════════════════════════════════════════════
class RedChannelFilter : public Filter {
public:
    RedChannelFilter() : Filter("05", "Red Channel Only", "Pixel Transform") {}
    void apply(Image& img) override;
    std::string getDescription() const override { return "Keeps Red channel; zeroes G and B"; }
    Filter* clone() const override { return new RedChannelFilter(*this); }
};

// ══════════════════════════════════════════════════════════════════════════════
// Filter 06 — Green Channel Only
// ══════════════════════════════════════════════════════════════════════════════
class GreenChannelFilter : public Filter {
public:
    GreenChannelFilter() : Filter("06", "Green Channel Only", "Pixel Transform") {}
    void apply(Image& img) override;
    std::string getDescription() const override { return "Keeps Green channel; zeroes R and B"; }
    Filter* clone() const override { return new GreenChannelFilter(*this); }
};

// ══════════════════════════════════════════════════════════════════════════════
// Filter 07 — Blue Channel Only
// ══════════════════════════════════════════════════════════════════════════════
class BlueChannelFilter : public Filter {
public:
    BlueChannelFilter() : Filter("07", "Blue Channel Only", "Pixel Transform") {}
    void apply(Image& img) override;
    std::string getDescription() const override { return "Keeps Blue channel; zeroes R and G"; }
    Filter* clone() const override { return new BlueChannelFilter(*this); }
};

// ══════════════════════════════════════════════════════════════════════════════
// Filter 08 — Box Blur (3x3)
// ══════════════════════════════════════════════════════════════════════════════
class BoxBlurFilter : public Filter {
public:
    BoxBlurFilter() : Filter("08", "Box Blur (3x3)", "Spatial Filter") {}
    void apply(Image& img) override;
    std::string getDescription() const override { return "3x3 average blur; edge-safe"; }
    Filter* clone() const override { return new BoxBlurFilter(*this); }
};

// ══════════════════════════════════════════════════════════════════════════════
// Filter 09 — Flip Horizontal
// ══════════════════════════════════════════════════════════════════════════════
class FlipHorizontalFilter : public Filter {
public:
    FlipHorizontalFilter() : Filter("09", "Flip Horizontal", "Geometric") {}
    void apply(Image& img) override;
    std::string getDescription() const override { return "Mirrors the image left-to-right"; }
    Filter* clone() const override { return new FlipHorizontalFilter(*this); }
};

// ══════════════════════════════════════════════════════════════════════════════
// Filter 10 — Flip Vertical
// ══════════════════════════════════════════════════════════════════════════════
class FlipVerticalFilter : public Filter {
public:
    FlipVerticalFilter() : Filter("10", "Flip Vertical", "Geometric") {}
    void apply(Image& img) override;
    std::string getDescription() const override { return "Mirrors the image top-to-bottom"; }
    Filter* clone() const override { return new FlipVerticalFilter(*this); }
};
