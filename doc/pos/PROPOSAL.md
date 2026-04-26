# Proof-of-Stake (PoS) Proposal for augustcoopspng-cell/litecoin

Overview

This document proposes a staged migration from a pure Proof-of-Work (PoW) consensus to a hybrid PoW/PoS model for the augustcoopspng-cell/litecoin fork. It defines high-level consensus changes, validator selection, staking and delegation, reward and slashing mechanics, activation/migration strategy, testing and rollout plans, and security considerations. This proposal represents the initial specification and is intended to be reviewed, iterated, and accepted by project maintainers before any consensus-code changes are merged.

Goals

- Introduce PoS to enable low-energy validation, improved decentralization of block production over time, and flexible staking/delegation models.
- Minimize risk during transition by starting with a hybrid PoW/PoS model.
- Provide a clear migration and activation plan to coordinate node operators, exchanges, and miners.
- Preserve user security by requiring explicit opt-in for staking and protecting keys and bonded stakes.

Recommended Defaults (used in this initial spec)

- Hybrid PoW/PoS initial model; epoch = 1,440 blocks (~1 day at 1-minute blocks).
- Validator selection: stake-weighted VRF-style randomized proposer selection.
- Delegation: allow delegation; minimum validator stake = 1000 LTC, minimum delegation = 100 LTC.
- Bonding/unbonding: on-chain bonding with a 14-day unbonding period.
- Slashing: equivocation default = 10% of bonded stake; optional downtime penalties configurable.
- Rewards: target annual yield = 3–5% distributed per-epoch from a staking reward pool; transitional PoW/PoS subsidy split.
- Finality: probabilistic initially; plan for optional epoch checkpoints and future BFT finality.
- Activation: hard-fork activation at a coordinated block height with a multi-week signaling and upgrade window.

High-level Consensus Changes

1. Block structure
- Extend the block header with a new PoS section that includes: epoch identifier, proposer identifier (validator ID or pubkey), VRF proof or selection proof, and optional PoS-specific signatures necessary for validation.
- Maintain backward compatibility with transaction encoding where possible.

2. State transition
- Introduce a staking ledger in chain state that tracks bonded stake, delegation records, validator metadata, and unbonding queues.
- Define epoch boundaries and per-epoch reward distribution calculations.

3. Chain selection
- Initially keep the canonical longest-chain rule modified to accept PoS-authored blocks according to proposer selection. In hybrid mode, both valid PoW and PoS blocks are accepted; chain selection gives weight to the combined total of validated work + stake-derived score (detailed formula in Appendix).

Validator Selection & Proposer Algorithm

- Use stake-weighted randomized selection based on VRF outputs seeded with recent epoch randomness (mix of recent block hashes and validator contributions) to elect a proposer set per slot/block.
- Validators register by bonding an on-chain deposit (bond amount) and publishing a validator-registration transaction containing their public key and staking metadata.
- Delegation is allowed: delegators register an on-chain delegation to a validator, increasing its effective stake for selection.

Staking Economics & Rewards

- Introduce a staking reward pool funded by a configured portion of block subsidy (split between PoW miners and PoS validators during transition) and optionally by a small epoch inflation.
- Reward distribution occurs at epoch boundaries and is distributed proportionally to bonded stake (including delegated stake) after slashing deductions and protocol fees, if any.

Bonding, Unbonding & Withdrawal

- Bonding requires an on-chain transaction that locks coins into the staking ledger; these coins are not spendable while bonded.
- Unbonding starts a 14-day cooldown (configurable) after which the stake becomes withdrawable back to a spendable wallet UTXO.
- The staking ledger includes an unbonding queue to prevent instant withdrawal and to manage slashing windows.

Slashing & Penalties

- Slashing conditions: double-signing / equivocation, provable misbehavior (as defined by signed evidence), and optional long downtime windows.
- Default equivocation slashing penalty: 10% of the bonded stake (configurable) with configurable portion burned / sent to a penalty pool.
- Slashing proofs are included as on-chain evidence transactions and processed by consensus rules at inclusion.

Epochs, Finality & Fork Handling

- Epoch length: 1,440 blocks by default. Epochs are used for validator rotation, reward distribution, and randomness updates.
- Finality: initial design uses probabilistic finality; a later RFC will propose BFT-style checkpoint finality if desired.
- Long-range attack mitigations: include checkpointing, slashing windows, and an optional weak subjectivity checkpoint mechanism for light clients.

Activation & Migration Strategy

- Activation will be by coordinated hard fork at a specific block height H_ACT.
- Upgrade timeline proposal: announce at least 8 weeks prior, followed by 4-week testing period, 2-week signaling window (BIP9-like), and final activation height at least 2 weeks after signaling end. Exact timing adjustable by maintainers.
- Node software versions must include replay-protection measures and upgrade notifications for exchanges and custodial services.
- Provide tools to replay chain state into a PoS-aware node and scripts for validators to prepare keys, bonds, and testnet validators.

Replay Protection & Compatibility

- The fork will change consensus rules: PoW-only clients that do not upgrade will follow the legacy chain from the activation height; upgraded nodes will follow the new PoW/PoS chain. This will cause a permanent chain split relative to legacy Litecoin networks.
- Include serialization/version bumps and transaction or header version flags to help exchanges and tooling detect new chain.

Wallet UX & RPCs

- Wallet-level features:
  - staking-cli: bond/unbond, delegate, withdraw, validator registration, reward withdrawal.
  - RPC endpoints: listvalidators, getvalidatorinfo, bond, unbond, delegate, withdrawstaking, stakingstatus.
  - Cold-staking options: allow signing-only validator keys kept offline while staking transactions are produced via watch-only or remote-signing mechanisms.

Security, Testing & Audit Plan

- Testnets: staged rollout on devnet -> private testnet -> public testnet -> mainnet.
- Tests: unit tests for staking ledger, consensus integration tests (fork choice, slashing), long-range attack simulations, validator rotation fuzzing, and regtests for upgrade activation.
- Formal verification: critical consensus components (slashing, staking state transitions, VRF verification) should be audited and reviewed; consider formal proofs for critical invariants.
- External audits: engage third-party auditors to review core consensus changes before mainnet activation.

Operational Requirements for Validators & Exchanges

- Validator operators must run updated node software, generate and protect validator keys, and stake minimum bonded amounts.
- Exchanges and custodial services must decide whether to upgrade and support the new chain or maintain legacy support; they must implement replay protection and notify users.

Implementation Roadmap (staged PRs)

1. Design & spec (this document) — review and signoff.
2. Core consensus skeleton: add staking ledger data structures, header extensions, and block validation hooks (no activation flag set).
3. Test harness & regtests: add unit tests and simulation utilities; private testnet.
4. Validator registration & staking transactions: transaction types and RPC support.
5. Proposer selection & VRF: randomness and selection logic with tests.
6. Reward distribution, slashing implementation, and epoch processing.
7. Activation changes behind a fork-height guard; extensive testnet testing.
8. Mainnet activation and post-activation monitoring.

Appendices

- Appendix A: detailed block header and tx-format sketches (to be expanded in follow-up revision).
- Appendix B: reward math formulas and example epoch accounting (to be provided).
- Appendix C: security checklist and recommended audit scope.

Next steps

- Review this proposal and provide feedback or requested modifications (especially parameters: minimum stake, epoch length, slashing percentages, reward targets).
- Once the design is accepted, we will proceed with the staged implementation roadmap and create narrowly-scoped PRs for each milestone.

Commit note

This file is the initial PoS proposal; treat it as a living document and the authoritative spec for subsequent consensus changes.
