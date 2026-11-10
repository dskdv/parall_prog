import random
import sys

N = int(sys.argv[1]) if len(sys.argv) > 1 else 3

def generate_matrix(filename, n):
    with open(filename, "w") as f:
        f.write(f"{n}\n") 
        for _ in range(n):
            row = [str(random.randint(0, 9)) for _ in range(n)]
            f.write(" ".join(row) + "\n")

generate_matrix("matrix1.txt", N)
generate_matrix("matrix2.txt", N)

print(f"Сгенерированы матрицы {N}x{N} в matrix1.txt и matrix2.txt")