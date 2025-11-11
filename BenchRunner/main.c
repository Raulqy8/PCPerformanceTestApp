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

	long long iterations = 20000000LL;
	double t = run_integer_test(iterations);
	printf("Integer Kernel: iterations = %lld, time = %.4f s\n", iterations, t);



}