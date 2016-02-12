#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <signal.h>

//For Parsing the bloody entry because C++ doesn't seem to have a built in 
//function for such a task
#include <vector>
#include <sstream>

using namespace std;

bool cmdnm( string pid );

int spawn(char* program, char** arg_list);

void systat();

bool displayProcFile(string filename);
bool displayProcFileMeminfo();
bool displayProcFileCPUInfo();

void help();
void cwd();

int main()
{
	//For parsing the shell arguments
	vector<string> entry;

	string entry1;
	string entry2;

    while(true)
    {
    cout << "dsh > ";
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
        //kill?
        if ( entry.size() != 3 )
        {
            cout << "Usage: signal <signal_num> <pid>" << endl;
        }
        else
        {
            if( kill( stoi(entry.at(2)), 0 ) != 0 )
            {
                if(kill( stoi(entry.at(2)), stoi(entry.at(1))) == 0)
                {
                    cout << "Invalid Signal" << endl;
                }
            }
            else
            {
                cout << "Invalid pid number, perhaps reversed? " 
                << endl;
            }
            cout << "entry(2) = " << stoi(entry.at(2)) <<
                " - entry(1) = " << stoi(entry.at(1)) << endl;
        }
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
    }
}

bool cmdnm( string pid )
{
// USER PID	%CPU	%MEM	VSZ	RSS	TTY	STAT	Start	Time	COMMAND
	string command;
	ifstream fin;

	fin.open( "/proc/" + pid + "/comm" );
	if (fin.fail())
	{
		cerr << "Could not open cmdline file" << endl;
		return false;
	}
	while( getline( fin, command ) )
	cout << command << endl;
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
  pid_t child_pid;

  /* Duplicate this process.  */
  child_pid = fork();
  if (child_pid != 0)
  {
    /* This is the parent process.  */
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


//systat
/*
version: /proc/version
uptime: /proc/uptime
memtotal : /proc/meminfo first line
memfree: /proc/meminfo second line
cpuinfo: /proc/cpuinfo lines 2-9
*/
void systat()
{	
	displayProcFile( "version");
	displayProcFile("uptime");
	displayProcFileMeminfo();
	displayProcFileCPUInfo();	
}

bool displayProcFile( string filename) 
{
	ifstream fin;
	string data;
	fin.open("/proc/" + filename);
	if (fin.fail())
	{
		cerr << "Could not open " << filename << " file" << endl;
		return false;
	}
	getline(fin, data);
	cout << filename << ": " << data << endl;
	fin.close();
	return true;
}

bool displayProcFileMeminfo()
{
	ifstream fin;
	string data;
	string filename = "meminfo";
	fin.open("/proc/" + filename);
	if (fin.fail())
	{
		cerr << "Could not open " << filename << " file" << endl;
		return false;
	}
	getline(fin, data);
	cout << "MemTotal: " << data << endl;
	getline(fin, data);
	cout << "MemFree: " << data << endl;
	fin.close();
	return true;
}

bool displayProcFileCPUInfo()
{
	ifstream fin;
	string data;
	string filename = "cpuinfo";
	fin.open("/proc/" + filename);
	if (fin.fail())
	{
		cerr << "Could not open " << filename << " file" << endl;
		return false;
	}
	//skip first line
	getline(fin, data);
	for (int i = 1; i < 9; i++)
	{
		getline(fin, data);
		cout << data << endl;
	}
	fin.close();
	return true;
}

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

void cwd()
{
    char *nonConstantString = get_current_dir_name();
    //strcpy(nonConstantString, (entry1.c_str()));
    //nonConstantString = 
    cout << "Current Directory: " << string(nonConstantString) 
        << endl;
    free( nonConstantString );
}
               
