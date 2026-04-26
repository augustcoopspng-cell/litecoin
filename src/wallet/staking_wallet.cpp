#include "wallet/staking_wallet.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace staking {

WalletStakingManager::WalletStakingManager(const std::string& data_dir)
    : data_dir_(data_dir), state_file_(fs::path(data_dir) / "staking_state.txt") {}

bool WalletStakingManager::Init() {
    if (!fs::exists(data_dir_)) {
        fs::create_directories(data_dir_);
    }
    return Load();
}

bool WalletStakingManager::Load() {
    state_ = StakeInfo();
    std::ifstream ifs(state_file_);
    if (!ifs) return true; // no state yet
    std::string line;
    while (std::getline(ifs, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        auto k = line.substr(0,pos);
        auto v = line.substr(pos+1);
        if (k == "enabled") state_.enabled = (v == "1");
        else if (k == "duration_days") state_.duration_days = std::stoi(v);
        else if (k == "note") state_.note = v;
    }
    return true;
}

bool WalletStakingManager::Save() const {
    std::ofstream ofs(state_file_, std::ios::trunc);
    if (!ofs) return false;
    ofs << "enabled=" << (state_.enabled ? "1" : "0") << "\n";
    ofs << "duration_days=" << state_.duration_days << "\n";
    ofs << "note=" << state_.note << "\n";
    return true;
}

bool WalletStakingManager::StakeAllFunds(int duration_days, bool dry_run) {
    // This function does not move coins on-chain. It marks wallet metadata indicating the intent
    // to stake all available funds. Integration with wallet transaction construction is required
    // for real on-chain delegation / bonding.
    state_.enabled = true;
    state_.duration_days = duration_days;
    state_.note = dry_run ? "DRY_RUN: stake all" : "STAKE_ALL: active";
    if (!dry_run) return Save();
    return true;
}

bool WalletStakingManager::UnstakeAll(bool dry_run) {
    state_.enabled = false;
    state_.duration_days = 0;
    state_.note = dry_run ? "DRY_RUN: unstake all" : "UNSTAKED";
    if (!dry_run) return Save();
    return true;
}

StakeInfo WalletStakingManager::GetStatus() const { return state_; }

} // namespace staking
