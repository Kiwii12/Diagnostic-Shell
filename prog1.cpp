#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
//#include <unistd.h>
#include <fstream>

//For Parsing the bloody entry because C++ doesn't seem to have a built in 
//function for such a task
#include <vector>
#include <sstream>

using namespace std;

bool cmdnm( string pid );

int spawn(char* program, char** arg_list);

bool systat();

char **d2array (int rows);

int main()
{
	//For parsing the shell arguments
	vector<string> entry;

	string entry1;
	string entry2;


	// Faking being a shell here
    cout << "dsh > ";

    while(true)
    {
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
	//	cout << entry.at(0) << " is entry 0" << endl;
	}

	/*// Start of "Switch"
	if (entry.size() > 3)
	{
		cout << " Too many commands " << endl;
	}*/
        if( entry1 == "\n"){ }
        else if( entry1 == "cmdnm" )
        {
		if( entry.size() != 2 )
		{
			cout << "Usage: cmdnm <pid>" << endl;
		}
		cmdnm(entry2);
        }
        else if( entry1 == "systat" )
        {
		cout << "Usage: systat" << endl;
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
//				chdir(entry2.c_str());
			}
        }
        else if( entry1 == "pwd" )
        {
			//getcwd()
			cout << "Usage: pwd" << endl;
        }
        else if( entry1 == "signal" )
        {
			//kill?
			cout << "Usage: signal <signal_num> <pid>" << endl;
        }
	else if (entry1 == "help")
	{
		cout << "Usage: cmdnm <pid>" << endl;
		cout << "     Will return the command string " <<
		"(name) that started the process for the given id" << endl;
		cout << "Usage: signal <signal_num> <pid>" << endl;
		cout << "     This function will send a signal to a process" << endl;
		cout << "Usage: systat" << endl;
		cout << "     Print out some process information using /proc/* files" << endl
		<< "     print (to stdout) Linux version and system uptime." << endl
		<< "     print memory usage information : memtotal and memfree." << endl
		<< "     print cpu information : vendor id through cache size." << endl;
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
        else
        {
            char* nonConstantString;
            char* arg_list[300];
            for( int i = 0; i < (entry.size() - 1); i++ )
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

bool cmdnm( string pid )
{
// USER PID	%CPU	%MEM	VSZ	RSS	TTY	STAT	Start	Time	COMMAND
	string command;
	ifstream fin;

	fin.open( "/proc/" + pid + "/cmdline" );
	if (fin.fail())
	{
		cerr << "Could not open cmdline file" << endl;
		return false;
	}
	getline( fin, command );
	cout << command << " is in command" << endl;
	cout << pid << " " << command << endl;
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
int spawn (char* program, char** arg_list)
{
  //pid_t child_pid;

  /* Duplicate this process.  */
  //child_pid = fork ();
/*  if (child_pid != 0)
    /* This is the parent process.  */
/*    return child_pid;
  else {
    /* Now execute PROGRAM, searching for it in the path.  */
    //execvp (program, arg_list);
    /* The execvp function returns only if an error occurs.  */
    cerr << "an error occurred in execvp" << endl;
    abort ();
//  }
}


//systat
/*
version: /proc/version
uptime: /proc/uptime
memtotal : /proc/meminfo first line
memfree: /proc/meminfo second line
cpuinfo: /proc/cpuinfo lines 2-9
*/
bool systat()
{
	ifstream fin;
	string data;
	fin.open("/proc/version");
	if (fin.fail())
	{
		cerr << "Could not open version file" << endl;
		return false;
	}
	getline(fin, data);
	cout << "Version: " << data << endl;

	fin.open("/proc/uptime");
	if (fin.fail())
	{
		cerr << "Could not open uptime file" << endl;
		return false;
	}
	getline(fin, data);
	cout << "uptime: " << data << endl;

	fin.open("/proc/meminfo");
	if (fin.fail())
	{
		cerr << "Could not open meminfo file" << endl;
		return false;
	}
	getline(fin, data);
	cout << "meminfo: " << data << endl;

	
}

openProcFile(ifstream fin,)