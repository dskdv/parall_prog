import std;

int main() {

    std::ifstream f1("matrix1.txt");
    std::ifstream f2("matrix2.txt");
    std::ofstream out("result.txt");

    if (!f1.is_open() || !f2.is_open()) {
        std::println(std::cerr, "Ошибка: не удалось открыть matrix1.txt или matrix2.txt");
        return 1;
    }

    int N;
    f1 >> N;
    f2 >> N;

    std::vector<std::vector<int>> A(N, std::vector<int>(N));
    std::vector<std::vector<int>> B(N, std::vector<int>(N));
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            f1 >> A[i][j];

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            f2 >> B[i][j];

    f1.close();
    f2.close();

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    out << N << '\n';
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            out << C[i][j] << ' ';
        }
        out << '\n';
    }
    out.close();
    
    std::println("Размер матрицы: {}x{}", N, N);
    std::println("Время умножения: {} мс", duration.count());

    return 0;
}