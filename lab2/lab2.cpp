#include "lab2.h"

double lab2(const int* argc, char*** argv)
{
	uint64_t num;
	uint64_t divider = 0;

	size_t threadCount = 16;
	double timeStart, timeEnd;
	double timeProcedure;

	printf_s("Enter number: ");
	scanf_s("%llu", &num);

	omp_set_num_threads(threadCount);
	timeStart = omp_get_wtime();

	#pragma omp parallel
	{

	}

	timeEnd = omp_get_wtime();
	timeProcedure = timeEnd - timeStart;
}