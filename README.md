# Stable Matching (IITM MATCH-IT)

[![Compile](https://img.shields.io/badge/g++-C%2B%2B17-green)](https://github.com/Arshidha-IITM/StableMatching)

C++ **Gale-Shapley** implementation for hospital-resident matching with quotas. Built for [IITM MATCH-IT contest](https://match.iitm.ac.in/matchit/).

## 🚀 Features
- Parses exact IITM `@PartitionA/B`, `@PreferenceListsA/B` format
- Hospital quotas: `b1 (2)` = accepts 2 residents maximum
- **Resident-proposing** stable matching (A-optimal)
- Correct slot numbering in output (`a1,b1,1` = slot #1 of b1's quota)

## 📋 Input Format

@PartitionA
a1; a2; a3; a4; # Residents (A side)
@End

@PartitionB
b1 (2), b2 (1), b3 (1); # Hospitals (B side) with quotas
@End

@PreferenceListsA
a1: b1, b2, b3; # Each A lists preferred B's (top = most preferred)
a2: b1, b2, b3;
a3: b1, b2;
a4: b1, b2;
@End

@PreferenceListsB
b1: a1, a2, a3, a4; # Each B lists preferred A's
b2: a1, a2, a3, a4;
b3: a1, a2;
@End

text

## 📤 Output Format
a1, b1, 1 # Resident a1 matched to hospital b1, slot #1
a2, b1, 2 # Resident a2 matched to hospital b1, slot #2
a3, b2, 1 # Sequential slot numbering per hospital quota
a4, b3, 1

text

## 🛠 Quick Start
```bash
g++ -std=c++17 stable_matching.cpp -o matching
./matching < sample.in
🏗 Build Requirements
C++17 (-std=c++17)

Standard library only: vector, string, regex, sstream

No external dependencies

📁 Repository Structure
text
StableMatching/
├── stable_matching.cpp    # Main implementation
├── README.md             # This file
├── .gitignore           # C++ build artifacts
└── LICENSE              # MIT
🎓 Algorithm
Hospital-Resident (Many-to-One) Stable Matching using Gale-Shapley:

Free residents propose to their top hospital choice

Hospitals provisionally accept best-ranked resident (up to quota)

Rejected residents propose to next choice

Continues until no rejections occur

Guarantee: Stable matching exists and algorithm terminates.

🤝 Contributing
Fork repo

Create feature branch (git checkout -b feature/AmazingFeature)

Commit changes (git commit -m 'Add some AmazingFeature')

Push branch (git push origin feature/AmazingFeature)

Open Pull Request

📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
