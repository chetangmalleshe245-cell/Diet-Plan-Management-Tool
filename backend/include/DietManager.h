#ifndef DIET_MANAGER_H
#define DIET_MANAGER_H

#include "DietPlan.h"
#include "RegisteredUser.h"
#include "FileManager.h"
#include <vector>
#include <string>

// ─────────────────────────────────────────────────────────────
//  DietManager Class
//  Demonstrates: Abstraction, Encapsulation
//  Orchestrates diet plan generation and retrieval
// ─────────────────────────────────────────────────────────────
class DietManager {
private:
    FileManager& fileManager;
    std::string  generatePlanID(const std::string& userID) const;

public:
    explicit DietManager(FileManager& fm);

    // Generate a new diet plan for the user
    DietPlan generatePlan(RegisteredUser& user);

    // Retrieve all plans for a user
    std::vector<DietPlan> getUserHistory(const std::string& userID);

    // Log calories for today
    bool logCalories(RegisteredUser& user, double calories, std::string& errMsg);

    // Get progress summary as JSON string
    std::string getProgressJSON(const RegisteredUser& user);
};

#endif // DIET_MANAGER_H
