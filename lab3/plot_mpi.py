import matplotlib.pyplot as plt
from collections import defaultdict

data = {}  # (N, P) -> время, мс
with open("timings.csv", "r") as f:
    next(f)
    for line in f:
        n, p, t = line.strip().split(",")
        data[(int(n), int(p))] = float(t)

sizes = sorted({n for n, _ in data})
procs = sorted({p for _, p in data})

# 1. Время от размера матрицы для разного числа процессов
plt.figure(figsize=(10, 6))
for p in procs:
    plt.plot(sizes, [data[(n, p)] for n in sizes], marker="o", linewidth=2, label=f"{p} проц.")
plt.title("Зависимость времени умножения матриц от размера (MPI)")
plt.xlabel("Размер матрицы N")
plt.ylabel("Время выполнения (мс)")
plt.grid(True, linestyle="--", alpha=0.7)
plt.legend()
plt.savefig("graph_time.png", dpi=150)

# 2. Время от числа процессов для разных размеров
plt.figure(figsize=(10, 6))
for n in sizes:
    plt.plot(procs, [data[(n, p)] for p in procs], marker="o", linewidth=2, label=f"N={n}")
plt.title("Зависимость времени умножения от числа процессов")
plt.xlabel("Число процессов")
plt.ylabel("Время выполнения (мс)")
plt.xticks(procs)
plt.grid(True, linestyle="--", alpha=0.7)
plt.legend()
plt.savefig("graph_procs.png", dpi=150)

# 3. Ускорение S = T1 / Tp
plt.figure(figsize=(10, 6))
for n in sizes:
    t1 = data[(n, 1)]
    plt.plot(procs, [t1 / data[(n, p)] for p in procs], marker="o", linewidth=2, label=f"N={n}")
plt.plot(procs, procs, "k--", label="идеальное")
plt.title("Ускорение параллельной программы")
plt.xlabel("Число процессов")
plt.ylabel("Ускорение S = T1 / Tp")
plt.xticks(procs)
plt.grid(True, linestyle="--", alpha=0.7)
plt.legend()
plt.savefig("graph_speedup.png", dpi=150)

# 4. Эффективность E = S / P
plt.figure(figsize=(10, 6))
for n in sizes:
    t1 = data[(n, 1)]
    plt.plot(procs, [t1 / data[(n, p)] / p for p in procs], marker="o", linewidth=2, label=f"N={n}")
plt.title("Эффективность параллельной программы")
plt.xlabel("Число процессов")
plt.ylabel("Эффективность E = S / P")
plt.xticks(procs)
plt.grid(True, linestyle="--", alpha=0.7)
plt.legend()
plt.savefig("graph_efficiency.png", dpi=150)

# Таблица ускорения для отчёта
print("Ускорение S = T1/Tp:")
print(f"{'N':<8}" + "".join(f"{'P=' + str(p):<10}" for p in procs))
for n in sizes:
    t1 = data[(n, 1)]
    print(f"{n:<8}" + "".join(f"{t1 / data[(n, p)]:<10.2f}" for p in procs))

print("\nГрафики сохранены: graph_time.png, graph_procs.png, graph_speedup.png, graph_efficiency.png")
plt.show()
