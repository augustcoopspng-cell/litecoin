// RPC stubs for staking operations
// These are lightweight command-line utilities / RPC entry points to be integrated
// with the node's RPC framework in follow-up PRs.

#include "wallet/staking_wallet.h"
#include "crypto/encryption.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // Simple CLI for manual testing in early stages.
    if (argc < 2) {
        std::cout << "usage: stakingrpc <command> [args]\n";
        std::cout << "commands: status init stake_all unstake_all encrypt_test" << std::endl;
        return 1;
    }
    std::string cmd = argv[1];
    std::string datadir = "."; // default; in real node use node's datadir
    staking::WalletStakingManager mgr(datadir);
    mgr.Init();
    if (cmd == "status") {
        auto s = mgr.GetStatus();
        std::cout << "enabled=" << s.enabled << " duration_days=" << s.duration_days << " note=" << s.note << std::endl;
        return 0;
    }
    if (cmd == "stake_all") {
        int days = 365;
        bool dry = true;
        if (argc >= 3) days = std::stoi(argv[2]);
        if (argc >= 4) dry = (std::string(argv[3]) != "live");
        mgr.StakeAllFunds(days, dry);
        std::cout << "stake_all: " << (dry ? "dry-run" : "executed") << " days=" << days << std::endl;
        return 0;
    }
    if (cmd == "unstake_all") {
        bool dry = true;
        if (argc >= 2) dry = (std::string(argv[2]) != "live");
        mgr.UnstakeAll(dry);
        std::cout << "unstake_all: " << (dry ? "dry-run" : "executed") << std::endl;
        return 0;
    }
    if (cmd == "encrypt_test") {
        if (!crypto::InitCrypto()) {
            std::cerr << "sodium_init failed" << std::endl;
            return 2;
        }
        auto key = crypto::GenerateKey();
        crypto::Bytes pt = { 'h','e','l','l','o' };
        auto ct = crypto::EncryptAEAD(key, pt);
        auto dec = crypto::DecryptAEAD(key, ct);
        std::cout << "pt=" << std::string(pt.begin(), pt.end()) << " dec=" << std::string(dec.begin(), dec.end()) << std::endl;
        return 0;
    }
    std::cout << "unknown command" << std::endl;
    return 2;
}
