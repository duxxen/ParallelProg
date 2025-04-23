#pragma once
#include "../common.h"

namespace LR3
{
	#define HAVE_STRUCT_TIMESPEC
	#include <pthread.h>
	int lab3(int argc, char* argv[]);
}