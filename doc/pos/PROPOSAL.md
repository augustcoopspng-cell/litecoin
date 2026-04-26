# Proof-of-Stake (PoS) Design Proposal for augustcoopspng-cell/litecoin

Status: Draft
Branch: feature/pos-design
Author: Copilot (@copilot)
Created: 2026-04-26

## Summary
This document proposes a staged design to augment the existing PoW-based Litecoin fork with Proof-of-Stake (PoS) via a hybrid PoW/PoS consensus. The intent is to provide a safe, testable migration path enabling staking, validator selection, and on-chain rewards while minimizing disruption to existing users during transition. This proposal focuses on consensus rules, state transitions, activation strategy, and a staged rollout plan. Implementation-level changes will follow after review and iteration.

## Goals
- Introduce PoS staking and validator participation while preserving security and backward-compatibility where possible.
- Use a hybrid model initially (PoW + PoS) to allow gradual adoption and smoother migration.
- Provide stake-weighted randomized proposer selection (VRF-style) for fairness and resistance to grinding.
- Support delegation (non-custodial where possible), bonding/unbonding, and configurable slashing for equivocation.
- Minimize consensus surface and provide a clear activation & rollback plan.

## Non-goals
- This doc does not include full code-level specifications for every module. Those will be produced in follow-up design PRs.
- It does not implement full BFT finality (that will be considered in a later RFC).

## High-level model
- Hybrid PoW/PoS: Both PoW miners and PoS validators produce blocks. Each block contains a block type flag: POW or POS.
- During a configurable transition epoch, block reward is split between PoW miners and PoS validators according to a parameterized ratio.
- Validators are selected per-slot using a stake-weighted VRF selection algorithm.
- Epochs: fixed-length in blocks (configurable; default 1,440 blocks per epoch ≈ 1 day at 1m target block time).

## Key decisions (defaults)
1. Validator selection: stake-weighted VRF-style randomized selection.
2. Delegation: allowed; minimum validator stake 1000 (configurable), minimum delegation 100.
3. Bonding/unbonding: on-chain bonding with a 14-day unbonding period (configurable between 7–21 days).
4. Slashing: equivocation slashing default 10% of bonded stake; configurable. Optional downtime slashing/penalties available.
5. Rewards: target annual yield ~3–5% distributed per-epoch from a staking reward pool. During transition, block subsidy split e.g. 70% PoW / 30% PoS initially and shiftable.
6. Finality: probabilistic initially; plan optional epoch checkpointing later.
7. Activation: hard fork activated at a coordinated block height following a multi-week signaling window.

## Data model changes
- Introduce staking-specific on-chain objects and state:
  - StakeCredential (stake address / staking key hash)
  - Validator registry entries: bonded amount, public validator key, stake lock height, stake unlock height, punishment flags
  - Epoch ledger summarization for stake distribution and entropy seed for VRF selection
- Block header extensions:
  - block_type: POW | POS
  - pos_signature / validator_proof: VRF proof and validator signature for POS blocks
  - epoch_index, slot_index (for POS proposer selection)

## Stake & validator lifecycle
- Bonding: wallet issues a bonding transaction that deposits a staking output to a special staking UTXO type that records validator key and bonded amount. While bonded, funds are unspendable until unbonding completes (on-chain lock) OR delegations may occur via off-chain signed delegation messages (depending on mode).
- Delegation: a holder can delegate stake to a validator by issuing a Delegation transaction (on-chain metadata or signed off-chain registration referencing validator key). Delegation does not transfer custody unless the pool design requires it.
- Unbonding: once unbond initiated, a configurable unbonding period begins (default 14 days) during which stake does not participate.

## VRF-based validator selection (overview)
- Each epoch, validators register VRF public keys.
- Per-slot, validators compute a VRF output for the current epoch/slot and produce a proof with their block proposal. The VRF is verified by peers and compared against a stake-weighted threshold derived from total stake and per-validator stake.
- VRF randomness seed drawn from previous epoch entropy (derived from block headers) to avoid bias.

