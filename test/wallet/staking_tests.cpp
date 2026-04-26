#include "wallet/staking_wallet.h"
#include <iostream>

int main() {
    staking::WalletStakingManager mgr("./testdata");
    mgr.Init();
    mgr.StakeAllFunds(365, true);
    auto s = mgr.GetStatus();
    std::cout << "dry-run enabled=" << s.enabled << " days=" << s.duration_days << " note=" << s.note << std::endl;
    mgr.StakeAllFunds(365, false);
    s = mgr.GetStatus();
    std::cout << "live enabled=" << s.enabled << " days=" << s.duration_days << " note=" << s.note << std::endl;
    mgr.UnstakeAll(false);
    s = mgr.GetStatus();
    std::cout << "after unstake enabled=" << s.enabled << " days=" << s.duration_days << " note=" << s.note << std::endl;
    return 0;
}
