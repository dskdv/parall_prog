import subprocess
import re

SIZES = [200, 400, 800, 1200, 1600, 2000]
REPEATS = 3

BASE_DIR = r"C:\Users\darya\OneDrive\Desktop\paral_prog\lab1"
GENERATE = BASE_DIR + r"\generate.py"
EXE = r"C:\Users\darya\OneDrive\Desktop\paral_prog\out\build\x64-Debug\lab1\lab1.exe"
TIMINGS = BASE_DIR + r"\timings.txt"

PATTERN = re.compile(r"Время умножения:\s*(\d+)\s*мс")

def run_once(size):
    result = subprocess.run(
        [EXE],
        capture_output=True,
        text=True,
        encoding="utf-8"
    )
    if result.returncode != 0:
        print(f"Ошибка! при запуске exe для N={size}")
        print("STDOUT:", result.stdout)
        print("STDERR:", result.stderr)
        raise SystemExit(1)

    match = PATTERN.search(result.stdout)
    if not match:
        print(f"Не удалось распарсить вывод для N={size}:")
        print(result.stdout)
        raise SystemExit(1)

    return int(match.group(1))

results = {}

for N in SIZES:
    print(f"\nРазмер матрицы: {N}x{N}")

    # 1. Сгенерировать матрицы нужного размера
    subprocess.run(["python", GENERATE, str(N)], check=True)

    # 2. Три раза запустить exe и собрать времена
    times = []
    for run in range(REPEATS):
        t = run_once(N)
        times.append(t)
        print(f"  Запуск {run + 1}: {t} мс")

    avg = sum(times) / len(times)
    results[N] = avg
    print(f"  Среднее для N={N}: {avg:.2f} мс")

# 3. Итоговая таблица
print("\n\nИТОГОВАЯ ТАБЛИЦА")
print(f"{'Размер (N)':<15}{'Среднее время (мс)':<25}")
for N, t in results.items():
    print(f"{N:<15}{t:<25.2f}")

# 4. Сохраняем в файл
with open(TIMINGS, "w", encoding="utf-8") as f:
    f.write("N,Time_ms\n")
    for N, t in results.items():
        f.write(f"{N},{t:.2f}\n")

print(f"\nРезультаты сохранены в {TIMINGS}")