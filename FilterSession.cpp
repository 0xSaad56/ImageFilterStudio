#include "FilterSession.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

//  Constructor / Destructor 

FilterSession::FilterSession(const std::string& cnic, const std::string& ts)
    : image(nullptr), cnic(cnic), timestamp(ts)
{}

FilterSession::~FilterSession() {
    clearPipeline();
    delete image;
    image = nullptr;
}

//  Method Chaining 

FilterSession& FilterSession::addFilter(Filter* f) {
    pipeline.push_back(f);
    return *this;   // enables method chaining
}

void FilterSession::clearPipeline() {
    for (Filter* f : pipeline)
        delete f;
    pipeline.clear();
}

// ─── Image Loading ───────────────────────────────────────────────────────────

bool FilterSession::loadImage(const std::string& path) {
    delete image;
    image = new Image(1, 1);   // placeholder; loadFromFile resizes internally
    try {
        if (!image->loadFromFile(path)) {
            delete image;
            image = nullptr;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "[Session] Exception loading image: " << e.what() << "\n";
        delete image;
        image = nullptr;
        return false;
    }
    return true;
}

void FilterSession::useTestPattern(int width, int height) {
    delete image;
    image = new Image(width, height);
    image->generateTestPattern();
}

// ─── Apply Pipeline ──────────────────────────────────────────────────────────

void FilterSession::applyPipeline() {
    if (!image) {
        std::cout << "  [!] No image loaded.\n";
        return;
    }
    if (pipeline.empty()) {
        std::cout << "  [!] Pipeline is empty.\n";
        return;
    }

    std::cout << "\n=== Applying Pipeline ===\n";
    for (size_t i = 0; i < pipeline.size(); ++i) {
        Filter* f = pipeline[i];
        std::cout << "Applying filter " << (i+1) << "/" << pipeline.size()
                  << ": " << f->getName() << " ...\n";
        try {
            f->apply(*image);
        } catch (const std::exception& e) {
            std::cerr << "  [!] Filter error: " << e.what() << "\n";
        }
        image->displayASCII();
    }
    std::cout << "All " << pipeline.size() << " filter(s) applied.\n";
}

// ─── Save Result ─────────────────────────────────────────────────────────────

std::string FilterSession::saveResult(const std::string& ext) {
    if (!image) return "";
    outputFile = cnic + "_" + timestamp + "." + ext;
    try {
        if (image->save(outputFile))
            std::cout << "  Image saved to: " << outputFile << "\n";
        else
            std::cout << "  [!] Failed to save image.\n";
    } catch (const std::exception& e) {
        std::cerr << "  [!] Save error: " << e.what() << "\n";
    }
    return outputFile;
}

// ─── Pipeline String ─────────────────────────────────────────────────────────

std::string FilterSession::getPipelineString() const {
    if (pipeline.empty()) return "(none)";
    std::string result;
    for (size_t i = 0; i < pipeline.size(); ++i) {
        if (i) result += ">";
        result += pipeline[i]->getName();
    }
    return result;
}

// ─── Friend Access Demo ──────────────────────────────────────────────────────

void FilterSession::demoFriendAccess() const {
    if (!image || image->grid == nullptr) return;
    // As a friend of Image we can access private members directly
    std::cout << "[Friend] Direct grid access: top-left pixel = "
              << image->grid[0][0] << "\n";
}
