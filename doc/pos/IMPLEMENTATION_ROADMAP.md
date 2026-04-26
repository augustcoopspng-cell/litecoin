# PoS Implementation Roadmap (companion to PROPOSAL.md)

This document contains a more detailed staged implementation plan for the PoS migration. It lists the initial set of changes we will land on the feature/pos-design branch and how we will break the work into small, reviewable PRs.

Planned PRs (high level)

1. pos/spec: keep design doc (done).
2. pos/stub: add staking ledger data structures and header extensions, with unit tests (no activation).
3. pos/tests: add consensus simulation harness and regtests.
4. wallet/staking: add wallet-level delegation module (this PR) and CLI/RPC stubs for manual testing.
5. pos/vrf: implement proposer selection and randomness; unit tests.
6. pos/rewards: implement epoch accounting and reward distribution.
7. pos/slashing: implement slashing evidence, processing, and tests.
8. pos/activation: add fork-height guard and activation signalling tests.
9. pos/integration: testnet deployment, documentation, audit fixes.

This branch (feature/pos-design) will hold the design doc and early implementation artifacts (wallet-level delegation and encryption module). Later consensus changes will be landed in small, audited PRs.