## Slashing and penalties
- Equivocation (double-signing the same slot/height by the same validator): detect via event logs and punish by burning a configurable percent (default 10%) of bonded stake and marking validator as banned for a period.
- Optional downtime penalties: if validator misses proposer windows above a threshold, apply small stake loss or reduced rewards.
- Evidence inclusion: slashing evidence submitted on-chain in Evidence transactions; evidence must include signed block headers and proofs.

## Rewards and economics
- Rewards are calculated per-epoch and distributed to bonded validators and delegators proportionally.
- Reward source: either a portion of block subsidy reallocated to PoS rewards, or a dedicated inflationary reward pool defined in genesis/config.
- Reward distribution mechanism: validators receive reward outputs credited to their stake credentials; delegators receive proportional shares according to off-chain/on-chain accounting rules.

## Block validation & fork choice
- Both POW and POS blocks are valid under new rules; fork choice remains longest (heaviest) chain by combined difficulty/weight metric. During hybrid phase, define combined score = alpha * cumulative_pow_difficulty + beta * cumulative_pos_score (configurable weights). After full transition, can switch to pure stake-based scoring if desired.

## Activation & migration
- Activation method: hard fork at target block height H
- Pre-activation: multi-week upgrade window with optional miner/node signaling and testnet runs.
- Replay protection: include consensus-versioning and a replay-protection marker in transactions to prevent replay across old/new chains.
- Rollout plan:
  - Stage 0: Spec review, tests, and benchmarks (devnet)
  - Stage 1: Internal testnet (validators + miners)
  - Stage 2: Public testnet with incentive program
  - Stage 3: Activation mainnet at H after community coordination

## Compatibility & UX
- Wallet changes: new RPCs for staking, validator key management, bonding/delegation, reward claiming, and slashing evidence submission.
- Exchanges & services: must upgrade to handle new block types and reward accounting.
- Node operators: a configuration flag to enable PoS validator mode and to register validator keys.

## Security considerations
- Threat models: long-range attacks, nothing-at-stake, grinding attacks, bribery/censorship
- Mitigations: VRF randomness, slashing, epoch-based checkpointing as later option, external audits
- Required audits: cryptographic review of VRF, staking state, evidence handling, and reward math

## Testing plan
- Unit tests for: stake accounting, bonding/unbonding, VRF verification, slashing evidence handling
- Integration tests/regtests for: hybrid consensus operation, fork choice under mixed blocks, attack scenarios (double-sign, grinding)
- Staged networks: devnet -> private testnet -> public testnet with incentivized validators

## Implementation roadmap (module-by-module)
1. Design/spec approval (this document) — current step
2. Add staking primitives & data structures to consensus layer (small PR): staking UTXO type, validator registration, epoch ledger
3. VRF subsystem & randomness oracle (PR) — library selection and verification
4. POS block proposal & validation (PR)
5. Fork choice changes & combined weight scoring (PR)
6. Wallet RPCs & UX changes (PR)
7. Tests, benchmarks, audits (PRs)
8. Testnet rollout & monitoring scripts

## Operational checklist for activation
- Publish timeline and activation height H
- Provide precompiled validator tooling & node images
- Coordinate with exchanges and major service operators
- Run public testnet for minimum 6 weeks
- Perform third-party audits and bug bounty

## Open questions
- Exact reward math and inflation schedule (tuneable parameter; needs economic modeling)
- Exact epoch length and slot granularity vs existing block time
- Details of delegation protocol (on-chain vs off-chain)

## Appendices
- References: Tendermint, Ouroboros, Ethereum Beacon Chain, Cardano staking docs
- Glossary: Epoch, Slot, VRF, Bonding, Delegation, Equivocation


---

Draft created and committed to branch feature/pos-design. This is intentionally a high-level design to get stakeholder feedback; implementation-level specs and code-level changes will follow after review and acceptance.
