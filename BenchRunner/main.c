#include <stdio.h>
#include <stdlib.h>
#include "BenchCore.h"

int main() {
	char vendor[13];
	char brand[64];

	get_cpu_vendor(vendor, sizeof(vendor));
	printf("CPU vendor: %s\n", vendor);

	get_cpu_brand(brand, sizeof(brand));
	printf("CPU brand: %s\n", brand);

	get_cache_info();
	printf("CPU frequency: %.01f MHz\n", get_cpu_freq());
	get_system_info();

	long long iterations = 20000000LL;
	double t_int = run_integer_test(iterations);
	printf("Integer Kernel: iterations = %lld, time = %.4f s\n", iterations, t_int);

	int w = 800, h = 600, maxIt = 200;
	double t_fp = run_mandelbrot_test(w, h, maxIt);
	printf("Mandelbrot (%dx%d, maxIt=%d) time = %.4f s\n", w, h, maxIt, t_fp);

	size_t bytes = 200 * 1024 * 1024;
	double t_mem = run_memory_copy_test(bytes);
	printf("Memory copy: size=%.1f MB, throughput=%.2f MB/s\n", (double)bytes / (1024.0 * 1024.0), t_mem);

}