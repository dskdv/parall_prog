#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <string>
#include <omp.h>

using namespace std;

using Clock = chrono::high_resolution_clock;
using Seconds = chrono::duration<double>;

vector<vector<double>> makeMatrix(int n) {
    vector<vector<double>> m(n, vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            m[i][j] = (rand() % 1000) / 100.0;
        }
    }
    return m;
}

vector<vector<double>> mulSeq(const vector<vector<double>>& A,
    const vector<vector<double>>& B,
    int n) {
    vector<vector<double>> C(n, vector<double>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            double aik = A[i][k];
            for (int j = 0; j < n; ++j) {
                C[i][j] += aik * B[k][j];
            }
        }
    }
    return C;
}

vector<vector<double>> mulOmp(const vector<vector<double>>& A,
    const vector<vector<double>>& B,
    int n) {
    vector<vector<double>> C(n, vector<double>(n, 0.0));

#pragma omp parallel for schedule(static)
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            double aik = A[i][k];
            for (int j = 0; j < n; ++j) {
                C[i][j] += aik * B[k][j];
            }
        }
    }
    return C;
}

double maxDiff(const vector<vector<double>>& X,
    const vector<vector<double>>& Y) {
    double d = 0.0;
    int n = (int)X.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double cur = fabs(X[i][j] - Y[i][j]);
            if (cur > d) d = cur;
        }
    }
    return d;
}

double measureOmp(const vector<vector<double>>& A,
    const vector<vector<double>>& B,
    int n,
    int threads,
    double& gflops) {
    omp_set_num_threads(threads);

    auto t0 = Clock::now();
    auto C = mulOmp(A, B, n);
    auto t1 = Clock::now();

    double sec = Seconds(t1 - t0).count();

    double ops = 2.0 * (double)n * (double)n * (double)n;
    gflops = (ops / sec) / 1e9;

    if (C.empty()) return 0.0;
    return sec;
}

void checkCorrectness(int n, int threads) {
    srand(12345);
    auto A = makeMatrix(n);
    auto B = makeMatrix(n);

    omp_set_num_threads(threads);
    auto C_par = mulOmp(A, B, n);
    auto C_seq = mulSeq(A, B, n);

    double err = maxDiff(C_par, C_seq);

    cout << "  Check for N=" << n
        << ", threads=" << threads
        << ": max deviation = "
        << scientific << setprecision(2) << err << endl;

    if (err < 1e-9) {
        cout << "  Result: OK (matches sequential version)" << endl;
    }
    else {
        cout << "  Result: ERROR! Values differ" << endl;
    }
}

int main() {
#ifdef _OPENMP
    cout << "OpenMP supported, version: " << _OPENMP << endl;
#else
    cout << "OpenMP NOT supported by compiler!" << endl;
    return 1;
#endif

    cout << "Available logical processors: " << omp_get_num_procs() << endl;
    cout << "Default max threads: " << omp_get_max_threads() << endl;
    cout << string(80, '=') << endl;

    vector<int> sizes = { 200, 400, 800, 1200, 1600, 2000 };
    vector<int> threads = { 1, 2, 4, 8 };

    cout << "\n[1] Correctness verification of parallel multiplication" << endl;
    cout << string(80, '-') << endl;
    checkCorrectness(200, 4);
    checkCorrectness(400, 8);

    cout << "\n[2] Experiments: size x threads" << endl;
    cout << string(80, '-') << endl;
    cout << left
        << setw(8) << "N"
        << setw(10) << "Threads"
        << setw(14) << "Time, s"
        << setw(12) << "GFLOPS"
        << endl;
    cout << string(80, '-') << endl;

    for (int t : threads) {
        for (int n : sizes) {
            srand(42);
            auto A = makeMatrix(n);
            auto B = makeMatrix(n);

            double gflops = 0.0;
            double sec = measureOmp(A, B, n, t, gflops);

            cout << left
                << setw(8) << n
                << setw(10) << t
                << setw(14) << fixed << setprecision(4) << sec
                << setw(12) << setprecision(3) << gflops
                << endl;
        }
    }

    cout << "\n[3] Summary times table (seconds)" << endl;
    cout << string(80, '-') << endl;
    cout << setw(8) << "N";
    for (int t : threads) cout << setw(12) << ("T=" + to_string(t));
    cout << endl;
    cout << string(8 + 12 * (int)threads.size(), '-') << endl;

    vector<vector<double>> timeTable(sizes.size(), vector<double>(threads.size(), 0.0));

    for (size_t si = 0; si < sizes.size(); ++si) {
        int n = sizes[si];
        cout << setw(8) << n;

        for (size_t ti = 0; ti < threads.size(); ++ti) {
            int t = threads[ti];
            srand(42);
            auto A = makeMatrix(n);
            auto B = makeMatrix(n);

            double gflops = 0.0;
            double sec = measureOmp(A, B, n, t, gflops);
            timeTable[si][ti] = sec;

            cout << setw(12) << fixed << setprecision(4) << sec;
        }
        cout << endl;
    }

    cout << "\n[4] Speedup relative to 1 thread" << endl;
    cout << string(80, '-') << endl;
    cout << setw(8) << "N";
    for (int t : threads) cout << setw(12) << ("T=" + to_string(t));
    cout << endl;
    cout << string(8 + 12 * (int)threads.size(), '-') << endl;

    for (size_t si = 0; si < sizes.size(); ++si) {
        cout << setw(8) << sizes[si];

        double base = timeTable[si][0];
        for (size_t ti = 0; ti < threads.size(); ++ti) {
            double cur = timeTable[si][ti];
            double speedup = (cur > 0) ? (base / cur) : 0.0;
            cout << setw(12) << fixed << setprecision(2) << speedup;
        }
        cout << endl;
    }

    cout << string(80, '=') << endl;
    cout << "Done." << endl;

    return 0;
}