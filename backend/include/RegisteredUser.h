#ifndef REGISTERED_USER_H
#define REGISTERED_USER_H

#include "User.h"
#include <ctime>

// ─────────────────────────────────────────────────────────────
//  Derived Class: RegisteredUser
//  Demonstrates: Inheritance, Polymorphism (override displayInfo)
// ─────────────────────────────────────────────────────────────
class RegisteredUser : public User {
private:
    std::string registrationDate;
    int         totalPlansGenerated;
    double      currentCaloriesLogged;   // today's log

public:
    RegisteredUser();
    RegisteredUser(const std::string& id,
                   const std::string& name,
                   int age,
                   double weight,
                   double height,
                   const std::string& goal,
                   const std::string& email,
                   const std::string& pwdHash);

    // ── Overridden virtual methods (Polymorphism) ──
    void        displayInfo() const override;
    std::string getRole()     const override { return "RegisteredUser"; }

    // ── Extra getters / setters ──
    std::string getRegistrationDate()    const { return registrationDate; }
    int         getTotalPlansGenerated() const { return totalPlansGenerated; }
    double      getCurrentCaloriesLogged()const{ return currentCaloriesLogged; }

    void incrementPlanCount()                  { ++totalPlansGenerated; }
    void logCalories(double cal)               { currentCaloriesLogged += cal; }
    void resetDailyCalories()                  { currentCaloriesLogged = 0.0; }

    // ── Serialisation ──
    std::string serialize()                const override;
    bool        deserialize(const std::string& line) override;
};

#endif // REGISTERED_USER_H
