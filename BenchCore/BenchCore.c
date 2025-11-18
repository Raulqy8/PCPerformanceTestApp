#define _CRT_SECURE_NO_WARNINGS
#include "BenchCore.h"
#include <Windows.h>
#include <intrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static double get_time_seconds() {
	LARGE_INTEGER freq, t;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&t);
	return (double)t.QuadPart / (double)freq.QuadPart;
}

void get_system_info() {
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf("Processor Architecture: %u\n", sysInfo.wProcessorArchitecture);
	printf("Number of Processors: %lu\n", sysInfo.dwNumberOfProcessors);
	printf("Page Size: %lu bytes\n", sysInfo.dwPageSize);
	printf("Min App Address: %p\n", sysInfo.lpMinimumApplicationAddress);
	printf("Max App Address: %p\n", sysInfo.lpMaximumApplicationAddress);
	printf("Allocation Granularity: %lu bytes\n", sysInfo.dwAllocationGranularity);
	printf("Processor Type: %lu\n", sysInfo.dwProcessorType);
}

void get_cpu_vendor(char* outBuf, size_t bufLen) {
	if (!outBuf || bufLen == 0)
		return;
	int cpu[4];
	__cpuid(cpu, 0);
	memcpy(outBuf + 0, &cpu[1], 4);
	memcpy(outBuf + 4, &cpu[3], 4);
	memcpy(outBuf + 8, &cpu[2], 4);
	outBuf[12] = 0;
}

void get_cpu_brand(char* outBuf, size_t bufLen) {
	if (!outBuf || bufLen == 0)
		return;
	int cpu[4];
	char brand[49] = { 0 };
	__cpuidex(cpu, 0x80000002, 0); //First 16 bytes
	memcpy(brand + 0, cpu, 16);
	__cpuidex(cpu, 0x80000003, 0); //Next 16 bytes
	memcpy(brand + 16, cpu, 16);
	__cpuidex(cpu, 0x80000004, 0); //Final 16 bytes
	memcpy(brand + 32, cpu, 16);
	
	char* end = brand + 48;
	while (end > brand && (end[-1] == ' ' || end[-1] == 0))
		end--;
	*end = 0;
	strncpy(outBuf, brand, bufLen - 1);
	outBuf[bufLen - 1] = 0;
}

void get_cache_info() {
	int cpu[4];
	for (int ecx = 0; ; ecx++) {
		__cpuidex(cpu, 4, ecx);
		int type = cpu[0] & 0x1F;
		if (type == 0)
			break;
		int level = (cpu[0] >> 5) & 0x7;
		int ways = ((cpu[1] >> 22) & 0x3FF) + 1;
		int parts = ((cpu[1] >> 12) & 0x3FF) + 1;
		int lineSize = (cpu[1] & 0xFFF) + 1;
		int sets = cpu[2] + 1;
		long long size = (long long)ways * parts * lineSize * sets / 1024;
		const char* cache_type;
		if (type == 1)
			cache_type = "Data";
		else if (type == 2)
			cache_type = "Instruction";
		else if (type == 3)
			cache_type = "Unified";
		else cache_type = "Reserved";

		printf("L%d %s Cache: %lld KB\n", level, cache_type, size);
	}
}

double get_cpu_freq() {
	int cpu[4] = { 0 };
	__cpuid(cpu, 0);
	if (cpu[0] >= 0x16) {
		__cpuidex(cpu, 0x16, 0);
		if (cpu[0] != 0)
			return (double)cpu[0];
	}

	LARGE_INTEGER freq, t0, t1;
	unsigned long long c1, c2;
	double s;
	
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&t0);
	c1 = __rdtsc();

	Sleep(100);

	QueryPerformanceCounter(&t1);
	c2 = __rdtsc();

	s = (double)(t1.QuadPart - t0.QuadPart) / freq.QuadPart;
	double mhz = (double)(c2 - c1) / s / 1e6;

	return mhz;

}

double run_integer_test(long long it) {
	if (it <= 0)
		return 0.0;
	volatile long long x = 1;
	double t0 = get_time_seconds();
	for (long long i = 1; i <= it; i++) {
		long long m = (long long)(i % 13);
		x = (x + i) * (m + 1);
		x = x ^ (i & 255LL);
	}
	double t1 = get_time_seconds();

	return t1 - t0;
}

double run_mandelbrot_test(int width, int height, int maxIt) {
	if (width <= 0 || height <= 0 || maxIt <= 0)
		return 0.0;
	double t0 = get_time_seconds();
	double xmin = -2.0, xmax = 1.0;
	double ymin = -1.2, ymax = 1.2;
	for (int py = 0; py < height; py++) {
		for (int px = 0; px < width; px++) {
			double x0 = xmin + (xmax - xmin) * px / (double)width;
			double y0 = ymin + (ymax - ymin) * py / (double)height;
			double xr = 0.0, yr = 0.0;
			int i = 0;
			while (xr * xr + yr * yr <= 4.0 && i < maxIt) {
				double xt = xr * xr - yr * yr + x0;
				yr = 2.0 * xr * yr + y0;
				xr = xt;
				i++;
			}
			(void)i;
		}
	}
	double t1 = get_time_seconds();
	return t1 - t0;
}

double run_memory_copy_test(size_t bytes) {
	if (bytes == 0)
		return 0.0;
	char* src = (char*)malloc(bytes);
	char* dst = (char*)malloc(bytes);
	if (!src || !dst) {
		free(src);
		free(dst);
		return 0.0;
	}
	memset(src, 0xA5, bytes);
	memcpy(dst, src, bytes);

	double t0 = get_time_seconds();
	memcpy(dst, src, bytes);
	double t1 = get_time_seconds();

	double seconds = t1 - t0;
	double mb = (double)bytes / (1024.0 * 1024.0);
	free(src);
	free(dst);
	if (seconds <= 0.0)
		return 0.0;
	return mb / seconds;
}