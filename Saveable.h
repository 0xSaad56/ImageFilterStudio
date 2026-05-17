#pragma once
#include <string>

// Interface: anything that can be saved to disk
class Saveable {
public:
    virtual bool save(const std::string& path) = 0;
    virtual ~Saveable() = default;
};
