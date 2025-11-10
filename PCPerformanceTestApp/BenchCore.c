
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
	__cpuidex
}