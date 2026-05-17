#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "Pixel.h"
#include "Image.h"
#include "Filter.h"
#include "Filters.h"
#include "FilterSession.h"
#include "User.h"
#include "FileManager.h"
int Filter::filterCount = 0;

// ─── Globals ─────────────────────────────────────────────────────────────────

static std::vector<Filter*> g_catalog;
static CustomerFileManager  g_custMgr;
static CatalogFileManager   g_catMgr;
static SessionFileManager   g_sessMgr;

// ─── Utility ─────────────────────────────────────────────────────────────────

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static std::string getTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm*    tm  = std::localtime(&now);
    std::ostringstream oss;
    oss << std::setfill('0')
        << (1900 + tm->tm_year)
        << std::setw(2) << (1 + tm->tm_mon)
        << std::setw(2) << tm->tm_mday << "_"
        << std::setw(2) << tm->tm_hour
        << std::setw(2) << tm->tm_min
        << std::setw(2) << tm->tm_sec;
    return oss.str();
}


static void printBox(const std::string& title, const std::vector<std::string>& items) {
    const int W = 46;
    std::string hline(W-2, '-');
    std::cout << "\n+" << hline << "+\n";
    int pad = (W - 2 - (int)title.size()) / 2;
    if (pad < 0) pad = 0;
    int rpad = W - 2 - pad - (int)title.size();
    if (rpad < 0) rpad = 0;
    std::cout << "|" << std::string(pad,' ') << title << std::string(rpad,' ') << "|\n";
    std::cout << "+" << hline << "+\n";
    for (const auto& item : items) {
        int sp = W - 3 - (int)item.size();
        if (sp < 0) sp = 0;
        std::cout << "| " << item << std::string(sp,' ') << "|\n";
    }
    std::cout << "+" << hline << "+\n";
}

// ─── Validation ──────────────────────────────────────────────────────────────

static bool validateCnic(const std::string& cnic) {
    if (cnic.size() != 13) return false;
    for (char c : cnic) if (!isdigit(c)) return false;
    return true;
}

static bool validatePassword(const std::string& pw) {
    if (pw.size() != 9) return false;
    bool hasUpper = false, hasDigit = false;
    for (char c : pw) {
        if (isupper(c)) hasUpper = true;
        if (isdigit(c)) hasDigit = true;
    }
    return hasUpper && hasDigit;
}

// ─── Catalog Helpers ─────────────────────────────────────────────────────────

static void buildDefaultCatalog() {
    g_catalog.push_back(new GrayscaleFilter());
    g_catalog.push_back(new InvertFilter());
    g_catalog.push_back(new BrightnessFilter());
    g_catalog.push_back(new ContrastStretchFilter());
    g_catalog.push_back(new RedChannelFilter());
    g_catalog.push_back(new GreenChannelFilter());
    g_catalog.push_back(new BlueChannelFilter());
    g_catalog.push_back(new BoxBlurFilter());
    g_catalog.push_back(new FlipHorizontalFilter());
    g_catalog.push_back(new FlipVerticalFilter());
}

static void freeCatalog() {
    for (Filter* f : g_catalog) delete f;
    g_catalog.clear();
}

static Filter* findFilterById(const std::string& id) {
    for (Filter* f : g_catalog)
        if (f->getId() == id) return f;
    return nullptr;
}

