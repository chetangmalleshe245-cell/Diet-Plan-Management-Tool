#include "../include/DietPlan.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <numeric>

// ─── Helper ───────────────────────────────────────────────────
static std::string todayStr() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    return std::string(buf);
}

// ─── Constructors ─────────────────────────────────────────────
DietPlan::DietPlan()
    : planID(""), userID(""), goal("maintain"),
      generatedDate(todayStr()), targetCalories(2000.0) {}

DietPlan::DietPlan(const std::string& pid,
                   const std::string& uid,
                   const std::string& g,
                   double cal)
    : planID(pid), userID(uid), goal(g),
      generatedDate(todayStr()), targetCalories(cal) {}

// ─── Macros for adding a Meal ──────────────────────────────────
static Meal makeMeal(const std::string& type,
                     const std::string& foods,
                     double cal, double pro, double carb, double fat) {
    return {type, foods, cal, pro, carb, fat};
}

// ─────────────────────────────────────────────────────────────
//  WEIGHT LOSS PLAN  (~1400-1700 kcal, high protein, low carb)
// ─────────────────────────────────────────────────────────────
void DietPlan::buildWeightLossPlan(double tCal) {
    meals.clear();
    notes = "High protein, caloric deficit. Drink 3-4L water daily. Avoid sugar.";

    // Breakfast ~25%
    meals.push_back(makeMeal("Breakfast",
        "Oats with skimmed milk, 2 boiled eggs, black coffee",
        tCal * 0.25, 25, 40, 8));

    // Mid-morning ~10%
    meals.push_back(makeMeal("Mid-Morning Snack",
        "1 apple, handful of almonds (10 pcs)",
        tCal * 0.10, 5, 20, 8));

    // Lunch ~35%
    meals.push_back(makeMeal("Lunch",
        "Grilled chicken breast 150g, 1 cup brown rice, cucumber salad",
        tCal * 0.35, 40, 55, 10));

    // Evening snack ~10%
    meals.push_back(makeMeal("Evening Snack",
        "Greek yogurt (low-fat), 1 banana",
        tCal * 0.10, 12, 25, 3));

    // Dinner ~20%
    meals.push_back(makeMeal("Dinner",
        "Paneer tikka 100g / Fish 150g, steamed broccoli, 2 chapatis",
        tCal * 0.20, 30, 30, 12));
}

// ─────────────────────────────────────────────────────────────
//  WEIGHT GAIN PLAN  (~2700-3200 kcal, high protein + carbs)
// ─────────────────────────────────────────────────────────────
void DietPlan::buildWeightGainPlan(double tCal) {
    meals.clear();
    notes = "Caloric surplus with high protein. Focus on compound lifts. Sleep 8hrs.";

    meals.push_back(makeMeal("Breakfast",
        "4 whole eggs, 2 slices whole wheat toast, peanut butter, banana shake",
        tCal * 0.25, 35, 75, 20));

    meals.push_back(makeMeal("Mid-Morning Snack",
        "Mass gainer shake / whole milk 400ml, mixed nuts",
        tCal * 0.15, 20, 60, 15));

    meals.push_back(makeMeal("Lunch",
        "Chicken/mutton curry 200g, 2 cups white rice, dal, salad",
        tCal * 0.30, 50, 90, 18));

    meals.push_back(makeMeal("Pre-Workout Snack",
        "Sweet potato 150g, 2 boiled eggs",
        tCal * 0.10, 15, 35, 6));

    meals.push_back(makeMeal("Dinner",
        "Salmon/chicken 200g, pasta / quinoa 1 cup, veggies, olive oil",
        tCal * 0.20, 45, 65, 20));
}

// ─────────────────────────────────────────────────────────────
//  MAINTENANCE PLAN  (balanced macros)
// ─────────────────────────────────────────────────────────────
void DietPlan::buildMaintenancePlan(double tCal) {
    meals.clear();
    notes = "Balanced macros for sustainable health. Stay hydrated. Limit processed food.";

    meals.push_back(makeMeal("Breakfast",
        "2 eggs, 1 cup oats, fresh fruit, green tea",
        tCal * 0.25, 20, 50, 10));

    meals.push_back(makeMeal("Mid-Morning Snack",
        "Fruit salad, handful of walnuts",
        tCal * 0.10, 5, 25, 8));

    meals.push_back(makeMeal("Lunch",
        "Dal rice / chapati sabzi, curd, salad",
        tCal * 0.35, 25, 70, 10));

    meals.push_back(makeMeal("Evening Snack",
        "Sprouts chaat / roasted makhana, coconut water",
        tCal * 0.10, 8, 20, 3));

    meals.push_back(makeMeal("Dinner",
        "Vegetable khichdi / grilled fish, 1 chapati, soup",
        tCal * 0.20, 20, 45, 8));
}

// ─── generate ─────────────────────────────────────────────────
void DietPlan::generate() {
    if (goal == "loss")    buildWeightLossPlan(targetCalories);
    else if (goal == "gain") buildWeightGainPlan(targetCalories);
    else                   buildMaintenancePlan(targetCalories);
}

