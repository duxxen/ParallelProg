#pragma once
#include "../common.h"

namespace LR1
{
	#include <mpi.h>
	#define MASTER 0
	#define MAXPROC 128
	#define BUFSIZE 256
	#define ITERATIONS 1000000
	
	#define TAG_MSG 5
	#define TAG_FIN 6
	#define TAG_DIV 7

	int masterProcedure();
	int slaveProcedure();

	int lab1(int argc, char* argv[]);
}