#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include "RegisteredUser.h"
#include "FileManager.h"
#include "OTPService.h"
#include <string>

// ─────────────────────────────────────────────────────────────
//  AuthManager Class
//  Demonstrates: Encapsulation, Abstraction
//  Handles: Registration, Login, Password Hashing
// ─────────────────────────────────────────────────────────────
class AuthManager {
private:
    FileManager& fileManager;
    OTPService   otpService;

    // Simple hash – NOT for production (use bcrypt/argon2 in real apps)
    std::string hashPassword(const std::string& plain) const;
    std::string generateUserID() const;

public:
    explicit AuthManager(FileManager& fm);

    // ── Registration flow ──
    // Step 1: validate inputs & store temp OTP
    bool initiateRegistration(const std::string& email,
                              std::string& outOTP);

    // Step 2: verify OTP, then create the account
    bool completeRegistration(const std::string& name,
                              int age,
                              double weight,
                              double height,
                              const std::string& goal,
                              const std::string& email,
                              const std::string& password,
                              const std::string& otp,
                              RegisteredUser& outUser,
                              std::string& errMsg);

    // ── Login flow ──
    bool initiateLogin(const std::string& email,
                       const std::string& password,
                       std::string& outOTP,
                       std::string& errMsg);

    bool completeLogin(const std::string& email,
                       const std::string& otp,
                       RegisteredUser& outUser,
                       std::string& errMsg);

    // ── Profile update ──
    bool updateProfile(RegisteredUser& user,
                       double newWeight,
                       double newHeight,
                       const std::string& newGoal,
                       std::string& errMsg);

    // ── Password change ──
    bool changePassword(RegisteredUser& user,
                        const std::string& oldPwd,
                        const std::string& newPwd,
                        std::string& errMsg);
};

#endif // AUTH_MANAGER_H
