#include "../include/AuthManager.h"
#include <sstream>
#include <iomanip>
#include <functional>
#include <ctime>

// ─── Constructor ──────────────────────────────────────────────
AuthManager::AuthManager(FileManager& fm)
    : fileManager(fm), otpService(fm) {}

// ─── Simple hash using std::hash (NOT production-safe) ───────
std::string AuthManager::hashPassword(const std::string& plain) const {
    std::hash<std::string> hasher;
    size_t h = hasher(plain + "DietSalt#2024");
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << h;
    return oss.str();
}

// ─── Generate unique user ID ──────────────────────────────────
std::string AuthManager::generateUserID() const {
    // Format: UID_<timestamp>_<rand>
    time_t now = time(nullptr);
    std::ostringstream oss;
    oss << "UID_" << now << "_" << (rand() % 9000 + 1000);
    return oss.str();
}

// ─── Initiate Registration ────────────────────────────────────
bool AuthManager::initiateRegistration(const std::string& email,
                                       std::string& outOTP) {
    if (fileManager.userExists(email)) return false;   // already registered
    outOTP = otpService.generateAndStore(email);
    otpService.simulateSend(email, outOTP);
    return true;
}

// ─── Complete Registration ────────────────────────────────────
bool AuthManager::completeRegistration(const std::string& name,
                                       int age,
                                       double weight,
                                       double height,
                                       const std::string& goal,
                                       const std::string& email,
                                       const std::string& password,
                                       const std::string& otp,
                                       RegisteredUser& outUser,
                                       std::string& errMsg) {
    // Validate OTP
    if (!otpService.verify(email, otp)) {
        errMsg = "Invalid or expired OTP.";
        return false;
    }

    // Validate inputs
    if (name.empty() || age < 5 || age > 120) {
        errMsg = "Invalid name or age.";
        return false;
    }
    if (weight < 20 || weight > 500 || height < 50 || height > 300) {
        errMsg = "Invalid weight or height values.";
        return false;
    }
    if (goal != "loss" && goal != "gain" && goal != "maintain") {
        errMsg = "Goal must be: loss, gain, or maintain.";
        return false;
    }
    if (password.length() < 6) {
        errMsg = "Password must be at least 6 characters.";
        return false;
    }

    std::string uid  = generateUserID();
    std::string hash = hashPassword(password);

    outUser = RegisteredUser(uid, name, age, weight, height, goal, email, hash);

    if (!fileManager.saveUser(outUser)) {
        errMsg = "Failed to save user. Check data directory.";
        return false;
    }

    otpService.invalidate(email);
    errMsg = "";
    return true;
}

// ─── Initiate Login ───────────────────────────────────────────
bool AuthManager::initiateLogin(const std::string& email,
                                const std::string& password,
                                std::string& outOTP,
                                std::string& errMsg) {
    RegisteredUser user;
    if (!fileManager.loadUserByEmail(email, user)) {
        errMsg = "Email not found.";
        return false;
    }

    if (user.getPasswordHash() != hashPassword(password)) {
        errMsg = "Incorrect password.";
        return false;
    }

    outOTP = otpService.generateAndStore(email);
    otpService.simulateSend(email, outOTP);
    errMsg = "";
    return true;
}

// ─── Complete Login ───────────────────────────────────────────
bool AuthManager::completeLogin(const std::string& email,
                                const std::string& otp,
                                RegisteredUser& outUser,
                                std::string& errMsg) {
    if (!otpService.verify(email, otp)) {
        errMsg = "Invalid or expired OTP.";
        return false;
    }

    if (!fileManager.loadUserByEmail(email, outUser)) {
        errMsg = "User not found.";
        return false;
    }

    otpService.invalidate(email);
    errMsg = "";
    return true;
}

// ─── Update Profile ───────────────────────────────────────────
bool AuthManager::updateProfile(RegisteredUser& user,
                                double newWeight,
                                double newHeight,
                                const std::string& newGoal,
                                std::string& errMsg) {
    if (newWeight < 20 || newWeight > 500) {
        errMsg = "Invalid weight."; return false;
    }
    if (newHeight < 50 || newHeight > 300) {
        errMsg = "Invalid height."; return false;
    }
    if (newGoal != "loss" && newGoal != "gain" && newGoal != "maintain") {
        errMsg = "Invalid goal."; return false;
    }

    user.setWeight(newWeight);
    user.setHeight(newHeight);
    user.setGoal(newGoal);

    if (!fileManager.updateUser(user)) {
        errMsg = "Failed to update user file."; return false;
    }
    errMsg = "";
    return true;
}

// ─── Change Password ──────────────────────────────────────────
bool AuthManager::changePassword(RegisteredUser& user,
                                 const std::string& oldPwd,
                                 const std::string& newPwd,
                                 std::string& errMsg) {
    if (user.getPasswordHash() != hashPassword(oldPwd)) {
        errMsg = "Current password is incorrect."; return false;
    }
    if (newPwd.length() < 6) {
        errMsg = "New password too short."; return false;
    }
    user.setPasswordHash(hashPassword(newPwd));
    if (!fileManager.updateUser(user)) {
        errMsg = "Failed to save new password."; return false;
    }
    errMsg = "";
    return true;
}
