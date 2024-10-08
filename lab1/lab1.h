#pragma once
#include <stdio.h>
#include <mpi.h>

#define MASTER 0
#define MAXPROC 128
#define BUFSIZE 256

#define TAG_MSG 5
#define TAG_FIN 6
#define TAG_DIV 7

// returns timeProcedure values of every procedure (max value is MASTER)
double lab1(const int* argc, char*** argv);