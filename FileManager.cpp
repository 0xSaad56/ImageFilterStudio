#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>

// ════════════════════════════════════════════════════════════════════════════
// Helpers
// ════════════════════════════════════════════════════════════════════════════

static std::vector<std::string> splitLine(const std::string& line, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delim))
        parts.push_back(token);
    return parts;
}

// ════════════════════════════════════════════════════════════════════════════
// CustomerFileManager
// ════════════════════════════════════════════════════════════════════════════

CustomerFileManager::CustomerFileManager(const std::string& cf,
                                         const std::string& bf)
    : customersFile(cf), blockedFile(bf) {}

std::vector<Customer> CustomerFileManager::loadAll() const {
    std::vector<Customer> customers;
    try {
        std::ifstream f(customersFile);
        if (!f.is_open()) return customers;
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p = splitLine(line, '|');
            if (p.size() < 7) continue;
            bool blocked  = (p[6] == "1");
            int  sessions = 0;
            if (p.size() >= 8) sessions = std::stoi(p[7]);
            customers.emplace_back(p[0], p[1], p[2], p[3], p[4], p[5], blocked, sessions);
        }
    } catch (const std::exception& e) {
        std::cerr << "[CustomerFileManager] loadAll error: " << e.what() << "\n";
    }
    return customers;
}

void CustomerFileManager::saveAll(const std::vector<Customer>& customers) const {
    try {
        std::ofstream f(customersFile, std::ios::trunc);
        if (!f.is_open()) {
            std::cerr << "[CustomerFileManager] Cannot open " << customersFile << "\n";
            return;
        }
        for (const auto& c : customers)
            f << c.toFileLine() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[CustomerFileManager] saveAll error: " << e.what() << "\n";
    }
}

void CustomerFileManager::appendCustomer(const Customer& c) const {
    try {
        std::ofstream f(customersFile, std::ios::app);
        if (!f.is_open()) {
            std::cerr << "[CustomerFileManager] Cannot open " << customersFile << "\n";
            return;
        }
        f << c.toFileLine() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[CustomerFileManager] appendCustomer error: " << e.what() << "\n";
    }
}

Customer* CustomerFileManager::findByCnic(const std::string& cnic,
                                           std::vector<Customer>& customers) const {
    for (auto& c : customers)
        if (c.getCnic() == cnic) return &c;
    return nullptr;
}

bool CustomerFileManager::blockCustomer(const std::string& cnic) {
    try {
        auto customers = loadAll();
        bool found = false;
        for (auto& c : customers) {
            if (c.getCnic() == cnic) {
                c.setBlocked(true);
                found = true;
                break;
            }
        }
        if (found) {
            saveAll(customers);
            addToBlockedList(cnic);
        }
        return found;
    } catch (const std::exception& e) {
        std::cerr << "[CustomerFileManager] blockCustomer error: " << e.what() << "\n";
        return false;
    }
}

bool CustomerFileManager::deleteCustomer(const std::string& cnic) {
    try {
        auto customers = loadAll();
        auto before = customers.size();
        customers.erase(
            std::remove_if(customers.begin(), customers.end(),
                [&cnic](const Customer& c){ return c.getCnic() == cnic; }),
            customers.end()
        );
        if (customers.size() < before) {
            saveAll(customers);
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "[CustomerFileManager] deleteCustomer error: " << e.what() << "\n";
        return false;
    }
}

bool CustomerFileManager::isCnicBlocked(const std::string& cnic) const {
    try {
        std::ifstream f(blockedFile);
        if (!f.is_open()) return false;
        std::string line;
        while (std::getline(f, line))
            if (line == cnic) return true;
    } catch (...) {}
    return false;
}

void CustomerFileManager::addToBlockedList(const std::string& cnic) const {
    try {
        std::ofstream f(blockedFile, std::ios::app);
        if (f.is_open()) f << cnic << "\n";
    } catch (...) {}
}

int CustomerFileManager::countSessions(const std::string& cnic) const {
    int count = 0;
    try {
        std::ifstream f("sessions.txt");
        if (!f.is_open()) return 0;
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p = splitLine(line, '|');
            if (!p.empty() && p[0] == cnic) ++count;
        }
    } catch (...) {}
    return count;
}

// ════════════════════════════════════════════════════════════════════════════
// CatalogFileManager
// ════════════════════════════════════════════════════════════════════════════

CatalogFileManager::CatalogFileManager(const std::string& file)
    : catalogFile(file) {}

void CatalogFileManager::loadCatalog(std::vector<Filter*>& filters) const {
    try {
        std::ifstream f(catalogFile);
        if (!f.is_open()) return;   // keep defaults
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;
            auto p = splitLine(line, '|');
            if (p.size() < 4) continue;
            std::string id      = p[0];
            bool        enabled = (p[3] == "1");
            for (Filter* fi : filters)
                if (fi->getId() == id)
                    fi->setEnabled(enabled);
        }
    } catch (const std::exception& e) {
        std::cerr << "[CatalogFileManager] loadCatalog error: " << e.what() << "\n";
    }
}

void CatalogFileManager::saveCatalog(const std::vector<Filter*>& filters) const {
    try {
        std::ofstream f(catalogFile, std::ios::trunc);
        if (!f.is_open()) {
            std::cerr << "[CatalogFileManager] Cannot open " << catalogFile << "\n";
            return;
        }
        f << "# FilterID|FilterName|Category|IsEnabled\n";
        for (const Filter* fi : filters)
            f << fi->getId() << "|" << fi->getName() << "|"
              << fi->getCategory() << "|" << (fi->isEnabled() ? "1" : "0") << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[CatalogFileManager] saveCatalog error: " << e.what() << "\n";
    }
}

void CatalogFileManager::ensureDefaultCatalog(const std::vector<Filter*>& filters) const {
    std::ifstream test(catalogFile);
    if (test.good()) return;   // already exists
    saveCatalog(filters);
}

// ════════════════════════════════════════════════════════════════════════════
// SessionFileManager
// ════════════════════════════════════════════════════════════════════════════

SessionFileManager::SessionFileManager(const std::string& file)
    : sessionsFile(file) {}

void SessionFileManager::appendSession(const std::string& cnic,
                                        const std::string& ts,
                                        const std::string& filtersApplied,
                                        const std::string& outFile) const {
    try {
        std::ofstream f(sessionsFile, std::ios::app);
        if (!f.is_open()) {
            std::cerr << "[SessionFileManager] Cannot open " << sessionsFile << "\n";
            return;
        }
        f << cnic << "|" << ts << "|" << filtersApplied << "|" << outFile << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[SessionFileManager] appendSession error: " << e.what() << "\n";
    }
}

std::vector<SessionFileManager::SessionRecord> SessionFileManager::loadAll() const {
    std::vector<SessionRecord> records;
    try {
        std::ifstream f(sessionsFile);
        if (!f.is_open()) return records;
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p = splitLine(line, '|');
            if (p.size() < 4) continue;
            records.push_back({p[0], p[1], p[2], p[3]});
        }
    } catch (const std::exception& e) {
        std::cerr << "[SessionFileManager] loadAll error: " << e.what() << "\n";
    }
    return records;
}

std::vector<SessionFileManager::SessionRecord>
SessionFileManager::loadForCnic(const std::string& cnic) const {
    auto all = loadAll();
    std::vector<SessionRecord> mine;
    for (const auto& r : all)
        if (r.cnic == cnic) mine.push_back(r);
    return mine;
}
