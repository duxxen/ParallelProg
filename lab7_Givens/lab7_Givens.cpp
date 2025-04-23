#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <condition_variable>

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

const double EPSILON = 1e-10;

// Вычисление sin и cos для Гивенса
void computeGivens(double a, double b, double& c, double& s) {
    double r = hypot(a, b);
    c = a / r;
    s = -b / r;
}

// Применение поворота Гивенса к строкам i и j
void applyGivens(Matrix& A, int i, int j, int col, double c, double s) {
    for (int k = col; k < A[0].size(); ++k) {
        double temp_i = c * A[i][k] - s * A[j][k];
        double temp_j = s * A[i][k] + c * A[j][k];
        A[i][k] = temp_i;
        A[j][k] = temp_j;
    }
}

// Прямой ход (параллельный)
void forwardElimination(Matrix& A, int num_threads) {
    int n = A.size();
    for (int j = 0; j < n - 1; ++j) {
        auto worker = [&](int tid) {
            for (int i = j + 1 + tid; i < n; i += num_threads) {
                if (fabs(A[i][j]) > EPSILON) {
                    double c, s;
                    computeGivens(A[j][j], A[i][j], c, s);
                    applyGivens(A, j, i, j, c, s);
                }
            }
            };

        std::vector<std::thread> threads;
        for (int t = 0; t < num_threads; ++t)
            threads.emplace_back(worker, t);
        for (auto& th : threads)
            th.join();
    }
}

// Обратный ход (последовательно)
Vector backSubstitution(const Matrix& A) {
    int n = A.size();
    Vector x(n);
    for (int i = n - 1; i >= 0; --i) {
        double sum = A[i][n];
        for (int j = i + 1; j < n; ++j)
            sum -= A[i][j] * x[j];
        x[i] = sum / A[i][i];
    }
    return x;
}

void read_matrix(std::ifstream& fin, Matrix& mtr) {
    std::string tempstr = "";
    for (int i = 0; std::getline(fin, tempstr); i++) {
        if (i == 0) {
            auto size = std::count(tempstr.begin(), tempstr.end(), ' ');
            mtr.resize(size, Vector(size + 1, 0));
        }

        std::istringstream inputrow(tempstr);
        for (int j = 0; std::getline(inputrow, tempstr, ' '); j++) {
            std::istringstream(tempstr) >> mtr[i][j];
        }
    }
}

int main(int argc, char* argv[])
{
    
}
