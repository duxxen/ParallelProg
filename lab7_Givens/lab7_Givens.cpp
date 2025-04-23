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

class Barrier {
public:
    explicit Barrier(size_t num_threads) : count(num_threads), original_count(num_threads) {}

    void wait() {
        std::unique_lock<std::mutex> lock(m);
        if (--count == 0) {
            count = original_count;
            cv.notify_all();
        } else {
            cv.wait(lock, [this]() { return count == original_count; });
        }
    }

private:
    std::mutex m;
    std::condition_variable cv;
    size_t count;
    size_t original_count;
};

void apply_givens_rotation(Matrix& A, Vector& b, int i, int k, int N) {
    double a = A[i][k];
    double b_ = A[k][k];

    double r = sqrt(a * a + b_ * b_);
    if (r < EPSILON) return;

    double c = b_ / r;
    double s = -a / r;

    for (int j = k; j < N; ++j) {
        double temp1 = c * A[k][j] - s * A[i][j];
        double temp2 = s * A[k][j] + c * A[i][j];
        A[k][j] = temp1;
        A[i][j] = temp2;
    }

    double temp_b1 = c * b[k] - s * b[i];
    double temp_b2 = s * b[k] + c * b[i];
    b[k] = temp_b1;
    b[i] = temp_b2;
}

void forward_elimination(Matrix& A, Vector& b, int num_threads) {
    int N = A.size();
    Barrier barrier(num_threads);

    for (int k = 0; k < N - 1; ++k) {
        auto worker = [&](int thread_id) {
            for (int i = k + 1 + thread_id; i < N; i += num_threads) {
                apply_givens_rotation(A, b, i, k, N);
            }
            barrier.wait();
        };

        std::vector<std::thread> threads;
        for (int t = 0; t < num_threads; ++t)
            threads.emplace_back(worker, t);

        for (auto& t : threads)
            t.join();
    }
}

void forward_elimination(Matrix& A, Vector& b) {
    int N = A.size();
    for (int k = 0; k < N - 1; ++k) {
        for (int i = k + 1; i < N; ++i) {
            if (fabs(A[i][k]) > EPSILON) {
                apply_givens_rotation(A, b, i, k, N);
            }
        }
    }
}

Vector back_substitution(const Matrix& A, const Vector& b) {
    int N = A.size();
    Vector x(N, 0.0);

    for (int i = N - 1; i >= 0; --i) {
        x[i] = b[i];
        for (int j = i + 1; j < N; ++j)
            x[i] -= A[i][j] * x[j];
        if (abs(A[i][i]) < EPSILON) throw std::runtime_error("Zero on diagonal");
        x[i] /= A[i][i];
    }

    return x;
}

void read_data(std::ifstream& fin, Matrix& mtr, Vector& vct) {
    std::string         tempstr;
    std::istringstream  inputstr;

    std::getline(fin, tempstr);
    inputstr = std::istringstream(tempstr);
    auto size = std::count(tempstr.begin(), tempstr.end(), ' ') + 1;

    vct.resize(size, 0.0);
    mtr.resize(size, vct);

    for (int i = 0; std::getline(inputstr, tempstr, ' '); i++) {
        std::istringstream(tempstr) >> vct[i];
    }
    for (int i = 0; std::getline(fin, tempstr); i++) {
        inputstr = std::istringstream(tempstr);
        for (int j = 0; std::getline(inputstr, tempstr, ' '); j++) {
            std::istringstream(tempstr) >> mtr[i][j];
        }
    }
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "Not enough arguments!\n";
        return -1;
    }

    int threads = 0;
    std::istringstream(argv[2]) >> threads;
    
    std::ifstream fin(argv[1]);

    if (!fin.is_open()) {
        std::cerr << "Cannot open the file " << argv[1] << "!\n";
        return -2;
    }

    Matrix mtr;
    Vector vct;
    read_data(fin, mtr, vct);

    auto tbeg = std::chrono::system_clock::now();
    if (threads) {
        std::cout << "forward parallel\n";
        forward_elimination(mtr, vct, threads);
    }
    else {
        std::cout << "forward sequential\n";
        forward_elimination(mtr, vct);
    }
    try {
        back_substitution(mtr, vct);
    }
    catch (std::runtime_error e) {
        std::cerr << e.what() << std::endl;
        return -3;
    }
    auto tend = std::chrono::system_clock::now();
    auto tdur = std::chrono::duration_cast<std::chrono::milliseconds>(tend - tbeg).count();
    std::cout << "Time: " << tdur << std::endl;

    return 0;
}
