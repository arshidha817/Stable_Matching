# Stable Matching (IITM MATCH-IT)

[![Compile](https://img.shields.io/badge/g++-C%2B%2B17-green)](https://github.com/Arshidha-IITM/StableMatching)

C++ **Gale-Shapley** for hospital-resident matching with quotas. [IITM MATCH-IT](https://match.iitm.ac.in/matchit/).

## Features
- Parses `@PartitionA/B`, `@PreferenceListsA/B` format
- Hospital quotas: `b1 (2)` = 2 slots  
- Resident-proposing stable matching
- Slot numbering: `a1,b1,1`

## Input Format

| @PartitionA | @PartitionB | @PreferenceListsA | @PreferenceListsB |
|-------------|-------------|-------------------|-------------------|
| `a1; a2; a3; a4;` | `b1 (2), b2 (1), b3 (1);` | `a1: b1, b2, b3;`<br>`a2: b1, b2, b3;`<br>`a3: b1, b2;`<br>`a4: b1, b2;` | `b1: a1, a2, a3, a4;`<br>`b2: a1, a2, a3, a4;`<br>`b3: a1, a2;` |

## Output Format
