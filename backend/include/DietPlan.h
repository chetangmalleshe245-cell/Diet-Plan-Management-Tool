#ifndef DIET_PLAN_H
#define DIET_PLAN_H

#include <string>
#include <vector>
#include <map>

// ─────────────────────────────────────────────────────────────
//  DietPlan Class
//  Demonstrates: Encapsulation, Abstraction, Composition
// ─────────────────────────────────────────────────────────────

struct Meal {
    std::string mealType;    // Breakfast / Lunch / Dinner / Snack
    std::string foodItems;   // comma-separated items
    double      calories;
    double      protein;     // grams
    double      carbs;       // grams
    double      fats;        // grams
};

class DietPlan {
private:
    std::string planID;
    std::string userID;
    std::string goal;          // "loss" | "gain" | "maintain"
    std::string generatedDate;
    double      targetCalories;
    std::vector<Meal> meals;
    std::string notes;

    // Internal helpers
    void buildWeightLossPlan(double targetCal);
    void buildWeightGainPlan(double targetCal);
    void buildMaintenancePlan(double targetCal);

public:
    DietPlan();
    DietPlan(const std::string& planID,
             const std::string& userID,
             const std::string& goal,
             double targetCalories);

    // ── Core operations ──
    void generate();          // builds meal list based on goal
    void display()   const;

    // ── Getters ──
    std::string getPlanID()        const { return planID; }
    std::string getUserID()        const { return userID; }
    std::string getGoal()          const { return goal; }
    std::string getGeneratedDate() const { return generatedDate; }
    double      getTargetCalories()const { return targetCalories; }
    const std::vector<Meal>& getMeals() const { return meals; }
    std::string getNotes()         const { return notes; }
    double      getTotalCalories() const;
    double      getTotalProtein()  const;
    double      getTotalCarbs()    const;
    double      getTotalFats()     const;

    // ── File I/O ──
    std::string serialize()   const;
    bool        deserialize(const std::string& block);

    // ── JSON export for frontend ──
    std::string toJSON() const;
};

#endif // DIET_PLAN_H
