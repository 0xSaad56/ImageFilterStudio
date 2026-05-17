#pragma once
#include "User.h"
#include "Filters.h"
#include <vector>
#include <string>

// ══════════════════════════════════════════════════════════════════════════════
// CustomerFileManager — handles customers.txt and blocked_cnics.txt
// ══════════════════════════════════════════════════════════════════════════════
class CustomerFileManager {
private:
    std::string customersFile;
    std::string blockedFile;

public:
    CustomerFileManager(const std::string& custFile   = "customers.txt",
                        const std::string& blockedFile = "blocked_cnics.txt");

    // Load all customers
    std::vector<Customer> loadAll() const;

    // Save all customers (overwrites)
    void saveAll(const std::vector<Customer>& customers) const;

    // Append one new customer
    void appendCustomer(const Customer& c) const;

    // Find customer by CNIC; returns nullptr if not found
    Customer* findByCnic(const std::string& cnic,
                         std::vector<Customer>& customers) const;

    // Block a customer (update file + add to blocked_cnics.txt)
    bool blockCustomer(const std::string& cnic);

    // Delete a customer record
    bool deleteCustomer(const std::string& cnic);

    // Check if a CNIC is in blocked_cnics.txt
    bool isCnicBlocked(const std::string& cnic) const;

    // Add a CNIC to blocked_cnics.txt
    void addToBlockedList(const std::string& cnic) const;

    // Count sessions for a CNIC from sessions.txt
    int countSessions(const std::string& cnic) const;
};

// ══════════════════════════════════════════════════════════════════════════════
// CatalogFileManager — handles catalog.txt
// ══════════════════════════════════════════════════════════════════════════════
class CatalogFileManager {
private:
    std::string catalogFile;

    struct CatalogEntry {
        std::string id;
        std::string name;
        std::string category;
        bool enabled;
    };

public:
    explicit CatalogFileManager(const std::string& file = "catalog.txt");

    // Load enabled-status from file into the filter vector
    void loadCatalog(std::vector<Filter*>& filters) const;

    // Save current enabled-status to file
    void saveCatalog(const std::vector<Filter*>& filters) const;

    // Create default catalog.txt if it doesn't exist
    void ensureDefaultCatalog(const std::vector<Filter*>& filters) const;
};

// ══════════════════════════════════════════════════════════════════════════════
// SessionFileManager — handles sessions.txt
// ══════════════════════════════════════════════════════════════════════════════
class SessionFileManager {
private:
    std::string sessionsFile;

public:
    struct SessionRecord {
        std::string cnic;
        std::string timestamp;
        std::string filtersApplied;
        std::string outputFile;
    };

    explicit SessionFileManager(const std::string& file = "sessions.txt");

    // Append one session record
    void appendSession(const std::string& cnic,
                       const std::string& timestamp,
                       const std::string& filtersApplied,
                       const std::string& outputFile) const;

    // Load all session records
    std::vector<SessionRecord> loadAll() const;

    // Load sessions for a specific CNIC
    std::vector<SessionRecord> loadForCnic(const std::string& cnic) const;
};
