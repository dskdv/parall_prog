// Лабораторная работа №3: параллельное умножение матриц по технологии MPI
// Основано на программе из л/р №1 (тот же алгоритм i-j-k, те же файлы ввода/вывода).
//
// Схема распараллеливания:
//   1. Процесс 0 читает matrix1.txt (A) и matrix2.txt (B).
//   2. Матрица B рассылается всем процессам (MPI_Bcast).
//   3. Строки матрицы A раздаются процессам (MPI_Scatterv) - каждый получает
//      свой блок строк (если N не делится на число процессов, часть процессов
//      получает на одну строку больше).
//   4. Каждый процесс считает свой блок строк результата C.
//   5. Блоки собираются на процессе 0 (MPI_Gatherv), он пишет result.txt.
//
// Сборка:  mpicxx -O2 -std=c++17 lab3.cpp -o lab3
// Запуск:  mpirun -np 4 ./lab3

#include <mpi.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdio>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0, size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 0;
    std::vector<int> A;  // только на процессе 0 (целиком)
    std::vector<int> B;  // на всех процессах (целиком)
    std::vector<int> C;  // только на процессе 0 (целиком)

    // ---------- Чтение данных (процесс 0) ----------
    if (rank == 0) {
        std::ifstream f1("matrix1.txt");
        std::ifstream f2("matrix2.txt");

        if (!f1.is_open() || !f2.is_open()) {
            std::cerr << "Ошибка: не удалось открыть matrix1.txt или matrix2.txt" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        f1 >> N;
        f2 >> N;

        A.resize((size_t)N * N);
        B.resize((size_t)N * N);
        C.resize((size_t)N * N, 0);

        for (int i = 0; i < N * N; i++) f1 >> A[i];
        for (int i = 0; i < N * N; i++) f2 >> B[i];
    }

    // Размер матрицы нужен всем
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) B.resize((size_t)N * N);

    // ---------- Разбиение строк между процессами ----------
    std::vector<int> sendcounts(size), displs(size);
    int base = N / size, rem = N % size, offset = 0;
    for (int p = 0; p < size; p++) {
        int rows_p = base + (p < rem ? 1 : 0);
        sendcounts[p] = rows_p * N;   // количество элементов, а не строк
        displs[p] = offset;
        offset += sendcounts[p];
    }
    int my_rows = sendcounts[rank] / N;

    std::vector<int> localA((size_t)my_rows * N);
    std::vector<int> localC((size_t)my_rows * N, 0);

    // ---------- Замер времени ----------
    // Измеряем всё параллельное вычисление целиком: рассылка + умножение + сбор
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    MPI_Bcast(B.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatterv(A.data(), sendcounts.data(), displs.data(), MPI_INT,
                 localA.data(), sendcounts[rank], MPI_INT,
                 0, MPI_COMM_WORLD);

    // Умножение своего блока строк (алгоритм из л/р №1)
    for (int i = 0; i < my_rows; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += localA[(size_t)i * N + k] * B[(size_t)k * N + j];
            }
            localC[(size_t)i * N + j] = sum;
        }
    }

    MPI_Gatherv(localC.data(), sendcounts[rank], MPI_INT,
                C.data(), sendcounts.data(), displs.data(), MPI_INT,
                0, MPI_COMM_WORLD);

    double end = MPI_Wtime();
    double local_ms = (end - start) * 1000.0;

    // Время всей программы = время самого медленного процесса
    double time_ms = 0.0;
    MPI_Reduce(&local_ms, &time_ms, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // ---------- Запись результата и вывод (процесс 0) ----------
    if (rank == 0) {
        std::ofstream out("result.txt");
        out << N << '\n';
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                out << C[(size_t)i * N + j] << ' ';
            }
            out << '\n';
        }
        out.close();

        std::printf("Размер матрицы: %dx%d\n", N, N);
        std::printf("Число процессов: %d\n", size);
        std::printf("Время умножения: %.3f мс\n", time_ms);
    }

    MPI_Finalize();
    return 0;
}
