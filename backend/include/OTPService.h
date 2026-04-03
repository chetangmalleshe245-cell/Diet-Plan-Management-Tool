#ifndef OTP_SERVICE_H
#define OTP_SERVICE_H

#include <string>
#include "FileManager.h"

// ─────────────────────────────────────────────────────────────
//  OTPService Class
//  Demonstrates: Encapsulation, Single Responsibility
//  Generates, stores, and verifies 6-digit OTPs
// ─────────────────────────────────────────────────────────────
class OTPService {
private:
    FileManager& fileManager;
    int          otpLength;
    int          expirySeconds;   // OTP is valid for this many seconds

    std::string generateRandom() const;

public:
    explicit OTPService(FileManager& fm,
                        int length = 6,
                        int expiry = 300);

    // Generates an OTP, saves it, and returns it (to be "sent")
    std::string generateAndStore(const std::string& email);

    // Returns true if otp matches stored value (and not expired)
    bool verify(const std::string& email, const std::string& otp);

    // Clears stored OTP after successful use
    void invalidate(const std::string& email);

    // Simulate sending (prints to console; in production: email/SMS)
    void simulateSend(const std::string& email, const std::string& otp) const;
};

#endif // OTP_SERVICE_H
