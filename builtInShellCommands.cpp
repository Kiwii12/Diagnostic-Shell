#include "structures.h"

// primary functions for loop
void * cmdnm( void* pid  );
void help(  );
void cwd(  );
void signal(int signaNum, pid_t pid  );
void heartbeat(int tinc, int tend, string tval  );

void* systat( void * doesNothing);
//systat helper functions
bool displayProcFile(string filename  );
bool displayProcFileMeminfo(  );
bool displayProcFileCPUInfo(  );


/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Displays the command that started a program
 *
 * @param[in]   string pid  -   the process id of the program in question
 *
 * @returns True - function was succesful
 * @returns False - function failed to open file
*******************************************************************************/
void *cmdnm( void* pid  )
{
	string command;
	ifstream fin;
	string currentPid((char*) pid);

	//string currentPid((char*) pid);

	//Opens file
	fin.open( "/proc/" + currentPid + "/comm" );
	if (fin.fail())
	{
		cerr << "Could not open cmdline file" << endl;
		pthread_exit(NULL);
	}
	//dumps all file information to screen
	while( getline( fin, command ) )
	{
		cout << "Command: " << command << endl;
	}
	pthread_exit(NULL);
}

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Displays help information about custom shell commands
*******************************************************************************/
void help(  )
{
   cout << "Usage: cmdnm <pid>" << endl;
	cout << "     Will return the command string " <<
    "(name) that started the process for the given id" << endl;
   cout << "Usage: signal <signal_num> <pid>" << endl;
   cout << "     This function will send a signal to a process"
        << endl;
   cout << "Usage: systat" << endl;
   cout << "     Print out some process information using" <<
        " /proc/* files" << endl
    << "     print (to stdout) Linux version and system uptime."
        << endl
    << "     print memory usage information : memtotal and" <<
        " memfree." << endl
    << "     print cpu information : vendor id through" <<
        " cache size." << endl;
   cout << "Usage: cd" << endl;
   cout << "     Displays files in current directory" << endl;
   cout << "       cd <absolut path>" << endl;
   cout << "     Changes to the given directory" << endl;
   cout << "       cd <relative path>" << endl;
   cout << "     Changes to the given directory based on"
    << " the files in the current directory" << endl;
   cout << "Usage: pwd" << endl;
   cout << "     Print out the current working "
    << "directory (the path from /)" << endl;   
}

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Displays the Current Working Directory
*******************************************************************************/
void cwd(  )
{
    //The command get_current_dir_name uses dynamic memory
    char *nonConstantString = get_current_dir_name();
   cout << "Current Directory: " << string(nonConstantString) 
        << endl;
    //Free's dynamic memory
    free( nonConstantString );
}
       
/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Sends a Signal to a process. This function will only work
 * if the user has permisions to use the kill command!!!
 * 
 *
 * @param[in]   string pid  -   the process id of the program in question
 * @param[in]   signalNum   -   The signal to be sent
 *
*******************************************************************************/
void signal(int signalNum, pid_t pid  )
{

    //kill returns 0 on success
    //Tests if pid is valid
    if( kill( pid, 0 ) == 0 )
    {
	//Tests if signalNum is valid
        if(kill( pid, signalNum) != 0)
        {
           cout << "Invalid Signal" << endl;
        }
        else return; //done
    }
    else
    {
       cout << "Invalid pid number, perhaps reversed? " 
        << endl;
    }
}

void heartbeat(int tinc, int tend, string tval  )
{
	int totalIncrement = 0;
	struct timeval *tv;

	//chrono::system_clock::time_point timePoint;
	//auto timePoint;

	while (totalIncrement < tend)
	{
		gettimeofday(tv, NULL);
		cout << "test " << endl;
		//auto timePoint = chrono::system_clock::now();
		//auto timePoint = chrono::system_clock::time_point<chrono::system_clock, chrono::microseconds>;
		//myout << put_time(localtime(chrono::system_clock::now()), "%F %T") << endl;
		//myout << timePoint << endl;

		cout << totalIncrement << " totalIncrement" << endl;
		//increase time
		this_thread::sleep_until(chrono::system_clock::now() + chrono::seconds(tinc));
		totalIncrement += tinc;
	}
	cout << "after while loop";
}

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Displays system information
 * version: /proc/version
 * uptime: /proc/uptime
 * memtotal : /proc/meminfo first line
 * memfree: /proc/meminfo second line
 * cpuinfo: /proc/cpuinfo lines 2-9
 *
*******************************************************************************/
void * systat( void * doesNothing )
{	
	displayProcFile( "version");
	displayProcFile("uptime");
	displayProcFileMeminfo();
	displayProcFileCPUInfo();
	pthread_exit(NULL);
}

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Displays the first line of a given file
 *
 * @param[in]   string filename  -   file being opened
 *
 * @returns True - function was succesful
 * @returns False - function failed to open file
*******************************************************************************/
bool displayProcFile( string filename  ) 
{
	ifstream fin;
	string data;

	//opens file
	fin.open("/proc/" + filename);
	if (fin.fail())
	{
		cerr << "Could not open " << filename << " file" << endl;
		return false;
	}
	//gets entirety of the first line but only first line
	getline(fin, data);
	//dumps data to screen
	cout <<  data << endl;
	fin.close();
	return true;
}

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Displays first 2 lines of meminfo file
 *
 * @returns True - function was succesful
 * @returns False - function failed to open file
*******************************************************************************/
bool displayProcFileMeminfo(  )
{
	ifstream fin;
	string data;
	string filename = "meminfo";
	//opens meminfo in proc folder
	fin.open("/proc/" + filename);
	if (fin.fail())
	{
		cerr << "Could not open " << filename << " file" << endl;
		return false;
	}
	//dumps first 2 lines to screen
	getline(fin, data);
	cout << data << endl;
	getline(fin, data);
	cout << data << endl;
	fin.close();
	return true;
}

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Displays lines 2-8 of File CPUInfo
 *
 * @returns True - function was succesful
 * @returns False - function failed to open file
*******************************************************************************/
bool displayProcFileCPUInfo(  )
{
	ifstream fin;
	string data;
	string filename = "cpuinfo";
	//opens cpuinfo from proc folder
	fin.open("/proc/" + filename);
	if (fin.fail())
	{
		cerr << "Could not open " << filename << " file" << endl;
		return false;
	}
	//skip first line
	getline(fin, data);
	//dumps lines 2-9 to screen
	for (int i = 1; i < 9; i++)
	{
		getline(fin, data);
		cout << data << endl;
	}
	fin.close();
	return true;
}