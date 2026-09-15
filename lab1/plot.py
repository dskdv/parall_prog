import matplotlib.pyplot as plt

sizes = []
times = []

with open("timings.txt", "r") as f:
    next(f) 
    for line in f:
        n, t = line.strip().split(",")
        sizes.append(int(n))
        times.append(float(t))

plt.figure(figsize=(10, 6))
plt.plot(sizes, times, marker='o', linestyle='-', color='b', linewidth=2)
plt.title('Зависимость времени умножения матриц от её размера')
plt.xlabel('Размер матрицы N')
plt.ylabel('Время выполнения (мс)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.savefig('graph.png', dpi=150)
plt.show()

print("График сохранён в graph.png")