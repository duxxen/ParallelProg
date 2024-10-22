#include "lab2.h"

double lab2(const int* argc, char*** argv)
{
	uint64_t num, end;
	uint64_t divider = 0;
	int64_t segment;

	size_t threadCount = 7;
	double timeStart, timeEnd;
	double timeProcedure;

	printf_s("Enter number: ");
	scanf_s("%llu", &num);

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
			
			#ifdef DEBUG
				printf_s("[%d]: Range: [%llu, %llu]\n", thread, start, end);
			#endif // DEBUG

			for (uint64_t div = start; div < end && !divider; div++)
			{ 
				if (!(num % div) != divider)
				{
					divider = div;
					#pragma omp flush
				}

				#ifdef DEBUG
				{
					if (thread == 0)
					{
						double timeProc = omp_get_wtime();
						if (timeProc - timeProcStart >= 5.f)
						{
							double progress = double(div - start) / segment * 100.0;
							printf_s("[%d]: Process running... [%llu/%llu] (%.2f%%): %.2f\n", thread, div, end, progress, timeProc - timeStart);
							timeProcStart = timeProc;
						}
					}
				}
				#endif
			}
		}

		#ifdef DEBUG
			printf("[%d]: finished! {%.2f}\n", thread, omp_get_wtime() - timeStart);
		#endif
	}

	if (divider != 0)	printf_s("%llu is not simple!\nDivider: %llu\n", num, divider);
	else				printf_s("%llu is simple!\n", num);

	timeEnd = omp_get_wtime();
	timeProcedure = timeEnd - timeStart;
	printf_s("Time: %.2f", timeProcedure);

	return timeProcedure;
}