// ─── display ──────────────────────────────────────────────────
void DietPlan::display() const {
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "  Diet Plan  [" << planID << "]\n";
    std::cout << "  Goal: " << goal
              << " | Target: " << (int)targetCalories << " kcal\n";
    std::cout << "  Date: " << generatedDate << "\n";
    std::cout << "──────────────────────────────────────────\n";
    for (const auto& m : meals) {
        std::cout << "  [" << m.mealType << "]\n";
        std::cout << "  Foods : " << m.foodItems << "\n";
        std::cout << "  Cal: " << (int)m.calories
                  << " | P: " << (int)m.protein
                  << "g | C: " << (int)m.carbs
                  << "g | F: " << (int)m.fats << "g\n\n";
    }
    std::cout << "  Notes: " << notes << "\n";
    std::cout << "╚══════════════════════════════════════════╝\n";
}

// ─── Totals ───────────────────────────────────────────────────
double DietPlan::getTotalCalories() const {
    double s = 0;
    for (auto& m : meals) s += m.calories;
    return s;
}
double DietPlan::getTotalProtein() const {
    double s = 0;
    for (auto& m : meals) s += m.protein;
    return s;
}
double DietPlan::getTotalCarbs() const {
    double s = 0;
    for (auto& m : meals) s += m.carbs;
    return s;
}
double DietPlan::getTotalFats() const {
    double s = 0;
    for (auto& m : meals) s += m.fats;
    return s;
}

// ─── Serialise ────────────────────────────────────────────────
// Format:
//   PLAN|planID|userID|goal|date|targetCal
//   MEAL|type|foods|cal|pro|carb|fat
//   ... (one per meal)
//   NOTES|...
//   END
std::string DietPlan::serialize() const {
    std::ostringstream oss;
    oss << "PLAN|" << planID << "|" << userID << "|"
        << goal << "|" << generatedDate << "|"
        << std::fixed << std::setprecision(2) << targetCalories << "\n";
    for (const auto& m : meals) {
        oss << "MEAL|" << m.mealType << "|" << m.foodItems << "|"
            << std::fixed << std::setprecision(2)
            << m.calories << "|" << m.protein << "|"
            << m.carbs    << "|" << m.fats    << "\n";
    }
    oss << "NOTES|" << notes << "\n";
    oss << "END\n";
    return oss.str();
}

// ─── Deserialise ──────────────────────────────────────────────
bool DietPlan::deserialize(const std::string& block) {
    meals.clear();
    std::istringstream iss(block);
    std::string line;
    bool planFound = false;

    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        std::istringstream ls(line);
        std::string tag;
        std::getline(ls, tag, '|');

        if (tag == "PLAN") {
            std::string tok;
            std::vector<std::string> parts;
            while (std::getline(ls, tok, '|')) parts.push_back(tok);
            if (parts.size() < 5) return false;
            planID         = parts[0];
            userID         = parts[1];
            goal           = parts[2];
            generatedDate  = parts[3];
            targetCalories = std::stod(parts[4]);
            planFound = true;
        } else if (tag == "MEAL") {
            std::string tok;
            std::vector<std::string> parts;
            while (std::getline(ls, tok, '|')) parts.push_back(tok);
            if (parts.size() < 6) continue;
            Meal m;
            m.mealType  = parts[0];
            m.foodItems = parts[1];
            m.calories  = std::stod(parts[2]);
            m.protein   = std::stod(parts[3]);
            m.carbs     = std::stod(parts[4]);
            m.fats      = std::stod(parts[5]);
            meals.push_back(m);
        } else if (tag == "NOTES") {
            std::getline(ls, notes);
        }
    }
    return planFound;
}

// ─── JSON export ──────────────────────────────────────────────
std::string DietPlan::toJSON() const {
    std::ostringstream j;
    j << "{";
    j << "\"planID\":\"" << planID << "\",";
    j << "\"userID\":\"" << userID << "\",";
    j << "\"goal\":\"" << goal << "\",";
    j << "\"date\":\"" << generatedDate << "\",";
    j << "\"targetCalories\":" << std::fixed << std::setprecision(0) << targetCalories << ",";
    j << "\"totalCalories\":" << (int)getTotalCalories() << ",";
    j << "\"totalProtein\":"  << (int)getTotalProtein()  << ",";
    j << "\"totalCarbs\":"    << (int)getTotalCarbs()    << ",";
    j << "\"totalFats\":"     << (int)getTotalFats()     << ",";
    j << "\"notes\":\"" << notes << "\",";
    j << "\"meals\":[";
    for (size_t i = 0; i < meals.size(); ++i) {
        if (i) j << ",";
        const Meal& m = meals[i];
        j << "{";
        j << "\"type\":\"" << m.mealType << "\",";
        j << "\"foods\":\"" << m.foodItems << "\",";
        j << "\"calories\":"  << (int)m.calories << ",";
        j << "\"protein\":"   << (int)m.protein  << ",";
        j << "\"carbs\":"     << (int)m.carbs    << ",";
        j << "\"fats\":"      << (int)m.fats;
        j << "}";
    }
    j << "]}";
    return j.str();
}
