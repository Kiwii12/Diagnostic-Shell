/***************************************************************************//**
 * @file File holds functions to run a self systaning shell
 *
 * @brief Contains dsh shell and helper functions
*******************************************************************************/

/*************************************************************************//**
 * @file 
 *
 * @mainpage Program 1 - Dsh
 * 
 * @section course_section Course Information 
 *
 * @author Johnny Ackerman
 * 
 * @date Feb 12, 2016
 * 
 * @par Professor: 
 *         Dr. McGough
 * 
 * @par Course: 
 *         CSC-456 - M001-2016SP -  2:00-2:50 PM MWTF
 * 
 * @par Location: 
 *         McLaury-306
 *
 * @section program_section Program Information 
 * 
 * @details Process identification program and simple shell
 *
 * Introduction to unix environment, system calls, signals, the proc filesystem
 * and the fork / exec system calls.
 *
 * Program is a command line executible string starting with dsh>. It will
 * accept commands to be executed at the prompt.
 *
 * @section compile_section Compiling and Usage
 *
 * @par Compiling Instructions: 
 *      Build and run
 * 
 * @par Build:
   @verbatim
   > g++ -o dsh dsh.cpp -Wall -g -std=c++11
   @endverbatim
 * 
 * @par Make:
   @vertbatim
   > make
   @endverbatim
 * 
 * @par Usage: 
   @verbatim  
   > ./dsh
   @endverbatim 
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 * 
 * @bugs	calling any non-custom command will cause a second dsh> to appear
 *      on the first line but, none to appear on the next. Thus leaving
 *      cursor without a prompt. (still functional)
 *
 * @bugs    signal doesn't seem to be working - seems to not have permission in
 *  ubuntu - will test in fedora.
 *  Signal will work if the user has permision to use the kill command
 * 
 * @par Modifications and Development Timeline: 
   @verbatim 
   Date          Modification 
   ------------  -------------------------------------------------------------- 
   09-03-2016	read program document - fought with c++ compilers
   02-05-2016   attempt loop structure - failed horribly
   02-09-2016   loop works with if/else instead of switch
   02-10-2016   wasted day attempting to find usable system calls
   02-11-2016   learned needed system calls and proc folders - program grew
                quickly. Also used sample code for fork-exec
   02-12-2016   Wrote up all documentation - did many final test and found 
                many bugs - also going to test in fedora
   @endverbatim
 *
 *****************************************************************************/



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

//to open files
#include <fstream>

//for kill command
#include <signal.h>

//For Parsing the bloody entry because C++ doesn't seem to have a built in 
//function for such a task
#include <vector>
#include <sstream>

using namespace std;

//function taken from sample code
int spawn(char* program, char** arg_list);

// primary functions for loop
bool cmdnm( string pid );
void help();
void cwd();
void signal(int signaNum, pid_t pid);

void systat();
//systat helper functions
bool displayProcFile(string filename);
bool displayProcFileMeminfo();
bool displayProcFileCPUInfo();




/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Runs an infinant loop for a shell process
 *
*******************************************************************************/
int main()
{
	//For parsing the shell arguments
	vector<string> entry;

	string entry1;
	string entry2;

	cout << "dsh > ";

    //start of "switch" statement
    while(true)
    {
    //cout << "dsh > ";
	// Grab cin inside the infinant loop in the off chance the user wants
	// to maybe use a second command
	getline(cin, entry1);

	// Uses the istringstream to tokinize
	istringstream myStringStream(entry1);
	while (getline(myStringStream, entry2, ' '))
	{
		entry.push_back(entry2);
	}
	if (!entry.empty())
	{
		entry1 = entry.at(0);
	}

    if( entry1 == "\n"){ }
    else if( entry1 == "cmdnm" )
    {
        if( entry.size() != 2 )
        {
            cout << "Usage: cmdnm <pid>" << endl;
        }
        else cmdnm(entry2);
    }
    else if( entry1 == "systat" )
    {
        if( entry.size() > 1)
        {
            cout << "Usage: systat" << endl;
        }
        else systat();
    }
    else if( entry1 == "exit" )
    {
        exit(0);
    }
    else if( entry1 == "cd" )
    {
        if( entry.size() != 2 ) 
        {
            cout << "Usage: cd" << endl;
            cout << "       cd <absolut path>" << endl;
            cout << "       cd <relative path>" << endl;
        }
        else
        {
            chdir(entry2.c_str());
        }
    }
    else if( entry1 == "pwd" )
    {
        if( entry.size() > 1 )
        {
        cout << "Usage: pwd" << endl;
        }
        else cwd();
    }
    else if( entry1 == "signal" )
    {
        if ( entry.size() != 3 )
        {
            cout << "Usage: signal <signal_num> <pid>" << endl;
        }
        else signal( stoi(entry.at(1)), stoi(entry.at(2) ));
    }
	else if (entry1 == "helpThis")
	{
        help();
	}
    else //Fork Exec
    {
        char nonConstantString[300];
        char* arg_list[300];
        for( int i = 0; i < int(entry.size() ); i++ )
        {
            strcpy(nonConstantString, (entry.at(i)).c_str());
            arg_list[i] = nonConstantString;
        }
        arg_list[entry.size()] = nullptr;
        //cout << "Invalid Command" << endl;
        strcpy(nonConstantString, (entry1.c_str()));
        spawn( nonConstantString, arg_list );
    }
    entry.clear();
    cout << "dsh > ";
    }
}

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
bool cmdnm( string pid )
{
	string command;
	ifstream fin;

	//Opens file
	fin.open( "/proc/" + pid + "/comm" );
	if (fin.fail())
	{
		cerr << "Could not open cmdline file" << endl;
		return false;
	}
	//dumps all file information to screen
	while( getline( fin, command ) )
	{
		cout << "Command: " << command << endl;
	}
	return true;
}


/***********************************************************************
* Code listing from "Advanced Linux Programming," by CodeSourcery LLC  *
* Copyright (C) 2001 by New Riders Publishing                          *
* See COPYRIGHT for license information.                               *
***********************************************************************/
/* Spawn a child process running a new program.  PROGRAM is the name
   of the program to run; the path will be searched for this program.
   ARG_LIST is a NULL-terminated list of character strings to be
   passed as the program's argument list.  Returns the process id of
   the spawned process.  */
/* - Note spawn was taken from under the Gnu Public License.  Thus this 
code is now under the GNU Public License as well.  You can find out more information about Gnu codes and the
GPL at   http://www.gnu.org/ . */
int spawn (char* program, char** arg_list)
{
  pid_t child_pid;

  /* Duplicate this process.  */
  child_pid = fork();
  if (child_pid != 0)
  {
    /* This is the parent process.  */
      wait();
    //cout << "dsh > ";
    return child_pid;
  }
  else 
  {
    /* Now execute PROGRAM, searching for it in the path.  */
    execvp (program, arg_list);
    /* The execvp function returns only if an error occurs.  */
    cerr << "No Command " << string(program) << " found" << endl;
    abort();
  }
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
void systat()
{	
	displayProcFile( "version");
	displayProcFile("uptime");
	displayProcFileMeminfo();
	displayProcFileCPUInfo();	
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
bool displayProcFile( string filename) 
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
bool displayProcFileMeminfo()
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
	cout << "MemTotal: " << data << endl;
	getline(fin, data);
	cout << "MemFree: " << data << endl;
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
bool displayProcFileCPUInfo()
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

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: Displays help information about custom shell commands
*******************************************************************************/
void help()
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
void cwd()
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
void signal(int signalNum, pid_t pid)
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
