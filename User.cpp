#include "User.h"
#include <sstream>

// ─── Customer::toFileLine ────────────────────────────────────────────────────
// Format: CNIC|Password|FullName|Gender|Phone|City|IsBlocked|Sessions

std::string Customer::toFileLine() const {
    std::ostringstream oss;
    oss << cnic     << "|"
        << password << "|"
        << fullName << "|"
        << gender   << "|"
        << phone    << "|"
        << city     << "|"
        << (blocked ? "1" : "0") << "|"
        << sessionsCompleted;
    return oss.str();
}

// showMenu() implementations are in main.cpp (forward-declared here)
void Admin::showMenu()    { /* handled by main menu loop */ }
void Customer::showMenu() { /* handled by main menu loop */ }
