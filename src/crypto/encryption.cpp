#include "encryption.h"
#include <sodium.h>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <iomanip>

namespace crypto {

bool InitCrypto() {
    if (sodium_init() < 0) return false;
    return true;
}

Bytes GenerateKey() {
    Bytes key(crypto_aead_chacha20poly1305_ietf_KEYBYTES);
    randombytes_buf(key.data(), key.size());
    return key;
}

Bytes EncryptAEAD(const Bytes& key, const Bytes& plaintext, const Bytes& aad) {
    if (key.size() != crypto_aead_chacha20poly1305_ietf_KEYBYTES) {
        throw std::invalid_argument("invalid key size");
    }
    const size_t nonce_len = crypto_aead_chacha20poly1305_ietf_NPUBBYTES;
    Bytes nonce(nonce_len);
    randombytes_buf(nonce.data(), nonce_len);

    size_t ciphertext_len = plaintext.size() + crypto_aead_chacha20poly1305_ietf_ABYTES;
    Bytes ciphertext(ciphertext_len);

    unsigned long long out_len;
    if (crypto_aead_chacha20poly1305_ietf_encrypt(
            ciphertext.data(), &out_len,
            plaintext.data(), plaintext.size(),
            aad.empty() ? nullptr : aad.data(), aad.size(),
            nullptr, nonce.data(), key.data()) != 0) {
        throw std::runtime_error("encryption failed");
    }
    // Prepend nonce to output
    Bytes out;
    out.reserve(nonce_len + out_len);
    out.insert(out.end(), nonce.begin(), nonce.end());
    out.insert(out.end(), ciphertext.begin(), ciphertext.begin() + out_len);
    // Overwrite sensitive buffers where possible
    sodium_memzero(ciphertext.data(), ciphertext.size());
    sodium_memzero(nonce.data(), nonce.size());
    return out;
}

Bytes DecryptAEAD(const Bytes& key, const Bytes& ciphertext_with_nonce, const Bytes& aad) {
    if (key.size() != crypto_aead_chacha20poly1305_ietf_KEYBYTES) {
        throw std::invalid_argument("invalid key size");
    }
    const size_t nonce_len = crypto_aead_chacha20poly1305_ietf_NPUBBYTES;
    if (ciphertext_with_nonce.size() < nonce_len + crypto_aead_chacha20poly1305_ietf_ABYTES) {
        return Bytes(); // too short / invalid
    }
    Bytes nonce(ciphertext_with_nonce.begin(), ciphertext_with_nonce.begin() + nonce_len);
    Bytes ciphertext(ciphertext_with_nonce.begin() + nonce_len, ciphertext_with_nonce.end());

    Bytes plaintext(ciphertext.size()); // will be >= real plaintext
    unsigned long long out_len;
    if (crypto_aead_chacha20poly1305_ietf_decrypt(
            plaintext.data(), &out_len,
            nullptr,
            ciphertext.data(), ciphertext.size(),
            aad.empty() ? nullptr : aad.data(), aad.size(),
            nonce.data(), key.data()) != 0) {
        // decryption failed (authentication failed)
        sodium_memzero(plaintext.data(), plaintext.size());
        return Bytes();
    }
    plaintext.resize(out_len);
    sodium_memzero(nonce.data(), nonce.size());
    return plaintext;
}

std::string ToHex(const Bytes& b) {
    std::ostringstream oss;
    for (unsigned char c : b) oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    return oss.str();
}

Bytes FromHex(const std::string& hex) {
    Bytes out;
    if (hex.size() % 2 != 0) return out;
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2) {
        unsigned int byte;
        std::istringstream iss(hex.substr(i,2));
        iss >> std::hex >> byte;
        out.push_back(static_cast<unsigned char>(byte));
    }
    return out;
}

} // namespace crypto
