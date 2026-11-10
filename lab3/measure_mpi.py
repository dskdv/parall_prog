import subprocess
import re

SIZES = [200, 400, 800, 1200, 1600, 2000]
PROCS = [1, 2, 4, 8, 16]          # число процессов (ядер)
REPEATS = 3

EXE = "./lab3"
TIMINGS = "timings.csv"

PATTERN = re.compile(r"Время умножения:\s*([\d.]+)\s*мс")


def run_once(n, p):
    result = subprocess.run(
        ["mpirun", "-np", str(p), EXE],
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    if result.returncode != 0:
        print(f"Ошибка при запуске N={n}, P={p}")
        print("STDOUT:", result.stdout)
        print("STDERR:", result.stderr)
        raise SystemExit(1)

    match = PATTERN.search(result.stdout)
    if not match:
        print(f"Не удалось распарсить вывод для N={n}, P={p}:")
        print(result.stdout)
        raise SystemExit(1)

    return float(match.group(1))


results = {}  # (N, P) -> среднее время

for n in SIZES:
    print(f"\nРазмер матрицы: {n}x{n}", flush=True)

    # матрицы генерируются один раз для каждого N
    subprocess.run(["python3", "generate.py", str(n)], check=True)

    for p in PROCS:
        times = []
        for run in range(REPEATS):
            t = run_once(n, p)
            times.append(t)
            print(f"  P={p:<3} запуск {run + 1}: {t:.3f} мс", flush=True)
        avg = sum(times) / len(times)
        results[(n, p)] = avg
        print(f"  P={p:<3} среднее: {avg:.3f} мс", flush=True)

# итоговая таблица
print("\n\nИТОГОВАЯ ТАБЛИЦА (среднее время, мс)")
print(f"{'N':<8}" + "".join(f"{'P=' + str(p):<14}" for p in PROCS))
for n in SIZES:
    print(f"{n:<8}" + "".join(f"{results[(n, p)]:<14.2f}" for p in PROCS))

with open(TIMINGS, "w", encoding="utf-8") as f:
    f.write("N,P,Time_ms\n")
    for (n, p), t in results.items():
        f.write(f"{n},{p},{t:.3f}\n")

print(f"\nРезультаты сохранены в {TIMINGS}")
