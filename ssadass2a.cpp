#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <vector>

// find determinant of a square matrix starting from the row 'row'
double det(std::list<std::shared_ptr<std::vector<double>>> &m, int row) {
    if (m.size() == 2) {
        double a, b, c, d;
        auto it = m.begin();
        a = (**it)[row], c = (**it)[row+1];
        it++;
        b = (**it)[row], d = (**it)[row+1];
        return (a * d) - (b * c);
    }

    double res = 0;
    auto it = m.begin();
    for (size_t i = 0; i < m.size(); ++i) {
        // the even rows (else) and odd rows (if)
        if (i & 1) {
            std::shared_ptr<std::vector<double>> v = *it;
            auto temp = it;
            temp++;
            m.erase(it);
            res -= (*v)[row] * det(m, row + 1);
            m.insert(temp, v);
        } else {
            std::shared_ptr<std::vector<double>> v = *it;
            auto temp = it;
            temp++;
            m.erase(it);
            res += (*v)[row] * det(m, row + 1);
            m.insert(temp, v);
        }
        it++;
    }
    return res;
}

int main() {
    std::ifstream in;
    std::ofstream out;
    in.open("input.txt");
    out.open("output.txt");
    char c;
    std::vector<std::vector<double>> v;
    for (size_t i = 0; in >> c; ++i) {
        if (c == '{' || c == ',') {
            v.push_back(std::vector<double>());
            for (size_t j = 0; in >> c; ++j) {
                if (c == '{' || c == ',') {
                    int a;
                    in >> a;
                    v[i].push_back(a);
                } else {
                    break;
                }
            }
        } else {
            break;
        }
    }
    std::list<std::shared_ptr<std::vector<double>>> m;
    for (size_t i = 0; i < v.size(); ++i) {
        m.push_back(
            std::make_shared<std::vector<double>>(std::vector<double>()));
        auto it = m.end();
        it--;

        for (size_t j = 0; j < v.size(); ++j) {
            (*it)->push_back(v[j][i]);
        }
    }

    out << det(m, 0) << '\n';
}
