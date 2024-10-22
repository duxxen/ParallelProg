#include "lab1.h"

double lab1(const int* argc, char*** argv)
{
	char msg[BUFSIZE];
	int rank, size;
	uint64_t num, divider,
		start = 2, end, segment;
	int isSimple = 1;

	int inMsg = 0;
	MPI_Status status;
	MPI_Request reqs[MAXPROC];
	MPI_Status stats[MAXPROC];

	double timeStart, timeEnd;
	double timeProcedure;

	MPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == MASTER)
	{
		printf_s("Enter number: ");
		fflush(stdout);
		scanf_s("%llu", &num);
		printf_s("Entered number : % llu\n", num);

		end = sqrt(num) + 1;
		segment = (end - start) / (size - 1);

		MPI_Bcast(&num, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(&end, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(&segment, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		timeStart = MPI_Wtime();

		int finished = size - 1;
		while (finished > 0)
		{
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			switch (status.MPI_TAG)
			{
			case TAG_FIN:
				finished--;
				MPI_Recv(&msg, BUFSIZE, MPI_CHAR, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
				break;

			case TAG_MSG:
				MPI_Recv(&msg, BUFSIZE, MPI_CHAR, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
				break;

			case TAG_DIV:
				MPI_Recv(&divider, 1, MPI_UINT64_T, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
				sprintf_s(msg, "Divider: %llu", divider);
				isSimple = 0;
				for (int i = 1; i < size; i++)
					if (i != status.MPI_SOURCE)
						MPI_Isend(&isSimple, 1, MPI_INT, i, TAG_MSG, MPI_COMM_WORLD, &reqs[i]);
				break;

			default:
				break;
			}
			#ifdef DEBUG
			{
				printf_s("[MASTER]: Message from [%u]: \"%s\"\n", status.MPI_SOURCE, msg);
				fflush(stdout);
			}
			#endif

			if (isSimple == 0)
				MPI_Waitall(size - 1, reqs, stats);

		}
		timeEnd = MPI_Wtime();

		if (isSimple == 0)
		{
			printf_s("\n[MASTER]: %llu is not simple!\n", num);
			printf_s("[MASTER]: Divider is %llu\n", divider);
		}
		else
			printf_s("\n[MASTER]: %llu is simple!\n", num);

		timeProcedure = timeEnd - timeStart;
		printf_s("Time: %f\n", timeProcedure);
	}
	else
	{
		MPI_Bcast(&num, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(&end, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(&segment, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		timeStart = MPI_Wtime();


		if (rank > 1)			start = (rank - 1) * segment;
		if (rank < size - 1)	end = rank * segment;

		#ifdef DEBUG
		{
			sprintf_s(msg, "Range: [%llu, %llu]", start, end);
			MPI_Send(&msg, BUFSIZE, MPI_CHAR, MASTER, TAG_MSG, MPI_COMM_WORLD);
		}
		#endif // DEBUG

		uint32_t iterator = 0;
		double timeProcStart = MPI_Wtime();
		for (divider = start; divider < end && isSimple; divider++)
		{
			if (iterator >= ITERATIONS)
			{
				iterator = 0;
				MPI_Iprobe(MASTER, TAG_MSG, MPI_COMM_WORLD, &inMsg, &status);
				if (inMsg == 1)
				{
					MPI_Recv(&isSimple, 1, MPI_INT, MASTER, TAG_MSG, MPI_COMM_WORLD, &status);
					#ifdef DEBUG
					{
						sprintf_s(msg, "Aborting!");
						MPI_Send(&msg, BUFSIZE, MPI_CHAR, MASTER, TAG_MSG, MPI_COMM_WORLD);
					}
					#endif // DEBUG
					break;
				}
			}

			#ifdef DEBUG
			{
				if (rank == 1)
				{
					double timeProc = MPI_Wtime();
					if (timeProc - timeProcStart > 2.f)
					{
						uint64_t step = divider - start;
						int per = (long double)step / (end - start) * 100;
						sprintf_s(msg, "Process running... [%llu/%llu] (%d%%): %f", step, end - start, per, timeProc - timeStart);
						MPI_Send(&msg, BUFSIZE, MPI_CHAR, MASTER, TAG_MSG, MPI_COMM_WORLD);
						timeProcStart = timeProc;
					}
				}
			}
			#endif // DEBUG

			iterator++;
			if (num % divider == 0)
			{
				isSimple = 0;
				break;
			}
		}

		if (!isSimple && !inMsg)
			MPI_Send(&divider, 1, MPI_UINT64_T, MASTER, TAG_DIV, MPI_COMM_WORLD);

		timeEnd = MPI_Wtime();
		timeProcedure = timeEnd - timeStart;

		#ifdef DEBUG
			sprintf_s(msg, "Finished! { %f }", timeProcedure); 
		#endif // DEBUG

		MPI_Send(&msg, BUFSIZE, MPI_CHAR, MASTER, TAG_FIN, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return timeProcedure;
}