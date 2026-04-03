#include "../include/DietManager.h"
#include <sstream>
#include <ctime>
#include <iomanip>

// ─── Constructor ──────────────────────────────────────────────
DietManager::DietManager(FileManager& fm) : fileManager(fm) {}

// ─── Generate Plan ID ─────────────────────────────────────────
std::string DietManager::generatePlanID(const std::string& userID) const {
    time_t now = time(nullptr);
    std::ostringstream oss;
    oss << "PLAN_" << userID.substr(0, 8) << "_" << now;
    return oss.str();
}

// ─── Generate a new Diet Plan ─────────────────────────────────
DietPlan DietManager::generatePlan(RegisteredUser& user) {
    std::string pid    = generatePlanID(user.getUserID());
    double      tCal   = user.calculateDailyCalories();

    DietPlan plan(pid, user.getUserID(), user.getGoal(), tCal);
    plan.generate();

    fileManager.saveDietPlan(plan);
    user.incrementPlanCount();
    fileManager.updateUser(user);

    return plan;
}

// ─── Get User History ─────────────────────────────────────────
std::vector<DietPlan> DietManager::getUserHistory(const std::string& uid) {
    return fileManager.loadUserPlans(uid);
}

// ─── Log Calories ─────────────────────────────────────────────
bool DietManager::logCalories(RegisteredUser& user,
                              double calories,
                              std::string& errMsg) {
    if (calories < 0 || calories > 10000) {
        errMsg = "Invalid calorie amount.";
        return false;
    }
    user.logCalories(calories);
    fileManager.updateUser(user);
    errMsg = "";
    return true;
}

// ─── Progress JSON ────────────────────────────────────────────
std::string DietManager::getProgressJSON(const RegisteredUser& user) {
    double bmi      = user.calculateBMI();
    double target   = user.calculateDailyCalories();
    double logged   = user.getCurrentCaloriesLogged();
    double remain   = target - logged;

    std::ostringstream j;
    j << "{";
    j << "\"userID\":\""     << user.getUserID()     << "\",";
    j << "\"name\":\""       << user.getName()       << "\",";
    j << "\"goal\":\""       << user.getGoal()       << "\",";
    j << "\"bmi\":"          << std::fixed << std::setprecision(1) << bmi << ",";
    j << "\"bmiCategory\":\"" << user.getBMICategory() << "\",";
    j << "\"targetCalories\":"<< (int)target  << ",";
    j << "\"loggedCalories\":"<< (int)logged  << ",";
    j << "\"remainingCalories\":"<< (int)remain << ",";
    j << "\"weight\":"        << user.getWeight() << ",";
    j << "\"height\":"        << user.getHeight() << ",";
    j << "\"totalPlans\":"    << user.getTotalPlansGenerated();
    j << "}";
    return j.str();
}
