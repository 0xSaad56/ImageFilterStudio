#pragma once
#include "Filters.h"
#include "Image.h"
#include <vector>
#include <string>
#include <iostream>

// FilterSession manages a chain of filters applied to an Image.
// It is declared friend of Image, so it can access the private pixel grid.
class FilterSession {
private:
    std::vector<Filter*> pipeline;   // Runtime polymorphism: Filter* vector
    Image*               image;      // Owned pointer (we manage lifetime)
    std::string          cnic;
    std::string          timestamp;
    std::string          outputFile;

public:
    FilterSession(const std::string& cnic, const std::string& ts);
    ~FilterSession();

    // Method chaining: returns *this so calls can be chained
    FilterSession& addFilter(Filter* f);

    // Clear and rebuild pipeline
    void clearPipeline();

    // Load image
    bool loadImage(const std::string& path);
    void useTestPattern(int width = 64, int height = 48);

    // Apply all filters in sequence, showing ASCII preview after each
    void applyPipeline();

    // Save result; returns output filename
    std::string saveResult(const std::string& ext = "png");

    // Getters for session info
    const std::vector<Filter*>& getPipeline()  const { return pipeline;   }
    std::string getCnic()                       const { return cnic;       }
    std::string getTimestamp()                  const { return timestamp;  }
    std::string getOutputFile()                 const { return outputFile; }
    Image*      getImage()                      const { return image;      }

    // Formatted string of applied filters for sessions.txt
    std::string getPipelineString() const;

    // Direct grid access (demonstrates friend relationship)
    void demoFriendAccess() const;
};
