#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <map>
#include <fstream>

using namespace std;

void trim(string& s) {
    auto it_start = find_if_not(s.begin(), s.end(), [](unsigned char c){ return isspace(c); });
    s.erase(s.begin(), it_start);

    auto it_end = find_if_not(s.rbegin(), s.rend(), [](unsigned char c){ return isspace(c); });
    s.erase(it_end.base(), s.end());
}
int getRank(const vector<string>& prefs, const string& agent) {
    for (int i = 0; i < prefs.size(); ++i) {
        if (prefs[i] == agent) return i;
    }
    return -1; 
}
pair<string, int> parseEntry(string entry){
    trim(entry);
    regex pattern(R"((\w+)\s*\((\d+)\))");
    smatch matches;
    if(regex_search(entry, matches, pattern)){
        return {matches[1], stoi(matches[2])};
    }
    return {entry, 1};     
}
int main(int argc, char* argv[]) {
    ifstream file(argv[1]);

    map<string, int> listA;
    map<string, int> listB;
    map<string, vector<string>> prefA;
    map<string, vector<string>> prefB;
    map<string, vector<string>> matchA;  // Who A is matched with
    map<string, vector<string>> matchB;  // Who B is matched with 

    string line, section = "";

    while (getline(file, line)) {
        trim(line);
        if (line.empty()) continue;

        // Detect Section Headers
        if (line.find("@PartitionA") != string::npos) { section = "PartA"; continue; }
        if (line.find("@PartitionB") != string::npos) { section = "PartB"; continue; }
        if (line.find("@PreferenceListsA") != string::npos) { section = "PrefA"; continue; }
        if (line.find("@PreferenceListsB") != string::npos) { section = "PrefB"; continue; }
        if (line.find("@Matching") != string::npos) { section = "Match"; continue; }
        if (line.find("@End") != string::npos) { section = ""; continue; }

        stringstream ss(line);
        string token;

        if (section == "PartA") {
            while (getline(ss, token, ',')) {
                trim(token);
                if (!token.empty() && token.back() == ';') token.pop_back();
                if (!token.empty()) {
                    auto [name, q] = parseEntry(token);
                    listA.insert({name, q});
                }
            }
        } 
        else if (section == "PartB") {
            while (getline(ss, token, ',')) {
                trim(token);
                if (!token.empty() && token.back() == ';') token.pop_back();
                if (!token.empty()) {
                    auto [name, q] = parseEntry(token);
                    listB.insert({name, q});
                }
            }
        }
        else if (section == "PrefA" || section == "PrefB") {
            string key;
            if (getline(ss, key, ':')) {
                trim(key);
                vector<string> prefs;
                while (getline(ss, token, ',')) {
                    trim(token);
                    if (!token.empty() && token.back() == ';') token.pop_back();
                    if (!token.empty()) prefs.push_back(token);
                }
                if (section == "PrefA") prefA[key] = prefs;
                else prefB[key] = prefs;
            }
        }
        else if(section == "Match"){
            string u, v, num;
            
            if (getline(ss, u, ',')) {
                if (getline(ss, v, ',')) { 
                    trim(u); trim(v);
                    if (!v.empty() && v.back() == ';') v.pop_back();                
                    if (!u.empty() && !v.empty()) {
                        if (listA.find(u) != listA.end()) {
                            matchA[u].push_back(v);
                            matchB[v].push_back(u);
                        } else if (listA.find(v) != listA.end()) {
                            matchA[v].push_back(u);
                            matchB[u].push_back(v);
                        }
                    }
                }
            }
        }
    }
    file.close();

    for(const auto& pair : listA) { if(matchA.find(pair.first) == matchA.end()) matchA[pair.first] = {}; }
    for(const auto& pair : listB) { if(matchB.find(pair.first) == matchB.end()) matchB[pair.first] = {}; }

    bool stable = true;

    for (const auto& pair : listA) {
        string a = pair.first;
        int quota_a = pair.second;
        
        vector<string>& current_partners_a = matchA[a];

        // Find A's worst current partner rank
        // (Used if A is full, to exchange with a better partner)
        int a_worst_rank = -1;
        for (const string& partner : current_partners_a) {
            int r = getRank(prefA[a], partner);
            if (r > a_worst_rank) a_worst_rank = r;
        }

        // Iterate A's preference list to find potential blocking partners
        for (const string& b_cand : prefA[a]) {
            
            // Skip if already matched
            bool already_matched = false;
            for(const string& p : current_partners_a) {
                if(p == b_cand) { already_matched = true; break; }
            }
            if (already_matched) continue;

            int rank_b_in_a = getRank(prefA[a], b_cand);

            // -- Check: Does A prefer B_cand? --
            bool a_wants_b = false;
            
            if (current_partners_a.size() < quota_a) {
                // A has space, so A accepts any valid partner
                a_wants_b = true; 
            } else {
                // A is full. Wants B only if B is better than A's worst partner
                if (a_worst_rank != -1 && rank_b_in_a < a_worst_rank) {
                    a_wants_b = true;
                }
            }

            if (!a_wants_b) continue; // A doesn't want to change

            // -- Check: Does B_cand prefer A? --
            
            if (listB.find(b_cand) == listB.end()) continue; // Safety check
            
            int quota_b = listB[b_cand];
            vector<string>& current_partners_b = matchB[b_cand];
            int rank_a_in_b = getRank(prefB[b_cand], a);

            if (rank_a_in_b == -1) continue;

            bool b_wants_a = false;
            
            // Find B's worst current partner
            int b_worst_rank = -1;
            for (const string& partner : current_partners_b) {
                int r = getRank(prefB[b_cand], partner);
                if (r > b_worst_rank) b_worst_rank = r;
            }

            if (current_partners_b.size() < quota_b) {
                // B has space
                b_wants_a = true; 
            } else {
                // B is full. Wants A only if A is better than B's worst partner
                if (b_worst_rank != -1 && rank_a_in_b < b_worst_rank) {
                    b_wants_a = true;
                }
            }

            if (b_wants_a) {
                cout << "Blocking pair: (" << a << "," << b_cand << ")" << endl;
                stable = false;
            }
        }
    }

    if (stable) cout << "Stable" << endl;
    else cout << "Unstable" << endl;

    return 0;
}
    

