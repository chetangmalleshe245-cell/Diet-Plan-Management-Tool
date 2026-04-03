#include "../include/User.h"
#include <sstream>
#include <cmath>
#include <iomanip>

// ─── Default constructor ───────────────────────────────────────
User::User()
    : userID(""), name(""), age(0),
      weight(0.0), height(0.0), goal("maintain"),
      email(""), passwordHash("") {}

// ─── Parameterised constructor ─────────────────────────────────
User::User(const std::string& id,
           const std::string& n,
           int a,
           double w,
           double h,
           const std::string& g,
           const std::string& e,
           const std::string& pwd)
    : userID(id), name(n), age(a),
      weight(w), height(h), goal(g),
      email(e), passwordHash(pwd) {}

// ─── BMI ───────────────────────────────────────────────────────
double User::calculateBMI() const {
    if (height <= 0) return 0.0;
    double hm = height / 100.0;   // cm → m
    return weight / (hm * hm);
}

std::string User::getBMICategory() const {
    double bmi = calculateBMI();
    if (bmi < 18.5)       return "Underweight";
    else if (bmi < 25.0)  return "Normal weight";
    else if (bmi < 30.0)  return "Overweight";
    else                  return "Obese";
}

// ─── Harris-Benedict Equation (simplified, gender-neutral avg) ─
double User::calculateDailyCalories() const {
    // BMR using Mifflin-St Jeor (average male/female)
    double bmr = (10 * weight) + (6.25 * height) - (5 * age) + 5;
    // Multiply by moderate activity factor 1.55
    double tdee = bmr * 1.55;

    if (goal == "loss")     return tdee - 500;   // deficit
    if (goal == "gain")     return tdee + 500;   // surplus
    return tdee;                                   // maintain
}

// ─── Serialise ─────────────────────────────────────────────────
std::string User::serialize() const {
    std::ostringstream oss;
    oss << userID << "|"
        << name   << "|"
        << age    << "|"
        << std::fixed << std::setprecision(2)
        << weight << "|"
        << height << "|"
        << goal   << "|"
        << email  << "|"
        << passwordHash;
    return oss.str();
}

// ─── Deserialise ───────────────────────────────────────────────
bool User::deserialize(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, '|'))
        tokens.push_back(token);

    if (tokens.size() < 8) return false;
    userID       = tokens[0];
    name         = tokens[1];
    age          = std::stoi(tokens[2]);
    weight       = std::stod(tokens[3]);
    height       = std::stod(tokens[4]);
    goal         = tokens[5];
    email        = tokens[6];
    passwordHash = tokens[7];
    return true;
}
