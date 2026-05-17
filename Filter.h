#pragma once
#include <string>
#include <iostream>

// Forward declaration — avoids circular include
class Image;

// ─── Abstract Base Class ─────────────────────────────────────────────────────
class Filter {
private:
    static int filterCount;   // static member: tracks total Filter objects

protected:
    std::string id;
    std::string name;
    std::string category;
    bool        enabled;

public:
    Filter(const std::string& id,
           const std::string& name,
           const std::string& category,
           bool enabled = true)
        : id(id), name(name), category(category), enabled(enabled)
    {
        ++filterCount;
        std::cout << "[Filter] " << name << " created (total: " << filterCount << ")\n";
    }

    // Copy constructor — needed so cloned filters also increment the count
    Filter(const Filter& other)
        : id(other.id), name(other.name), category(other.category), enabled(other.enabled)
    {
        ++filterCount;
    }

    // Virtual destructor — essential for polymorphic deletion
    virtual ~Filter() {
        --filterCount;
        std::cout << "[Filter] " << name << " destroyed (remaining: " << filterCount << ")\n";
    }

    // ── Pure virtual — every subclass MUST implement ───────────────────────
    virtual void        apply(Image& img)            = 0;
    virtual std::string getDescription()       const = 0;
    virtual Filter*     clone()                const = 0;

    // ── Getters ────────────────────────────────────────────────────────────
    std::string getId()       const { return id;       }
    std::string getName()     const { return name;     }
    std::string getCategory() const { return category; }
    bool        isEnabled()   const { return enabled;  }

    // ── Enable / Disable (Admin action) ───────────────────────────────────
    void setEnabled(bool e) { this->enabled = e; }

    // ── Static accessor ───────────────────────────────────────────────────
    static int getFilterCount() { return filterCount; }
};