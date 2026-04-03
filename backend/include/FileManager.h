#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "RegisteredUser.h"
#include "DietPlan.h"
#include <vector>
#include <string>

// ─────────────────────────────────────────────────────────────
//  FileManager Class
//  Demonstrates: Encapsulation, Single Responsibility Principle
//  All file I/O is centralised here.
// ─────────────────────────────────────────────────────────────
class FileManager {
private:
    std::string dataDir;        // e.g., "../../data"
    std::string usersFile;      // dataDir/users.txt
    std::string plansDir;       // dataDir/users/

    // ── Internal helpers ──
    std::string getUserPlanFile(const std::string& userID) const;
    bool        ensureDirectoryExists(const std::string& path) const;

public:
    explicit FileManager(const std::string& dataDirectory = "../../data");

    // ── User file operations ──
    bool saveUser(const RegisteredUser& user);
    bool updateUser(const RegisteredUser& user);
    bool userExists(const std::string& email)  const;
    bool loadUserByEmail(const std::string& email, RegisteredUser& out) const;
    bool loadUserByID(const std::string& userID, RegisteredUser& out)   const;
    std::vector<RegisteredUser> loadAllUsers() const;

    // ── Diet plan operations ──
    bool saveDietPlan(const DietPlan& plan);
    std::vector<DietPlan> loadUserPlans(const std::string& userID) const;
    bool planExists(const std::string& planID) const;

    // ── OTP storage (temp file) ──
    bool saveOTP(const std::string& email, const std::string& otp);
    bool validateOTP(const std::string& email, const std::string& otp);
    void clearOTP(const std::string& email);
};

#endif // FILE_MANAGER_H
