#pragma once
#include <vector>
#include <string>

namespace crypto {

using Bytes = std::vector<unsigned char>;

// Initialize the crypto library (call once at startup; returns false on error)
bool InitCrypto();

// Generate a random symmetric key (crypto_aead_chacha20poly1305_ietf_KEYBYTES)
Bytes GenerateKey();

// Encrypt plaintext with key. Returns ciphertext with nonce prepended: [nonce||ciphertext||tag]
Bytes EncryptAEAD(const Bytes& key, const Bytes& plaintext, const Bytes& aad = Bytes());

// Decrypt data produced by EncryptAEAD. Returns plaintext or empty vector on failure.
Bytes DecryptAEAD(const Bytes& key, const Bytes& ciphertext_with_nonce, const Bytes& aad = Bytes());

// Convenience helpers (hex encode / decode)
std::string ToHex(const Bytes& b);
Bytes FromHex(const std::string& hex);

} // namespace crypto
