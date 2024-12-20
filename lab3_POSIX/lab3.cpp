#include "lab3.h"

namespace LR3
{
	uint64_t number, segment;
	uint64_t divider = 0;

	clock_t globalStart;
	pthread_mutex_t mutex;

	void* find_divider(void* param)
	{
		size_t local = *static_cast<size_t*>(param);

		uint64_t start = segment * local + 2;
		uint64_t end = start + segment;

		for (uint64_t div = start; div < end && !divider; div++)
			if (number % div == 0)
			{
				pthread_mutex_lock(&mutex);
				divider = div;
				pthread_mutex_unlock(&mutex);
				break;
			}
		return nullptr;
	}

	int lab3(int argc, char* argv[])
	{
		number = 0;
		size_t size = 4;
		pthread_t* threads;
		size_t* params;

		if (argc > 2)
		{
			size = strtoull(argv[1], NULL, 10);
			number = strtoull(argv[2], NULL, 10);
		}
		else
		{
			printf_s("Enter number: ");
			fflush(stdout);
			scanf_s("%llu", &number);
		}
		printf_s("Threads num: %llu\n", size);
		printf_s("Number: %llu\n", number);

		threads = new pthread_t[size];
		params = new size_t[size];

		uint64_t start = 2;
		uint64_t end = sqrt(number);
		segment = (end - start) / (size);

		pthread_mutex_init(&mutex, NULL);

		clock_t timeStart = clock();
		globalStart = timeStart;


		for (size_t i = 0; i < size; i++)
		{
			params[i] = i;
			pthread_create(&threads[i], NULL, find_divider, static_cast<void*>(&params[i]));
		}
		for (size_t i = 0; i < size; i++)
			pthread_join(threads[i], NULL);

		clock_t timeEnd = clock();

		pthread_mutex_destroy(&mutex);
		delete[] threads;
		delete[] params;

		if (divider != 0)	printf_s("%llu is not simple!\nDivider: %llu\n", number, divider);
		else				printf_s("%llu is simple!\n", number);
		printf_s("Time: %f\n", double(timeEnd - timeStart) / CLOCKS_PER_SEC);

		return 0;
	}
}