static void printCatalog() {
    std::cout << "\n  ID  | Name                    | Category         | Status\n";
    std::cout << "  ----|-------------------------|------------------|--------\n";
    for (const Filter* f : g_catalog) {
        std::cout << "  " << std::setw(2) << f->getId() << "  | "
                  << std::left << std::setw(23) << f->getName() << " | "
                  << std::setw(16) << f->getCategory() << " | "
                  << (f->isEnabled() ? "Enabled" : "Disabled") << "\n"
                  << std::right;
    }
    std::cout << "\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// ADMIN PANEL
// ═════════════════════════════════════════════════════════════════════════════

static void adminManageCatalog() {
    bool back = false;
    while (!back) {
        printBox("MANAGE FILTER CATALOG", {
            "1. View Filter Catalog",
            "2. Enable a Filter",
            "3. Disable a Filter",
            "4. Back"
        });
        std::cout << "Your choice: ";
        int ch; std::cin >> ch; clearInput();
        switch (ch) {
        case 1:
            printCatalog();
            break;
        case 2: case 3: {
            printCatalog();
            std::cout << "Enter Filter ID (01-10): ";
            std::string id; std::getline(std::cin, id);
            // Zero-pad if single digit
            if (id.size() == 1) id = "0" + id;
            Filter* f = findFilterById(id);
            if (!f) { std::cout << "Filter not found.\n"; break; }
            f->setEnabled(ch == 2);
            g_catMgr.saveCatalog(g_catalog);
            std::cout << "Filter '" << f->getName() << "' is now "
                      << (f->isEnabled() ? "Enabled" : "Disabled") << ".\n";
            break;
        }
        case 4: back = true; break;
        default: std::cout << "Invalid choice.\n";
        }
    }
}

static void adminManageCustomers() {
    bool back = false;
    while (!back) {
        printBox("MANAGE CUSTOMERS", {
            "1. View All Customers",
            "2. Search by CNIC",
            "3. Search by Name",
            "4. Block Customer",
            "5. Delete Customer",
            "6. Back"
        });
        std::cout << "Your choice: ";
        int ch; std::cin >> ch; clearInput();

        auto customers = g_custMgr.loadAll();

        switch (ch) {
        case 1: {
            if (customers.empty()) { std::cout << "No customers registered.\n"; break; }
            std::cout << "\n"
                      << std::setw(14) << std::left << "CNIC" << " | "
                      << std::setw(20) << "Name" << " | "
                      << std::setw(6)  << "Gender" << " | "
                      << std::setw(12) << "Phone" << " | "
                      << std::setw(12) << "City" << " | "
                      << "Blocked\n";
            std::cout << std::string(80, '-') << "\n";
            for (const auto& c : customers)
                std::cout << std::setw(14) << c.getCnic()    << " | "
                          << std::setw(20) << c.getFullName() << " | "
                          << std::setw(6)  << c.getGender()  << " | "
                          << std::setw(12) << c.getPhone()   << " | "
                          << std::setw(12) << c.getCity()    << " | "
                          << (c.isBlocked() ? "YES" : "NO")  << "\n";
            std::cout << std::right;
            break;
        }
        case 2: {
            std::cout << "Enter CNIC: ";
            std::string cnic; std::getline(std::cin, cnic);
            Customer* found = g_custMgr.findByCnic(cnic, customers);
            if (!found) std::cout << "Customer not found.\n";
            else std::cout << "Found: " << found->getFullName()
                           << " | " << found->getCity()
                           << " | Blocked: " << (found->isBlocked() ? "YES":"NO") << "\n";
            break;
        }
        case 3: {
            std::cout << "Enter name (partial): ";
            std::string name; std::getline(std::cin, name);
            std::string nameLower = name;
            for (auto& c : nameLower) c = tolower(c);
            bool any = false;
            for (const auto& c : customers) {
                std::string fn = c.getFullName();
                for (auto& ch2 : fn) ch2 = tolower(ch2);
                if (fn.find(nameLower) != std::string::npos) {
                    std::cout << c.getCnic() << " | " << c.getFullName() << "\n";
                    any = true;
                }
            }
            if (!any) std::cout << "No matching customers.\n";
            break;
        }
        case 4: {
            std::cout << "Enter CNIC to block: ";
            std::string cnic; std::getline(std::cin, cnic);
            if (g_custMgr.blockCustomer(cnic))
                std::cout << "Customer blocked successfully.\n";
            else
                std::cout << "Customer not found.\n";
            break;
        }
        case 5: {
            std::cout << "Enter CNIC to delete: ";
            std::string cnic; std::getline(std::cin, cnic);
            if (g_custMgr.deleteCustomer(cnic))
                std::cout << "Customer deleted successfully.\n";
            else
                std::cout << "Customer not found.\n";
            break;
        }
        case 6: back = true; break;
        default: std::cout << "Invalid choice.\n";
        }
    }
}

static void adminViewSessions() {
    auto sessions = g_sessMgr.loadAll();
    if (sessions.empty()) { std::cout << "No sessions recorded yet.\n"; return; }
    std::cout << "\n"
              << std::setw(14) << std::left << "CNIC" << " | "
              << std::setw(16) << "Timestamp" << " | "
              << std::setw(30) << "Filters Applied" << " | "
              << "Output File\n";
    std::cout << std::string(90, '-') << "\n";
    for (const auto& s : sessions)
        std::cout << std::setw(14) << s.cnic          << " | "
                  << std::setw(16) << s.timestamp     << " | "
                  << std::setw(30) << s.filtersApplied << " | "
                  << s.outputFile  << "\n";
    std::cout << std::right;
}

static void runAdminPanel() {
    bool loggedIn = true;
    while (loggedIn) {
        printBox("ADMIN PANEL : Image Filter Studio", {
            "1. Manage Filter Catalog",
            "2. Manage Customers",
            "3. View All Sessions",
            "4. Logout"
        });
        std::cout << "Your choice: ";
        int ch; std::cin >> ch; clearInput();
        switch (ch) {
        case 1: adminManageCatalog();  break;
        case 2: adminManageCustomers(); break;
        case 3: adminViewSessions();   break;
        case 4: loggedIn = false; std::cout << "Logged out.\n"; break;
        default: std::cout << "Invalid choice.\n";
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// CUSTOMER PANEL
// ═════════════════════════════════════════════════════════════════════════════

static void buildPipeline(FilterSession& session) {
    std::cout << "\n=== Build Filter Pipeline ===\n";

    // Show available (enabled) filters
    std::cout << "\nAvailable Filters:\n";
    for (const Filter* f : g_catalog) {
        std::cout << "  " << f->getId() << " " << std::left << std::setw(24) << f->getName()
                  << " [" << f->getCategory() << "] "
                  << (f->isEnabled() ? "Enabled" : "Disabled") << "\n" << std::right;
    }
    std::cout << "\nCurrent pipeline: (empty)\n";

    bool done = false;
    while (!done) {
        std::cout << "\nEnter filter ID to add (0 to finish): ";
        std::string input; std::getline(std::cin, input);
        if (input == "0") { done = true; break; }
        // Zero-pad
        if (input.size() == 1) input = "0" + input;
        Filter* found = findFilterById(input);
        if (!found) { std::cout << "Filter not found.\n"; continue; }
        if (!found->isEnabled()) { std::cout << "That filter is disabled by admin.\n"; continue; }

        // Cloned filter (so we own it in the pipeline)
        Filter* f = found->clone();

        // Special handling for Brightness (needs amount)
        if (input == "03") {
            int amount = 30;
            std::cout << "Enter brightness amount (-255 to +255): ";
            std::cin >> amount; clearInput();
            amount = Pixel::clamp(amount, -255, 255);
            dynamic_cast<BrightnessFilter*>(f)->setAmount(amount);
        }

        // Method chaining — addFilter returns *this
        session.addFilter(f);
        std::cout << "Added: " << f->getName() << "\n";

        // Show current pipeline
        std::cout << "Pipeline: [ ";
        const auto& pl = session.getPipeline();
        for (size_t i = 0; i < pl.size(); ++i) {
            if (i) std::cout << " -> ";
            std::cout << pl[i]->getName();
            if (pl[i]->getId() == "03")
                std::cout << "(" << dynamic_cast<BrightnessFilter*>(pl[i])->getAmount() << ")";
        }
        std::cout << " ]\n";
    }

    if (session.getPipeline().empty())
        std::cout << "Pipeline is empty.\n";
    else
        std::cout << "Pipeline finalised. " << session.getPipeline().size()
                  << " filter(s) ready to apply.\n";
}

static void runCustomerPanel(Customer& customer) {
    bool loggedIn  = true;
    FilterSession* session = nullptr;

    // Create a new session for this login
    std::string ts = getTimestamp();
    session = new FilterSession(customer.getCnic(), ts);

    while (loggedIn) {
        int completedSessions = g_custMgr.countSessions(customer.getCnic());

        printBox("Welcome, " + customer.getFullName(), {
            "Sessions completed: " + std::to_string(completedSessions),
            "─────────────────────────────────────────",
            "1. Browse Filter Catalog",
            "2. Load Image",
            "3. Build Filter Pipeline",
            "4. Apply Pipeline & Save Result",
            "5. View My Session History",
            "6. Logout"
        });
        std::cout << "Your choice: ";
        int ch; std::cin >> ch; clearInput();

        switch (ch) {

        case 1:
            printCatalog();
            break;

        case 2: {
            std::cout << "\n=== Load Image ===\n"
                      << "  1. Load from JPG/PNG file\n"
                      << "  2. Generate test pattern\n"
                      << "Choice: ";
            int sub; std::cin >> sub; clearInput();

            if (sub == 1) {
                std::cout << "Enter full path to image file: ";
                std::string path; std::getline(std::cin, path);
                std::cout << "Loading image file...\n";
                if (session->loadImage(path)) {
                    Image* img = session->getImage();
                    std::cout << "Image loaded: " << img->getWidth() << " x "
                              << img->getHeight() << " pixels ("
                              << (img->getWidth() * img->getHeight()) << " total)\n";
                    img->displayASCII();
                    // Demo friend access
                    session->demoFriendAccess();
                } else {
                    std::cout << "Failed to load image. Try test pattern instead.\n";
                }
            } else {
                std::cout << "Generating test pattern (320x240)...\n";
                session->useTestPattern(320, 240);
                std::cout << "Test pattern created.\n";
                session->getImage()->displayASCII();
            }
            break;
        }

        case 3: {
            if (!session->getImage()) {
                std::cout << "Please load an image first (option 2).\n";
                break;
            }
            // Clear old pipeline if any
            session->clearPipeline();
            buildPipeline(*session);
            break;
        }

        case 4: {
            if (!session->getImage()) {
                std::cout << "No image loaded. Use option 2 first.\n";
                break;
            }
            if (session->getPipeline().empty()) {
                std::cout << "No pipeline built. Use option 3 first.\n";
                break;
            }
            session->applyPipeline();

            std::cout << "\nSave result? (y/n): ";
            char yn; std::cin >> yn; clearInput();
            if (yn == 'y' || yn == 'Y') {
                std::cout << "Save as (1) PNG  (2) JPG: ";
                int fmt; std::cin >> fmt; clearInput();
                std::string ext = (fmt == 2) ? "jpg" : "png";
                std::string outFile = session->saveResult(ext);
                if (!outFile.empty()) {
                    // Record session
                    g_sessMgr.appendSession(
                        customer.getCnic(), ts,
                        session->getPipelineString(), outFile);
                    std::cout << "Session recorded in sessions.txt.\n"
                              << "Open '" << outFile << "' in any image viewer to see the result.\n";

                    // Update session count in customers.txt
                    auto customers = g_custMgr.loadAll();
                    for (auto& c : customers) {
                        if (c.getCnic() == customer.getCnic()) {
                            c.incrementSessions();
                            break;
                        }
                    }
                    g_custMgr.saveAll(customers);

                    // Start new session for next run
                    delete session;
                    ts      = getTimestamp();
                    session = new FilterSession(customer.getCnic(), ts);
                }
            }
            break;
        }

        case 5: {
            auto history = g_sessMgr.loadForCnic(customer.getCnic());
            if (history.empty()) {
                std::cout << "No sessions yet.\n";
            } else {
                std::cout << "\n=== Your Session History ===\n";
                for (size_t i = 0; i < history.size(); ++i) {
                    const auto& s = history[i];
                    std::cout << (i+1) << ". [" << s.timestamp << "] "
                              << s.filtersApplied << " → " << s.outputFile << "\n";
                }
            }
            break;
        }

        case 6:
            loggedIn = false;
            std::cout << "Logged out. Goodbye, " << customer.getFullName() << "!\n";
            break;

        default:
            std::cout << "Invalid choice.\n";
        }
    }

    delete session;
}

// ═════════════════════════════════════════════════════════════════════════════
// REGISTRATION
// ═════════════════════════════════════════════════════════════════════════════

static void registerCustomer() {
    std::cout << "\n╔════════════════════════════════════════════╗\n"
              << "║          NEW CUSTOMER REGISTRATION          ║\n"
              << "╚════════════════════════════════════════════╝\n\n";

    std::string cnic, password, confirm, fullName, gender, phone, city;

    // ── CNIC ──────────────────────────────────────────────────────────────
    while (true) {
        std::cout << "Enter CNIC (13 digits, no dashes): ";
        std::getline(std::cin, cnic);
        if (!validateCnic(cnic)) {
            std::cout << "  [!] CNIC must be exactly 13 digits.\n";
            continue;
        }
        if (g_custMgr.isCnicBlocked(cnic)) {
            std::cout << "  [!] This CNIC is permanently banned.\n";
            return;
        }
        // Check duplicates
        auto customers = g_custMgr.loadAll();
        bool dup = false;
        for (const auto& c : customers)
            if (c.getCnic() == cnic) { dup = true; break; }
        if (dup) { std::cout << "  [!] CNIC already registered.\n"; continue; }
        break;
    }

    // ── Password ──────────────────────────────────────────────────────────
    while (true) {
        std::cout << "Password (9 chars, 1 uppercase, 1 digit): ";
        std::getline(std::cin, password);
        if (!validatePassword(password)) {
            std::cout << "  [!] Password must be exactly 9 characters with at least 1 uppercase and 1 digit.\n";
            continue;
        }
        std::cout << "Confirm password: ";
        std::getline(std::cin, confirm);
        if (confirm != password) {
            std::cout << "  [!] Passwords do not match.\n";
            continue;
        }
        break;
    }

    std::cout << "Full Name: ";           std::getline(std::cin, fullName);
    std::cout << "Gender (M/F/Other): ";  std::getline(std::cin, gender);
    std::cout << "Phone Number: ";        std::getline(std::cin, phone);
    std::cout << "City: ";                std::getline(std::cin, city);

    Customer c(cnic, password, fullName, gender, phone, city, false, 0);
    try {
        g_custMgr.appendCustomer(c);
        std::cout << "\n  ✓ Registration successful! You can now log in.\n";
    } catch (const std::exception& e) {
        std::cerr << "  [!] Registration failed: " << e.what() << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// LOGIN
// ═════════════════════════════════════════════════════════════════════════════

static void adminLogin() {
    Admin admin;
    std::cout << "\n=== Admin Login ===\n";
    std::cout << "Username: ";
    std::string user; std::getline(std::cin, user);
    std::cout << "Password: ";
    std::string pw; std::getline(std::cin, pw);
    if (user == "admin" && pw == admin.getPassword()) {
        std::cout << "Admin login successful.\n";
        runAdminPanel();
    } else {
        std::cout << "Invalid admin credentials.\n";
    }
}

static void customerLogin() {
    std::cout << "\n=== Customer Login ===\n";
    int attempts = 0;
    while (attempts < 3) {
        std::cout << "CNIC: ";
        std::string cnic; std::getline(std::cin, cnic);
        std::cout << "Password: ";
        std::string pw; std::getline(std::cin, pw);

        // Check blocked list first
        if (g_custMgr.isCnicBlocked(cnic)) {
            std::cout << "  [!] This account has been permanently banned by the administrator.\n";
            return;
        }

        auto customers = g_custMgr.loadAll();
        Customer* found = nullptr;
        for (auto& c : customers)
            if (c.getCnic() == cnic) { found = &c; break; }

        if (!found) {
            std::cout << "  [!] CNIC not found. Attempt " << (++attempts) << "/3.\n";
        } else if (found->isBlocked()) {
            std::cout << "  [!] Your account has been blocked by the administrator.\n";
            return;
        } else if (found->getPassword() != pw) {
            std::cout << "  [!] Incorrect password. Attempt " << (++attempts) << "/3.\n";
        } else {
            std::cout << "\n  Login successful! Welcome, " << found->getFullName() << ".\n";
            // Make a local copy (so we can modify it)
            Customer loggedIn = *found;
            runCustomerPanel(loggedIn);
            return;
        }
    }
    std::cout << "  [!] Too many failed attempts. Returning to main menu.\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "\n=== Initialising Image Filter Studio ===\n";

    // Build filter catalog (constructors print trace)
    buildDefaultCatalog();

    // Load/create catalog.txt
    g_catMgr.ensureDefaultCatalog(g_catalog);
    g_catMgr.loadCatalog(g_catalog);

    bool running = true;
    while (running) {
        printBox("IMAGE FILTER STUDIO", {
            "1. Admin Login",
            "2. Customer Login",
            "3. New Customer? Register here",
            "4. Exit"
        });
        std::cout << "Your choice: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }
        clearInput();

        switch (choice) {
        case 1: adminLogin();       break;
        case 2: customerLogin();    break;
        case 3: registerCustomer(); break;
        case 4:
            running = false;
            std::cout << "\nShutting down...\n";
            break;
        default:
            std::cout << "Invalid choice. Try again.\n";
        }
    }

    // Destructors print trace
    freeCatalog();
    std::cout << "\nGoodbye!\n";
    return 0;
}