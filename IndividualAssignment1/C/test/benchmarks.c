#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <psapi.h>
#include "matrix.c"

static double **createMatrix(int n) {
    double **m = malloc(n * sizeof(double *));
    for (int i = 0; i < n; ++i)
        m[i] = malloc(n * sizeof(double));
    return m;
}

static void releaseMatrix(double **m, int n) {
    for (int i = 0; i < n; ++i)
        free(m[i]);
    free(m);
}

static void fillRandom(double **m, int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            m[i][j] = (double)rand() / RAND_MAX;
}

static double currentMemoryMB() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
    return pmc.WorkingSetSize / (1024.0 * 1024.0);
}

static double currentCPUTime() {
    FILETIME c, e, k, u;
    GetProcessTimes(GetCurrentProcess(), &c, &e, &k, &u);
    ULARGE_INTEGER t;
    t.LowPart = u.dwLowDateTime;
    t.HighPart = u.dwHighDateTime;
    return t.QuadPart / 1e7;
}

static double benchmark(int n, int runs) {
    double **A = createMatrix(n);
    double **B = createMatrix(n);
    double **C = createMatrix(n);
    fillRandom(A, n);
    fillRandom(B, n);

    LARGE_INTEGER freq, start, stop;
    QueryPerformanceFrequency(&freq);

    double totalTime = 0.0, maxMem = 0.0;
    for (int r = 0; r < runs; ++r) {
        double cpuStart = currentCPUTime();
        QueryPerformanceCounter(&start);
        multiply(A, B, C, n);
        QueryPerformanceCounter(&stop);
        double cpuEnd = currentCPUTime();

        double elapsed = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
        double memNow = currentMemoryMB();
        if (memNow > maxMem) maxMem = memNow;

        totalTime += elapsed;
        printf("Run %d - time=%.4fs mem=%.2fMB cpu=%.4fs\n", r + 1, elapsed, memNow, cpuEnd - cpuStart);
    }

    printf("C summary - n=%d avg=%.6fs peak_mem=%.2fMB\n\n", n, totalTime / runs, maxMem);
    releaseMatrix(A, n);
    releaseMatrix(B, n);
    releaseMatrix(C, n);
    return totalTime / runs;
}

int main() {
    srand((unsigned)time(NULL));
    int sizes[] = {128, 256, 512};
    for (int i = 0; i < 3; ++i)
        benchmark(sizes[i], 3);
    return 0;
}
