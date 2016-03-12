//input
#include <iostream>
//string functions
#include <string>
#include <string.h>

//for system function commands
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

//for wait commands
#include <sys/wait.h>

//to open files
#include <fstream>

//for kill command
#include <signal.h>

//for time control
#include <chrono>
#include <thread>

#include <sys/time.h>
#include <ctime>

//For Parsing the bloody entry because C++ doesn't seem to have a built in 
//function for such a task
#include <vector>
#include <sstream>

//pthreads - for multithreading
#include <pthread.h>

//used to find position of something in a vector
#include <algorithm>

//used to redirect
#include <fcntl.h>


//because who wants to see std:: all over the place
using namespace std;

#ifndef __sturctures__H__
#define __sturctures__H__
struct CmdmnArgs
{
	int pid;
	string filename;
	bool overWrite;
};

struct HbArgs
{
	int tinc;
	int tend;
	string tval;
	string filename;
	bool overWrite;
};

#endif