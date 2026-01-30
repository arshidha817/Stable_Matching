#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <queue>
#include <fstream>

using namespace std;

void trim(string& s) {
    auto it_start = find_if_not(s.begin(), s.end(), [](unsigned char c){ return isspace(c); });
    s.erase(s.begin(), it_start);

    auto it_end = find_if_not(s.rbegin(), s.rend(), [](unsigned char c){ return isspace(c); });
    s.erase(it_end.base(), s.end());
}

pair<string, int> parseBEntry(string entry){
    trim(entry);
    regex pattern(R"((\w+)\s*\((\d+)\))");
    regex pattern1(R"((\w+)\s*)");
    smatch matches;
    if(regex_search(entry, matches, pattern)){
        return {matches[1], stoi(matches[2])};
    }
    else if(regex_search(entry, matches, pattern1)){
        return {matches[1], 1};
    }
    return {"", 0};     
}

int getRank(const vector<string>& b_prefs, const string& a) {
for (int i = 0; i < b_prefs.size(); ++i) {
if (b_prefs[i] == a) return i;
}
return -1; // b does not prefer a, hence b rejects a
}

int main(){
    vector<string> A_list;
    vector<string> B_names;
    vector<int> B_quota;
    vector<vector<string>> A_prefs;  
    vector<vector<string>> B_prefs; 
    
    string line, section = "";
    bool all_done = false;
    while(getline(cin, line)){ 
        trim(line);
        if (line == "@PartitionA") { section = "A"; continue; }
        if (line == "@PartitionB") { section = "B"; continue; }
        if (line == "@PreferenceListsA") { section = "Apref"; continue; }
        if (line == "@PreferenceListsB") { section = "Bpref"; continue; }
        if (line == "@End"){
            if(section == "Bpref") break;
            else {section = ""; continue; } 
        }
        
        if (section == "A") {
            stringstream ss(line);
            string token;
            while (getline(ss, token, ',')) {
                trim(token);
                if (!token.empty() && token.back() == ';') 
                    token.pop_back();
                if (!token.empty()) 
                    A_list.push_back(token);
            }
        } 
        else if (section == "B") {
            stringstream ss(line);
            string token;
            while (getline(ss, token, ',')) {
                trim(token);
                if (!token.empty() && token.back() == ';') 
                    token.pop_back();
                auto [name, q] = parseBEntry(token);
                if (!name.empty()) {
                    B_names.push_back(name);
                    B_quota.push_back(q);
                }
            }
        }
        else if(section == "Apref"){
            stringstream ss(line);
            string token;
            while (getline(ss, token, ':')) {
                trim(token);
                if (!token.empty() && token.back() == ';') token.pop_back();
                int idx = -1;
                for(int i=0; i<A_list.size(); i++){
                    if(A_list[i] == token){
                        idx = i; break;
                    }
                }
                if(idx!=-1){
                    while (getline(ss, token, ',')) {
                        trim(token);
                    if (!token.empty() && token.back() == ';') 
                        token.pop_back();
                        A_prefs.resize(A_list.size());
                        A_prefs[idx].push_back(token);
                    }
                    }
                }
            }
        else if(section == "Bpref"){
            stringstream ss(line);
            string token;
            while (getline(ss, token, ':')) {
                trim(token);
                if (!token.empty() && token.back() == ';') token.pop_back();
                int idx = -1;
                for(int i=0; i<B_names.size(); i++){
                    if(B_names[i] == token){
                        idx = i; break;
                    }
                }
                if(idx!=-1){
                    while (getline(ss, token, ',')) {
                        trim(token);
                    if (!token.empty() && token.back() == ';') 
                        token.pop_back();
                        B_prefs.resize(B_names.size());
                        B_prefs[idx].push_back(token);
                    }
                }
            }
        }
    }

    int nA = A_list.size();
    int nB = B_names.size();
    vector<int> partnerA(nA, -1);   //index of M(a); if unassigned partnerA[i] = -1
    vector<vector<int>> partnerB(nB);    //partnerB[j] = list of a's assigned to B[j]
    vector<int> nextPrefA(nA, 0);   //tracks the number of partners that a has proposed to

    queue<int> freeA;   //queue of a's who are unmatched. Initially this queue contains all a's

    for (int i = 0; i < nA; ++i) freeA.push(i);

    while (!freeA.empty()) {
        int a = freeA.front(); freeA.pop();
        if (nextPrefA[a] >= A_prefs[a].size()) continue;  // no more preferences left for a to propose to

        string a_name = A_list[a];
        string b_name = A_prefs[a][nextPrefA[a]];
        int b = -1;
        for (int j = 0; j < nB; j++) {
            if (B_names[j] == b_name) { b = j; break; }
        }
        if (b == -1) { ++nextPrefA[a]; continue; }  // if the preferred b != B_names[j], continue and find the j which satidifies b = B_names[j]

        int rank_a = getRank(B_prefs[b], a_name);   //finding the rank of a in b's pref list
        if(rank_a != -1){                           //runs iff b has a in its pref list
            if ((int)partnerB[b].size() < B_quota[b]) { //checks if the no. of partners matched for b <= b's quota
                partnerB[b].push_back(a);
                partnerA[a] = b;
            } 
        
            else {//codeblock that executes the rejection and replacement of current matches with better matches (after the quota is filled)
                // Find worst current match
                int worst_a = -1, worst_rank = -1;
                for (int curr_a : partnerB[b]) {    
                    string curr_name = A_list[curr_a];
                    int curr_rank = getRank(B_prefs[b], curr_name);
                    if (curr_rank > worst_rank) {
                        worst_rank = curr_rank;
                        worst_a = curr_a;
                    }
                }
                if (rank_a < worst_rank) {  // Prefer a (lower rank better)
                    // Reject worst_a
                    partnerA[worst_a] = -1;
                    partnerB[b].erase(find(partnerB[b].begin(), partnerB[b].end(), worst_a));
                    freeA.push(worst_a);
                    // Accept a
                    partnerB[b].push_back(a);
                    partnerA[a] = b;
                } else {
                    // Reject a
                    ++nextPrefA[a];
                    freeA.push(a);
                }
            }
        }
    }

    // Output matching
cout << "-----Output-----"<<'\n';
    for (int i = 0; i < nA; ++i) {
        if (partnerA[i] != -1) {
            cout << A_list[i] << ", " << B_names[partnerA[i]] << ", " << partnerA[i] + 1 << '\n';
        }
    }

    return 0;
}
