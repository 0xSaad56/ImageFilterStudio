#pragma once
#include <string>
#include <iostream>

// ─── Abstract Base Class ─────────────────────────────────────────────────────
class User {
protected:
    std::string cnic;
    std::string password;
    std::string fullName;
    std::string gender;
    std::string phone;
    std::string city;

public:
    User() = default;
    User(const std::string& cnic, const std::string& pw,
         const std::string& name,  const std::string& gender,
         const std::string& phone, const std::string& city)
        : cnic(cnic), password(pw), fullName(name),
          gender(gender), phone(phone), city(city) {}

    virtual ~User() {}

    // Pure virtual — forces subclasses to define their menu
    virtual void showMenu() = 0;
    virtual std::string getRole() const = 0;

    // Getters
    std::string getCnic()     const { return cnic;     }
    std::string getPassword() const { return password; }
    std::string getFullName() const { return fullName; }
    std::string getGender()   const { return gender;   }
    std::string getPhone()    const { return phone;    }
    std::string getCity()     const { return city;     }

    // Setters
    void setCnic(const std::string& c)     { this->cnic     = c; }
    void setPassword(const std::string& p) { this->password = p; }
    void setFullName(const std::string& n) { this->fullName = n; }
};

// ─── Admin ───────────────────────────────────────────────────────────────────
class Admin : public User {
public:
    Admin() : User("admin", "Admin1234", "Administrator", "M", "000", "HQ") {}
    void showMenu() override;
    std::string getRole() const override { return "Admin"; }
};

// ─── Customer ────────────────────────────────────────────────────────────────
class Customer : public User {
private:
    bool blocked;
    int  sessionsCompleted;

public:
    Customer() : blocked(false), sessionsCompleted(0) {}
    Customer(const std::string& cnic, const std::string& pw,
             const std::string& name,  const std::string& gender,
             const std::string& phone, const std::string& city,
             bool blocked = false, int sessions = 0)
        : User(cnic, pw, name, gender, phone, city),
          blocked(blocked), sessionsCompleted(sessions) {}

    void showMenu() override;
    std::string getRole() const override { return "Customer"; }

    bool isBlocked()          const { return blocked;            }
    int  getSessionsCompleted() const { return sessionsCompleted; }
    void setBlocked(bool b)        { this->blocked = b;          }
    void incrementSessions()       { ++sessionsCompleted;        }

    // Serialise to customers.txt format
    std::string toFileLine() const;
};
