#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

bool isConcat(std::string &s, int pos, std::vector<std::string> &v,
              std::unordered_set<int> st) {
    if (st.size() == v.size()) return 1;

    for (int i = 0; i < v.size(); ++i) {
        bool ok = 1;
        if (st.find(i) != st.end()) continue;

        if (v[i].size() > s.size() - pos) continue;

        for (int j = 0; j < v[i].size(); ++j) {
            if (s[pos + j] != v[i][j]) {
                ok = 0;
                break;
            }
        }

        if (ok) {
            st.insert(i);
            if (isConcat(s, pos + v[i].size(), v, st)) return 1;
            st.erase(i);
        }
    }
    return 0;
}

int main() {
    std::ifstream in;
    std::ofstream out;
    in.open("input.txt");
    out.open("output.txt");

    std::string s, temp;
    in >> s;

    std::vector<std::string> v;
    while (in >> temp) {
        v.push_back(temp);
    }

    for (int i = 0; i < s.size(); ++i) {
        std::unordered_set<int> st;
        if (isConcat(s, i, v, st)) out << i << ' ';
    }
}
