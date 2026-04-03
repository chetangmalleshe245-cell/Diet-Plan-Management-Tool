#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>

// ─────────────────────────────────────────────────────────────
//  Abstract Base Class: User
//  Demonstrates: Abstraction, Encapsulation
// ─────────────────────────────────────────────────────────────
class User {
protected:
    std::string userID;
    std::string name;
    int         age;
    double      weight;   // kg
    double      height;   // cm
    std::string goal;     // "loss" | "gain" | "maintain"
    std::string email;
    std::string passwordHash;

public:
    // Constructor
    User();
    User(const std::string& id,
         const std::string& name,
         int age,
         double weight,
         double height,
         const std::string& goal,
         const std::string& email,
         const std::string& pwdHash);

    // Virtual destructor for proper polymorphic cleanup
    virtual ~User() = default;

    // ── Pure virtual method (forces subclasses to implement) ──
    virtual void displayInfo() const = 0;
    virtual std::string getRole() const = 0;

    // ── Getters (Encapsulation) ──
    std::string getUserID()      const { return userID; }
    std::string getName()        const { return name; }
    int         getAge()         const { return age; }
    double      getWeight()      const { return weight; }
    double      getHeight()      const { return height; }
    std::string getGoal()        const { return goal; }
    std::string getEmail()       const { return email; }
    std::string getPasswordHash()const { return passwordHash; }

    // ── Setters ──
    void setName(const std::string& n)    { name = n; }
    void setAge(int a)                    { age = a; }
    void setWeight(double w)              { weight = w; }
    void setHeight(double h)              { height = h; }
    void setGoal(const std::string& g)    { goal = g; }
    void setEmail(const std::string& e)   { email = e; }
    void setPasswordHash(const std::string& p) { passwordHash = p; }

    // ── Utility ──
    double calculateBMI() const;
    std::string getBMICategory() const;
    double calculateDailyCalories() const;   // Harris-Benedict formula

    // Serialise to a single line for file storage
    virtual std::string serialize()   const;
    virtual bool        deserialize(const std::string& line);
};

#endif // USER_H
