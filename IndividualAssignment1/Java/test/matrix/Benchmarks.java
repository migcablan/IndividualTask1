package Java.test.matrix;

import matrix.Matrix;
import java.lang.management.*;
import java.util.Random;

public class Benchmarks {

    private static double[][] generateMatrix(int n, Random random) {
        double[][] m = new double[n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                m[i][j] = random.nextDouble();
        return m;
    }

    public static void main(String[] args) {
        int[] sizes = {128, 256, 512};
        int repetitions = 3;
        Random rand = new Random();
        ThreadMXBean bean = ManagementFactory.getThreadMXBean();

        for (int n : sizes) {
            double[][] A = generateMatrix(n, rand);
            double[][] B = generateMatrix(n, rand);
            double[][] C = new double[n][n];

            double totalTime = 0.0, totalCPU = 0.0, peakMem = 0.0;

            for (int r = 0; r < repetitions; r++) {
                long usedMemBefore = Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory();
                long startTime = System.nanoTime();
                long cpuBefore = bean.getCurrentThreadCpuTime();

                Matrix.multiply(A, B, C, n);

                long cpuAfter = bean.getCurrentThreadCpuTime();
                long endTime = System.nanoTime();
                long usedMemAfter = Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory();

                double elapsed = (endTime - startTime) / 1e9;
                double cpuUsed = (cpuAfter - cpuBefore) / 1e9;
                double memNow = (usedMemAfter - usedMemBefore) / (1024.0 * 1024.0);

                if (memNow > peakMem) peakMem = memNow;
                totalTime += elapsed;
                totalCPU += cpuUsed;

                System.out.printf("Run %d - time=%.4fs mem=%.2fMB cpu=%.4fs%n", r + 1, elapsed, memNow, cpuUsed);
            }

            System.out.printf("Java summary - n=%d avg=%.6fs peak_mem=%.2fMB avg_cpu=%.4fs%n%n",
                    n, totalTime / repetitions, peakMem, totalCPU / repetitions);
        }
    }
}
