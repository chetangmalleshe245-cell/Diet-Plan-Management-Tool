#include "../include/RegisteredUser.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>

// ─── Helper: current date string ──────────────────────────────
static std::string currentDate() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    return std::string(buf);
}

// ─── Constructors ─────────────────────────────────────────────
RegisteredUser::RegisteredUser()
    : User(), registrationDate(currentDate()),
      totalPlansGenerated(0), currentCaloriesLogged(0.0) {}

RegisteredUser::RegisteredUser(const std::string& id,
                               const std::string& n,
                               int a,
                               double w,
                               double h,
                               const std::string& g,
                               const std::string& e,
                               const std::string& pwd)
    : User(id, n, a, w, h, g, e, pwd),
      registrationDate(currentDate()),
      totalPlansGenerated(0),
      currentCaloriesLogged(0.0) {}

// ─── displayInfo (Polymorphism override) ──────────────────────
void RegisteredUser::displayInfo() const {
    std::cout << "\n╔══════════════════════════════════╗\n";
    std::cout << "  User Profile\n";
    std::cout << "  ID       : " << userID          << "\n";
    std::cout << "  Name     : " << name            << "\n";
    std::cout << "  Age      : " << age             << " yrs\n";
    std::cout << "  Weight   : " << weight          << " kg\n";
    std::cout << "  Height   : " << height          << " cm\n";
    std::cout << "  Goal     : " << goal            << "\n";
    std::cout << "  Email    : " << email           << "\n";
    std::cout << "  BMI      : " << std::fixed
              << std::setprecision(1) << calculateBMI()
              << " (" << getBMICategory() << ")\n";
    std::cout << "  Target   : " << (int)calculateDailyCalories()
              << " kcal/day\n";
    std::cout << "  Reg. Date: " << registrationDate << "\n";
    std::cout << "  Plans    : " << totalPlansGenerated << "\n";
    std::cout << "╚══════════════════════════════════╝\n";
}

// ─── Serialise (extends parent) ───────────────────────────────
std::string RegisteredUser::serialize() const {
    // Format: base|registrationDate|totalPlans|currentCal
    std::ostringstream oss;
    oss << User::serialize()   << "|"
        << registrationDate    << "|"
        << totalPlansGenerated << "|"
        << std::fixed << std::setprecision(2)
        << currentCaloriesLogged;
    return oss.str();
}

// ─── Deserialise ──────────────────────────────────────────────
bool RegisteredUser::deserialize(const std::string& line) {
    // Split all tokens by '|'
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|'))
        tokens.push_back(token);

    // Need at least 11 tokens (8 base + 3 extra)
    if (tokens.size() < 11) return false;

    // Rebuild base portion
    std::string baseLine =
        tokens[0] + "|" + tokens[1] + "|" + tokens[2] + "|" +
        tokens[3] + "|" + tokens[4] + "|" + tokens[5] + "|" +
        tokens[6] + "|" + tokens[7];

    if (!User::deserialize(baseLine)) return false;

    registrationDate      = tokens[8];
    totalPlansGenerated   = std::stoi(tokens[9]);
    currentCaloriesLogged = std::stod(tokens[10]);
    return true;
}
