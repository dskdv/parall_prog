import numpy as np

def read_matrix(filename):
    with open(filename, "r") as f:
        n = int(f.readline().strip())
        rows = [list(map(int, f.readline().split())) for _ in range(n)]
    return np.array(rows)

A = read_matrix("matrix1.txt")
B = read_matrix("matrix2.txt")
C_my = read_matrix("result.txt")

C_etalon = np.dot(A, B)

if np.array_equal(C_my, C_etalon):
    print("Верификация пройдена: результаты C++ и NumPy совпадают.")
else:
    diff = np.abs(C_my - C_etalon)
    print("Ошибка!: результаты не совпадают!")
    print(f"Максимальное расхождение: {diff.max()}")
    print(f"Несовпадающих элементов: {np.count_nonzero(diff)}")