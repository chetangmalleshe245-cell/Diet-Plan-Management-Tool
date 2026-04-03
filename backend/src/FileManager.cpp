#include "../include/FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include <algorithm>

// ─── Constructor ──────────────────────────────────────────────
FileManager::FileManager(const std::string& dir)
    : dataDir(dir),
      usersFile(dir + "/users.txt"),
      plansDir(dir + "/users") {
    ensureDirectoryExists(dataDir);
    ensureDirectoryExists(plansDir);
}

// ─── Directory helper ─────────────────────────────────────────
bool FileManager::ensureDirectoryExists(const std::string& path) const {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) return true;
#ifdef _WIN32
    return (_mkdir(path.c_str()) == 0);
#else
    return (mkdir(path.c_str(), 0755) == 0);
#endif
}

std::string FileManager::getUserPlanFile(const std::string& uid) const {
    return plansDir + "/" + uid + "_plans.txt";
}

// ─── Save User ────────────────────────────────────────────────
bool FileManager::saveUser(const RegisteredUser& user) {
    std::ofstream ofs(usersFile, std::ios::app);
    if (!ofs.is_open()) return false;
    ofs << user.serialize() << "\n";
    return true;
}

// ─── Update User (rewrite entire file) ───────────────────────
bool FileManager::updateUser(const RegisteredUser& updated) {
    std::vector<RegisteredUser> all = loadAllUsers();
    std::ofstream ofs(usersFile, std::ios::trunc);
    if (!ofs.is_open()) return false;
    for (auto& u : all) {
        if (u.getUserID() == updated.getUserID())
            ofs << updated.serialize() << "\n";
        else
            ofs << u.serialize() << "\n";
    }
    return true;
}

// ─── Check email existence ────────────────────────────────────
bool FileManager::userExists(const std::string& email) const {
    std::ifstream ifs(usersFile);
    if (!ifs.is_open()) return false;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        // email is token index 6
        std::istringstream ss(line);
        std::string tok;
        int idx = 0;
        while (std::getline(ss, tok, '|')) {
            if (idx == 6 && tok == email) return true;
            ++idx;
        }
    }
    return false;
}

// ─── Load by email ────────────────────────────────────────────
bool FileManager::loadUserByEmail(const std::string& email,
                                  RegisteredUser& out) const {
    std::ifstream ifs(usersFile);
    if (!ifs.is_open()) return false;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        RegisteredUser u;
        if (u.deserialize(line) && u.getEmail() == email) {
            out = u;
            return true;
        }
    }
    return false;
}

// ─── Load by ID ───────────────────────────────────────────────
bool FileManager::loadUserByID(const std::string& userID,
                               RegisteredUser& out) const {
    std::ifstream ifs(usersFile);
    if (!ifs.is_open()) return false;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        RegisteredUser u;
        if (u.deserialize(line) && u.getUserID() == userID) {
            out = u;
            return true;
        }
    }
    return false;
}

// ─── Load all users ───────────────────────────────────────────
std::vector<RegisteredUser> FileManager::loadAllUsers() const {
    std::vector<RegisteredUser> result;
    std::ifstream ifs(usersFile);
    if (!ifs.is_open()) return result;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        RegisteredUser u;
        if (u.deserialize(line))
            result.push_back(u);
    }
    return result;
}

// ─── Save Diet Plan ───────────────────────────────────────────
bool FileManager::saveDietPlan(const DietPlan& plan) {
    std::string file = getUserPlanFile(plan.getUserID());
    std::ofstream ofs(file, std::ios::app);
    if (!ofs.is_open()) return false;
    ofs << plan.serialize();
    return true;
}

// ─── Load User Plans ─────────────────────────────────────────
std::vector<DietPlan> FileManager::loadUserPlans(const std::string& uid) const {
    std::vector<DietPlan> result;
    std::string file = getUserPlanFile(uid);
    std::ifstream ifs(file);
    if (!ifs.is_open()) return result;

    std::string line, block;
    while (std::getline(ifs, line)) {
        block += line + "\n";
        if (line == "END") {
            DietPlan dp;
            if (dp.deserialize(block))
                result.push_back(dp);
            block.clear();
        }
    }
    return result;
}

bool FileManager::planExists(const std::string& planID) const {
    // Scan all plan files — O(n) but fine for file-based project
    // In practice would require an index file
    return false;  // simplified
}

// ─── OTP Storage ─────────────────────────────────────────────
// Format: email|otp|timestamp  in data/otps.txt
bool FileManager::saveOTP(const std::string& email, const std::string& otp) {
    std::string otpFile = dataDir + "/otps.txt";
    // Remove old entry first
    clearOTP(email);
    std::ofstream ofs(otpFile, std::ios::app);
    if (!ofs.is_open()) return false;
    time_t now = time(nullptr);
    ofs << email << "|" << otp << "|" << now << "\n";
    return true;
}

bool FileManager::validateOTP(const std::string& email,
                              const std::string& otp) {
    std::string otpFile = dataDir + "/otps.txt";
    std::ifstream ifs(otpFile);
    if (!ifs.is_open()) return false;
    std::string line;
    time_t now = time(nullptr);
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string e, o, ts;
        std::getline(ss, e, '|');
        std::getline(ss, o, '|');
        std::getline(ss, ts, '|');
        if (e == email && o == otp) {
            time_t saved = (time_t)std::stoll(ts);
            // 5-minute expiry (300 seconds)
            return (now - saved) <= 300;
        }
    }
    return false;
}

void FileManager::clearOTP(const std::string& email) {
    std::string otpFile = dataDir + "/otps.txt";
    std::ifstream ifs(otpFile);
    if (!ifs.is_open()) return;
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        std::string e = line.substr(0, line.find('|'));
        if (e != email) lines.push_back(line);
    }
    ifs.close();
    std::ofstream ofs(otpFile, std::ios::trunc);
    for (auto& l : lines) ofs << l << "\n";
}
