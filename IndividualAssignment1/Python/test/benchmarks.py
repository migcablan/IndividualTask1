import os, time, random, psutil
from src.matrix import multiply

def create_matrix(n):
    return [[random.random() for _ in range(n)] for _ in range(n)]

def run_benchmark(n, repetitions):
    process = psutil.Process(os.getpid())
    A = create_matrix(n)
    B = create_matrix(n)

    total_time = total_cpu = 0.0
    peak_mem = 0.0

    for r in range(repetitions):
        mem_before = process.memory_info().rss / (1024 ** 2)
        cpu_before = process.cpu_times().user
        start = time.perf_counter()

        multiply(A, B, n)

        end = time.perf_counter()
        cpu_after = process.cpu_times().user
        mem_after = process.memory_info().rss / (1024 ** 2)

        elapsed = end - start
        cpu_used = cpu_after - cpu_before
        mem_used = mem_after - mem_before

        total_time += elapsed
        total_cpu += cpu_used
        peak_mem = max(peak_mem, mem_used)

        print(f"Run {r+1} - time={elapsed:.4f}s mem={mem_used:.2f}MB cpu={cpu_used:.4f}s")

    print(f"Python summary - n={n} avg={total_time/repetitions:.6f}s peak_mem={peak_mem:.2f}MB avg_cpu={total_cpu/repetitions:.4f}s\n")

def main():
    for size in [128, 256, 512]:
        run_benchmark(size, 3)

if __name__ == "__main__":
    main()