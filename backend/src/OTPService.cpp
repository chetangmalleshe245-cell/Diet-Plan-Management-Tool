#include "../include/OTPService.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

// ─── Constructor ──────────────────────────────────────────────
OTPService::OTPService(FileManager& fm, int length, int expiry)
    : fileManager(fm), otpLength(length), expirySeconds(expiry) {
    srand((unsigned)time(nullptr));
}

// ─── Generate random N-digit OTP ─────────────────────────────
std::string OTPService::generateRandom() const {
    std::ostringstream oss;
    for (int i = 0; i < otpLength; ++i)
        oss << (rand() % 10);
    return oss.str();
}

// ─── Generate, store, and return OTP ─────────────────────────
std::string OTPService::generateAndStore(const std::string& email) {
    std::string otp = generateRandom();
    fileManager.saveOTP(email, otp);
    return otp;
}

// ─── Verify OTP ───────────────────────────────────────────────
bool OTPService::verify(const std::string& email, const std::string& otp) {
    return fileManager.validateOTP(email, otp);
}

// ─── Invalidate OTP ───────────────────────────────────────────
void OTPService::invalidate(const std::string& email) {
    fileManager.clearOTP(email);
}

// ─── Simulate send (console print) ───────────────────────────
void OTPService::simulateSend(const std::string& email,
                              const std::string& otp) const {
    std::cout << "\n┌─────────────────────────────────┐\n";
    std::cout << "│  [OTP Simulation]               │\n";
    std::cout << "│  To  : " << std::setw(24) << std::left
              << email << "│\n";
    std::cout << "│  OTP : " << otp
              << "  (valid 5 min)    │\n";
    std::cout << "└─────────────────────────────────┘\n";
}
