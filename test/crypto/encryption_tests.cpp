#include "crypto/encryption.h"
#include <iostream>

int main() {
    if (!crypto::InitCrypto()) {
        std::cerr << "sodium_init failed" << std::endl;
        return 1;
    }
    auto key = crypto::GenerateKey();
    std::string message = "test-message";
    crypto::Bytes pt(message.begin(), message.end());

    auto ct = crypto::EncryptAEAD(key, pt);
    auto dec = crypto::DecryptAEAD(key, ct);
    if (dec == pt) {
        std::cout << "encryption roundtrip OK" << std::endl;
        return 0;
    }
    std::cout << "encryption failed" << std::endl;
    return 2;
}
