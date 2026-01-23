# Stable Matching (IITM MATCH-IT)

C++ implementation of **Gale-Shapley algorithm** for hospital-resident matching with quotas.

[![Compile](https://img.shields.io/badge/g++-C%2B%2B17-green)](https://github.com/Arshidha-IITM/StableMatching)

## Features
- Parses @PartitionA/B, @PreferenceListsA/B format
- Handles B quotas: `b1 (2)` = 2 slots
- Resident-proposing stable matching

## Quick Start
```bash
g++ -std=c++17 stable_matching.cpp -o matching
./matching < sample.in
