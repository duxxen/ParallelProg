#include "lab2.h"

int LR2::lab2(int argc, char* argv[])
{
	uint64_t num, end;
	uint64_t divider = 0;
	int64_t segment;

	size_t threadCount = 4;
	double timeStart, timeEnd;
	double timeProcedure;

	if (argc > 2)
	{
		threadCount = strtoull(argv[1], NULL, 10);
		num = strtoull(argv[2], NULL, 10);
	}
	else
	{
		printf_s("Enter number: ");
		fflush(stdout);
		scanf_s("%llu", &num);
	}
	printf_s("Threads num: %llu\n", threadCount);
	printf_s("Number: %llu\n", num);

	end = sqrt(num);
	segment = (end - 2) / threadCount;

	omp_set_num_threads(threadCount);
	timeStart = omp_get_wtime();

	#pragma omp parallel
	{
		int thread = omp_get_thread_num();
		#pragma omp for
		for (int i = 0; i < threadCount; i++)
		{
			uint64_t start = i * segment + 2;
			uint64_t end = start + segment;
			double timeProcStart = omp_get_wtime();

			for (uint64_t div = start; div < end && !divider; div++)
			{
				if (!(num % div) != divider)
				{
					divider = div;
					#pragma omp flush
				}
			}
		}
	}
	timeEnd = omp_get_wtime();

	if (divider != 0)	printf_s("%llu is not simple!\nDivider: %llu\n", num, divider);
	else				printf_s("%llu is simple!\n", num);

	timeProcedure = timeEnd - timeStart;
	printf_s("Time: %f", timeProcedure);

	return 0;
}
