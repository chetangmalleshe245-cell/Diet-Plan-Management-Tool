/*
 * main.cpp  –  CGI entry point
 *
 * Compile:  g++ -std=c++17 -o diet_api.cgi src/*.cpp
 * Place in: /usr/lib/cgi-bin/  (Apache)  OR run directly for testing.
 *
 * Query string format (GET):
 *   action=register&name=...&age=...&weight=...&height=...
 *                 &goal=...&email=...&password=...&otp=...
 *   action=login&email=...&password=...
 *   action=verify_login&email=...&otp=...
 *   action=generate_plan&userID=...
 *   action=get_history&userID=...
 *   action=get_progress&userID=...
 *   action=log_calories&userID=...&calories=...
 *   action=update_profile&userID=...&weight=...&height=...&goal=...
 *   action=request_otp&email=...
 *
 * All responses are JSON (Content-Type: application/json).
 */

#include "include/FileManager.h"
#include "include/AuthManager.h"
#include "include/DietManager.h"
#include <iostream>
#include <sstream>
#include <map>
#include <cstdlib>
#include <string>

// ─── URL decode ───────────────────────────────────────────────
static std::string urlDecode(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '+') {
            out += ' ';
        } else if (s[i] == '%' && i + 2 < s.size()) {
            int v;
            sscanf(s.substr(i+1, 2).c_str(), "%x", &v);
            out += (char)v;
            i += 2;
        } else {
            out += s[i];
        }
    }
    return out;
}

// ─── Parse query string ──────────────────────────────────────
static std::map<std::string, std::string> parseQuery(const std::string& qs) {
    std::map<std::string, std::string> params;
    std::istringstream ss(qs);
    std::string pair;
    while (std::getline(ss, pair, '&')) {
        auto eq = pair.find('=');
        if (eq == std::string::npos) continue;
        std::string key = urlDecode(pair.substr(0, eq));
        std::string val = urlDecode(pair.substr(eq + 1));
        params[key] = val;
    }
    return params;
}

// ─── JSON helpers ─────────────────────────────────────────────
static std::string jsonOK(const std::string& data) {
    return "{\"status\":\"ok\"," + data + "}";
}
static std::string jsonErr(const std::string& msg) {
    return "{\"status\":\"error\",\"message\":\"" + msg + "\"}";
}
static std::string kv(const std::string& k, const std::string& v) {
    return "\"" + k + "\":\"" + v + "\"";
}

