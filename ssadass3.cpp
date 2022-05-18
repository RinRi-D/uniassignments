#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

class Matrix {
    std::vector<std::vector<double>> m_data;

   public:
    Matrix() {}

    // Copy constructor combined with transpose
    Matrix(const Matrix& m, bool transpose = 0) {
        const std::vector<std::vector<double>>& data = m.GetData();
        if (transpose) {
            if (data.size()) {
                int rows = data[0].size(), cols = data.size();
                m_data.resize(rows);
                for (auto& i : m_data) i.resize(cols, 0);

                for (int i = 0; i < rows; ++i)
                    for (int j = 0; j < cols; ++j) m_data[i][j] = data[j][i];
            }
        } else {
            m_data = data;
        }
    }

    // Move constructor
    Matrix(Matrix&& m) { m_data = std::move(m.GetData()); }

    // Constructor with size
    Matrix(size_t rows, size_t cols) {
        m_data.resize(rows);
        for (auto& i : m_data) i.resize(cols, 0);
    }

    // reference-Getter of m_data
    std::vector<std::vector<double>>& GetData() { return m_data; }

    // Getter of m_data
    const std::vector<std::vector<double>>& GetData() const { return m_data; }

    size_t GetRows() const { return m_data.size(); }

    size_t GetCols() const {
        if (m_data.size() == 0) return 0;
        return m_data[0].size();
    }

    // Matrix multiplication operator
    friend Matrix operator*(const Matrix& m1, const Matrix& m2);

    // Inverse matrix using Gaussian elimination
    static Matrix Inverse(const Matrix& m) {
        int rows = m.GetRows(), cols = m.GetCols();
        if (rows != cols)
            throw std::invalid_argument("Inverse is only for square matrices!");

        Matrix from(m), res(rows, rows);
        std::vector<std::vector<double>>&datares = res.GetData(),
        data = from.GetData();

        for (int i = 0; i < rows; ++i) datares[i][i] = 1;

        for (int i = 0; i < rows; ++i) {
            if (data[i][i] == 0.0)
                throw std::invalid_argument("No inverse for this matrix!");

            for (int j = 0; j < rows; ++j) {
                if (i != j) {
                    double ratio = data[j][i] / data[i][i];

                    for (int k = 0; k < rows; ++k)
                        data[j][k] -= ratio * data[i][k];

                    for (int k = 0; k < rows; ++k)
                        datares[j][k] -= ratio * datares[i][k];
                }
            }
        }

        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < rows; ++j) datares[i][j] /= data[i][i];

        return res;
    }

    void print(std::ostream& out) const {
        for (const auto& i : m_data) {
            for (const auto& j : i) out << j << ' ';
            out << '\n';
        }
    }
};

Matrix operator*(const Matrix& m1, const Matrix& m2) {
    size_t rows = m1.GetRows(), cols = m2.GetCols(), mid = m1.GetCols();
    if (mid != m2.GetRows()) throw std::invalid_argument("Couldn't multiply!");

    Matrix res(rows, cols);
    const std::vector<std::vector<double>>&data1 = m1.GetData(),
          &data2 = m2.GetData();
    std::vector<std::vector<double>>& datares = res.GetData();

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            datares[i][j] = 0;
            for (size_t k = 0; k < mid; ++k) {
                datares[i][j] += data1[i][k] * data2[k][j];
            }
        }
    }

    return res;
}

inline std::ostream& operator<<(std::ostream& out, const Matrix& m) {
    m.print(out);
    return out;
}

int main() {
    std::ifstream inputtxt("input.txt");
    std::ofstream outputtxt("output.txt");

#ifdef DEBUG
    std::istream& in = inputtxt;
    std::ostream& out = std::cout;
#else
    std::istream& in = inputtxt;
    std::ostream& out = outputtxt;
#endif
    out.precision(2);

    int n, m;
    in >> n >> m;
    Matrix A(m, n + 1), Y(m, 1);

    // input
    std::vector<std::vector<double>>&a = A.GetData(), &y = Y.GetData();
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            if (j == 0) a[i][j] = 1;
            if (j < n)
                in >> a[i][j + 1];
            else
                in >> y[i][0];
        }
    }

    out << "A:\n" << std::fixed << A << '\n';
    out << "b:\n" << std::fixed << Y << '\n';

    Matrix A_T(A, true),           // A transpose
        A_TA(std::move(A_T * A)),  // A transpose multiplied by A
        A_TA_inv(
            std::move(Matrix::Inverse(A_TA))),  // inverse of the previous one
        A_TA_invA_T(std::move(A_TA_inv * A_T)), // multiplied by A transpose
        ans(std::move(A_TA_invA_T * Y)); // answer x

    out << "A_T*A:\n" << std::fixed << A_TA << '\n';
    out << "(A_T*A)_-1:\n" << std::fixed << A_TA_inv << '\n';
    out << "(A_T*A)_-1*A_T:\n" << std::fixed << A_TA_invA_T << '\n';
    out << "x:\n" << std::fixed << ans << '\n';
}
