#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>

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
    smatch matches;
    if(regex_search(entry, matches, pattern)){
        return {matches[1], stoi(matches[2])};
    }
    return {"", 0};
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

    vector<int> A_matched(A_list.size(), -1);  // -1 = unmatched
    vector<int> B_matched(B_names.size(), 0);
    vector<int> pref_rank(A_list.size());

    for(int i=0; i<A_prefs.size(); i++){
        for(int k=0; k<A_prefs[i].size(); k++){
            int m=-1;
            for(int j=0; j<B_names.size(); j++){
                if(A_prefs[i][k] == B_names[j]){
                    m = j;
                }
            }
            if(m==-1){
                continue;
            }
            if(B_matched[m]<B_quota[m]){
                A_matched[i] = m;
                B_matched[m]++;
                pref_rank[i] = k+1;
                break;
            }
        }
    }
    for(int i=0; i<A_list.size(); i++){
        if(A_matched[i]!=-1){
            cout<<A_list[i]<<", "<<B_names[A_matched[i]]<<", "<<pref_rank[i]<<endl;
        }
        else
            continue;
    }
    
    return 0;
}
