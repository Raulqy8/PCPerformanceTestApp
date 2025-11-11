#pragma once
#ifndef BENCH_CORE_H
#define BENCH_CORE_H

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

	void get_cpu_vendor(char* outBuf, size_t bufLen);
	void get_cpu_brand(char* outBuf, size_t bufLen);

	double run_integer_test(long long it);
	double run_mandelbrot_test(int width, int height, int maxIter);
	double run_memory_copy_test(size_t bytes);
	double run_file_io_test(size_t fileSize, size_t blockSize);


#endif