// ─── Main ─────────────────────────────────────────────────────
int main() {
    // CGI header
    std::cout << "Content-Type: application/json\r\n";
    std::cout << "Access-Control-Allow-Origin: *\r\n";
    std::cout << "\r\n";

    // Get query string
    std::string qs;
    const char* qenv = getenv("QUERY_STRING");
    if (qenv) qs = std::string(qenv);
    // Also accept POST body (simple approach)
    const char* method = getenv("REQUEST_METHOD");
    if (method && std::string(method) == "POST") {
        const char* lenStr = getenv("CONTENT_LENGTH");
        if (lenStr) {
            int len = std::stoi(lenStr);
            std::string body(len, '\0');
            std::cin.read(&body[0], len);
            qs = body;
        }
    }

    auto params = parseQuery(qs);
    std::string action = params["action"];

    // Initialise services
    FileManager fm("../../data");
    AuthManager auth(fm);
    DietManager dietMgr(fm);

    // ── Route ──────────────────────────────────────────────────
    if (action == "request_otp") {
        // Initiate registration OTP
        std::string email = params["email"];
        std::string otp;
        if (auth.initiateRegistration(email, otp)) {
            // In a real system, OTP would be emailed. For demo, return it.
            std::cout << jsonOK(kv("otp", otp) + "," + kv("email", email));
        } else {
            std::cout << jsonErr("Email already registered.");
        }

    } else if (action == "register") {
        std::string errMsg, otp;
        RegisteredUser newUser;
        bool ok = auth.completeRegistration(
            params["name"],
            std::stoi(params.count("age") ? params["age"] : "0"),
            std::stod(params.count("weight") ? params["weight"] : "0"),
            std::stod(params.count("height") ? params["height"] : "0"),
            params["goal"],
            params["email"],
            params["password"],
            params["otp"],
            newUser,
            errMsg);
        if (ok) {
            std::cout << jsonOK(kv("userID", newUser.getUserID()) + ","
                              + kv("name", newUser.getName()) + ","
                              + kv("email", newUser.getEmail()) + ","
                              + kv("goal", newUser.getGoal()));
        } else {
            std::cout << jsonErr(errMsg);
        }

    } else if (action == "login") {
        std::string errMsg, otp;
        bool ok = auth.initiateLogin(params["email"], params["password"],
                                     otp, errMsg);
        if (ok) {
            // Return OTP in response (simulated; normally sent via email)
            std::cout << jsonOK(kv("otp", otp) + ","
                              + kv("email", params["email"]));
        } else {
            std::cout << jsonErr(errMsg);
        }

    } else if (action == "verify_login") {
        std::string errMsg;
        RegisteredUser user;
        bool ok = auth.completeLogin(params["email"], params["otp"],
                                     user, errMsg);
        if (ok) {
            std::cout << jsonOK(kv("userID", user.getUserID()) + ","
                              + kv("name", user.getName()) + ","
                              + kv("email", user.getEmail()) + ","
                              + kv("goal", user.getGoal()) + ","
                              + "\"bmi\":" + std::to_string(user.calculateBMI()) + ","
                              + "\"targetCalories\":"
                              + std::to_string((int)user.calculateDailyCalories()));
        } else {
            std::cout << jsonErr(errMsg);
        }

    } else if (action == "generate_plan") {
        RegisteredUser user;
        if (!fm.loadUserByID(params["userID"], user)) {
            std::cout << jsonErr("User not found.");
        } else {
            DietPlan plan = dietMgr.generatePlan(user);
            std::cout << "{\"status\":\"ok\"," << plan.toJSON().substr(1); // merge
        }

    } else if (action == "get_history") {
        auto plans = dietMgr.getUserHistory(params["userID"]);
        std::cout << "{\"status\":\"ok\",\"plans\":[";
        for (size_t i = 0; i < plans.size(); ++i) {
            if (i) std::cout << ",";
            std::cout << plans[i].toJSON();
        }
        std::cout << "]}";

    } else if (action == "get_progress") {
        RegisteredUser user;
        if (!fm.loadUserByID(params["userID"], user)) {
            std::cout << jsonErr("User not found.");
        } else {
            std::string prog = dietMgr.getProgressJSON(user);
            std::cout << "{\"status\":\"ok\"," << prog.substr(1);
        }

    } else if (action == "log_calories") {
        RegisteredUser user;
        if (!fm.loadUserByID(params["userID"], user)) {
            std::cout << jsonErr("User not found.");
        } else {
            std::string errMsg;
            double cal = std::stod(params.count("calories") ? params["calories"] : "0");
            if (dietMgr.logCalories(user, cal, errMsg)) {
                std::cout << jsonOK("\"logged\":" + std::to_string((int)cal));
            } else {
                std::cout << jsonErr(errMsg);
            }
        }

    } else if (action == "update_profile") {
        RegisteredUser user;
        if (!fm.loadUserByID(params["userID"], user)) {
            std::cout << jsonErr("User not found.");
        } else {
            std::string errMsg;
            bool ok = auth.updateProfile(
                user,
                std::stod(params.count("weight") ? params["weight"] : "0"),
                std::stod(params.count("height") ? params["height"] : "0"),
                params["goal"], errMsg);
            if (ok) {
                std::cout << jsonOK(kv("userID", user.getUserID()));
            } else {
                std::cout << jsonErr(errMsg);
            }
        }

    } else {
        std::cout << jsonErr("Unknown action: " + action);
    }

    return 0;
}
