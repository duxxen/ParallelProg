#include "lab4.h"
namespace LR4
{
	std::mutex mutex;

	uint64_t number, segment;
	uint64_t divider = 0;

	void find_divider(uint64_t index)
	{
		uint64_t start = segment * index + 2;
		uint64_t end = start + segment;

		for (uint64_t div = start; div < end && !divider; div++)
			if (number % div == 0)
			{
				mutex.lock();
				divider = div;
				mutex.unlock();
				break;
			}
	}

	int lab4(int argc, char* argv[])
	{
		number = 0;
		size_t size = 4;

		if (argc > 2) {
			std::stringstream sstream;
			sstream << argv[1];
			sstream >> size;
			sstream.clear();
			sstream << argv[2];
			sstream >> number;
		}
		else {
			std::cout << "Enter number: ";
			std::cin >> number;
		}
		std::cout << "Threads count: " << size << std::endl;
		std::cout << "Number: " << number << std::endl;

		segment = (sqrt(number) - 2) / size;

		std::thread* threads = new std::thread[size];

		clock_t timeStart = clock();
		for (int i = 0; i < size; i++)
			threads[i] = std::thread(find_divider, i);
		for (int i = 0; i < size; i++)
			threads[i].join();
		clock_t timeEnd = clock();

		if (divider != 0)
			std::cout << number << " is not prime!\nDivider: " << divider << std::endl;
		else
			std::cout << number << " is prime!\n";
		std::cout << "Time: " << double(timeEnd - timeStart) / CLOCKS_PER_SEC;

		delete[] threads;
		return 0;
	}
}
