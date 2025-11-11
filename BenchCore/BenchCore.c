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

void get_cpu_vendor(char* outBuf, size_t bufLen) {
	if (!outBuf || bufLen == 0)
		return;
	int info[4] = { 0 };
	__cpuidex(info, 0, 0);
	char vendor[13];
	memcpy(&vendor[0], &info[1], 4); //EBX
	memcpy(&vendor[4], &info[3], 4); //EDX
	memcpy(&vendor[8], &info[2], 4); //ECX
	vendor[12] = '\0';
	strncpy(outBuf, vendor, bufLen - 1);
	outBuf[bufLen - 1] = '\0';
}

void get_cpu_brand(char* outBuf, size_t bufLen) {
	if (!outBuf || bufLen == 0)
		return;
	int info[4];
	char brand[49];
	memset(brand, 0, sizeof(brand));
	__cpuidex(info, 0x80000002, 0);
	memcpy(brand + 0, info, 16);
	__cpuidex(info, 0x80000003, 0);
	memcpy(brand + 16, info, 16);
	__cpuidex(info, 0x80000004, 0);
	memcpy(brand + 32, info, sizeof(info));
	brand[48] = '\0';
	
	size_t i;
	for (i = 48; i > 0; i--)
		if (brand[i - 1] == ' ' || brand[i - 1] == '\0')
			break;
	brand[i] = '\0';
	strncpy(outBuf, brand, bufLen - 1);
	outBuf[bufLen - 1] = '\0';
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