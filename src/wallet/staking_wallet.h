#pragma once
#include <string>
#include <vector>

namespace staking {

struct StakeInfo {
    bool enabled{false};
    int duration_days{0};
    std::string note;
};

// Simple manager that records wallet-level staking metadata.
// This is intentionally lightweight and file-backed; integrate with wallet internals
// in follow-up PRs.
class WalletStakingManager {
public:
    WalletStakingManager(const std::string& data_dir);
    bool Init();

    // Mark all available funds as staked for duration_days (dry_run=false executes changes)
    bool StakeAllFunds(int duration_days, bool dry_run = true);

    // Remove staking marks (dry_run optional)
    bool UnstakeAll(bool dry_run = true);

    // Status
    StakeInfo GetStatus() const;

private:
    std::string data_dir_;
    std::string state_file_;
    StakeInfo state_;
    bool Load();
    bool Save() const;
};

} // namespace staking
