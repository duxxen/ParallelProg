#include "lab1.h"

namespace LR1
{
	char msg[BUFSIZE];

	int rank;
	int size;
	MPI_Request reqs[MAXPROC];
	MPI_Status	stats[MAXPROC];

	int simple;
	uint64_t number;
	uint64_t divider;

	uint64_t start;
	uint64_t end;
	uint64_t segment;

	int masterProcedure()
	{
		if (number == 0)
		{
			printf_s("Enter number: ");
			fflush(stdout);
			scanf_s("%llu", &number);
		}
		printf_s("Number: %llu\n", number);
		printf_s("Procs num: %d\n", size);

		start = 2;
		end = sqrt(number);
		segment = (end - start) / (size - 1);

		MPI_Bcast(&number, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(&end, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(&segment, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		double timeStart = MPI_Wtime();

		int notFinished = size - 1;
		while (notFinished)
		{
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stats[MASTER]);
			switch (stats[MASTER].MPI_TAG)
			{
			case TAG_DIV:
				MPI_Recv(&divider, 1, MPI_UINT64_T, stats[MASTER].MPI_SOURCE, stats[MASTER].MPI_TAG, MPI_COMM_WORLD, &stats[MASTER]);
				sprintf_s(msg, "Divider: %llu", divider);
				simple = 0;
				for (int i = 1; i < size; i++)
					if (i != stats[MASTER].MPI_SOURCE)
						MPI_Isend(&simple, 1, MPI_INT, i, TAG_MSG, MPI_COMM_WORLD, &reqs[i]);
				break;

			case TAG_FIN:
				notFinished--;
				MPI_Recv(&msg, BUFSIZE, MPI_CHAR, stats[MASTER].MPI_SOURCE, stats[MASTER].MPI_TAG, MPI_COMM_WORLD, &stats[MASTER]);
				break;

			default:
				MPI_Recv(&msg, BUFSIZE, MPI_CHAR, stats[MASTER].MPI_SOURCE, stats[MASTER].MPI_TAG, MPI_COMM_WORLD, &stats[MASTER]);
				break;
			}

			#ifdef DEBUG
			{
				printf_s("[MASTER]: Message from [%u]: \"%s\"\n", stats[MASTER].MPI_SOURCE, msg);
				fflush(stdout);
			}
			#endif

			if (simple == 0)
				MPI_Waitall(size - 1, reqs, stats);
		}
		double timeEnd = MPI_Wtime();

		if (simple == 0)
		{
			printf_s("\n[MASTER]: %llu is not simple!\n", number);
			printf_s("[MASTER]: Divider is %llu\n", divider);
		}
		else
			printf_s("\n[MASTER]: %llu is simple!\n", number);
		printf_s("Time: %f\n", timeEnd - timeStart);

		return 0;
	}

	int slaveProcedure()
	{
		MPI_Bcast(&number, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(&end, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);
		MPI_Bcast(&segment, 1, MPI_UINT64_T, MASTER, MPI_COMM_WORLD);

		start = 2;
		if (rank > 1)				start = (rank - 1) * segment;
		else if (rank < size - 1)	end = rank * segment;

		#ifdef DEBUG
		{
			sprintf_s(msg, "Range: [%llu, %llu]", start, end);
			MPI_Send(&msg, BUFSIZE, MPI_CHAR, MASTER, TAG_MSG, MPI_COMM_WORLD);
		}
		#endif // DEBUG

		int inmsg = 0;
		uint32_t iterator = 0;
		for (divider = start; divider <= end; divider++)
		{
			if (iterator >= ITERATIONS)
			{
				iterator = 0;
				MPI_Iprobe(MASTER, TAG_MSG, MPI_COMM_WORLD, &inmsg, &stats[rank]);
				if (inmsg)
				{
					MPI_Recv(&simple, 1, MPI_INT, MASTER, TAG_MSG, MPI_COMM_WORLD, &stats[rank]);
					#ifdef DEBUG
					{
						sprintf_s(msg, "Aborting!");
						MPI_Send(&msg, BUFSIZE, MPI_CHAR, MASTER, TAG_MSG, MPI_COMM_WORLD);
					}
					#endif // DEBUG
					break;
				}
			}

			iterator++;
			if (number % divider == 0)
			{
				MPI_Send(&divider, 1, MPI_UINT64_T, MASTER, TAG_DIV, MPI_COMM_WORLD);
				break;
			}
		}
		MPI_Send(&msg, BUFSIZE, MPI_CHAR, MASTER, TAG_FIN, MPI_COMM_WORLD);

		return 0;
	}

	int lab1(int argc, char* argv[])
	{
		simple = 1;
		number = 0;
		if (argc > 1)
			number = strtoull(argv[1], NULL, 10);

		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		if (rank == MASTER)		masterProcedure();
		else					slaveProcedure();

		MPI_Finalize();

		return 0;
	}
}
