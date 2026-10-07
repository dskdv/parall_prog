import matplotlib.pyplot as plt

plt.rcParams['font.family'] = 'Arial'
plt.rcParams['axes.unicode_minus'] = False

def parse_table(filename, section_marker):
    with open(filename, "r", encoding="utf-16") as f:
        lines = f.readlines()

    start = None
    for i, line in enumerate(lines):
        if section_marker in line:
            start = i
            break
    if start is None:
        raise ValueError(f"Раздел '{section_marker}' не найден в {filename}")

    j = start + 1
    while j < len(lines) and lines[j].strip().startswith("-"):
        j += 1
    header_line = lines[j]
    j += 1
    while j < len(lines) and lines[j].strip().startswith("-"):
        j += 1

    headers = header_line.split()

    sizes = []
    data = {h: [] for h in headers[1:]}

    while j < len(lines):
        row = lines[j].strip()
        if not row or row.startswith("=") or row.startswith("["):
            break
        parts = row.split()
        if len(parts) < len(headers):
            break
        sizes.append(int(parts[0]))
        for k, h in enumerate(headers[1:]):
            data[h].append(float(parts[k + 1]))
        j += 1

    return sizes, headers[1:], data


filename = "results.txt"

sizes, threads_labels, times = parse_table(filename, "[3] Summary times")

plt.figure(figsize=(10, 6))
markers = ['o', 's', '^', 'D']
for i, t in enumerate(threads_labels):
    n_threads = t.replace("T=", "")
    plt.plot(sizes, times[t], marker=markers[i % len(markers)],
             linestyle='-', linewidth=2, label=f"{n_threads} поток(ов)")

plt.title("Зависимость времени умножения матриц от размера")
plt.xlabel("Размер матрицы N")
plt.ylabel("Время выполнения, секунды")
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()
plt.savefig("graph_time.png", dpi=150)
plt.show()
print("Сохранено: graph_time.png")

sizes, threads_labels, speedups = parse_table(filename, "[4] Speedup")

plt.figure(figsize=(10, 6))
for i, t in enumerate(threads_labels):
    if t == "T=1":
        continue
    n_threads = t.replace("T=", "")
    plt.plot(sizes, speedups[t], marker=markers[i % len(markers)],
             linestyle='-', linewidth=2, label=f"{n_threads} поток(ов)")

for t in threads_labels:
    if t == "T=1":
        continue
    n = int(t.replace("T=", ""))
    plt.plot(sizes, [n] * len(sizes), linestyle=':', alpha=0.4,
             color='gray', label=f"Идеал {n}x")

plt.title("Ускорение относительно 1 потока")
plt.xlabel("Размер матрицы N")
plt.ylabel("Ускорение, раз")
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()
plt.savefig("graph_speedup.png", dpi=150)
plt.show()
print("Сохранено: graph_speedup.png")