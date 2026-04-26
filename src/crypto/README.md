# Encryption helper (ChaCha20-Poly1305 using libsodium)

This module provides a small, self-contained AEAD encryption helper using libsodium.

Build / Integration
- Requires libsodium development headers and library available on the build host.
- CMake: link with "sodium" or find libsodium using pkg-config. Example snippet:

  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SODIUM REQUIRED libsodium)
  include_directories(${SODIUM_INCLUDE_DIRS})
  target_link_libraries(your_target ${SODIUM_LIBRARIES})

Usage
- Call crypto::InitCrypto() once at program startup.
- Use crypto::GenerateKey() to create a fresh symmetric key, store it securely.
- Use crypto::EncryptAEAD / crypto::DecryptAEAD to encrypt / decrypt data.
