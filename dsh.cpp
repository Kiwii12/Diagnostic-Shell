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
   03-9 -2016	attempted to get hb working
   03-10-2016	refactered program to handle multiple commands at the same time
   				got pthreads working for cmdmn and systat
   				refactered program to ignore extra spaces
   03-11-2016	got redirection working finally. stopped tabs from breaking 
   				the program. redirection only works for fork execute commands.
   				Finally got dsh > to appear after exec commands
   @endverbatim
 *
 *****************************************************************************/
#include "structures.h"
#include "builtInShellCommands.cpp"



//defines the system clock for current time
//using std::chrono::system_clock;

//function taken from sample code
int spawn(char* program, char** arg_list);
int spawnRedirect (char* program, char** arg_list, string filename, bool in);

void* cmdnm( void* vars );
void* systat(void* vars);
void* heartbeat( void* vars);

//redirection
bool redirectOverWrite( ofstream &fout, string filename);
bool redirectAppend( ofstream &fout, string filename);

int checkForRedirect(vector<string> entry, bool &in);
int checkForPipe(vector<string> entry, bool &pipe);

void handlePipe(vector<string> entry, int position);




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

	string entry1; // the string used to check what command is done

	//these two, even though named similar to entry 1, are now only used
	//to remove spaces and tabs from entry. entry2 used to be used for
	//secondary arguments
	string entry2;
	string entry3;

	//thread stuff
	pthread_t newThreads[5];
	int i = 0;
	void* status;


	//if it seems weried that some variables are declared in line
	//and that some aren't, the ones declared up here should be
	//prevelant though the whole program, where as any declared in
	//line should be very local and not used again elsewhere.

	//redirection
	//int option;

	//initialization of stream pointer
	//ostream* myout;
	//myout = &cout;

	//*myout << "a test" << endl;


	//initial dsh
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
		while (getline(myStringStream, entry2, ' ') )
		{
			istringstream myStringStream2(entry2);
			while (getline(myStringStream2, entry3, '\t'))
				//if statment removes extra spaces
				if (!entry3.empty() )
				{
					entry.push_back(entry3);
				}
		}


		while (!entry.empty()) // this allows for multiple shell commands
		{//muliple non-shell commands are only allowed with special cases such as
			//redirect and piping, this is because all other commandline entries
			//are assumed to be arguments to that command.

			entry1 = entry.at(0);

			if( entry1.empty())
			{
				//tabs and spaces get into the entry vector as ""
				entry.erase(entry.begin());
			}
		    else if( entry1 == "\n")
		    { 
		    	//this skips newlines
		    	entry.erase(entry.begin()); 
		    }
		    else if( entry1 == "cmdnm" )
		    {
		    	//needs at least one argument
		        if( entry.size() < 2 )
		        {
		            cout << "Usage: cmdnm <pid>" << endl;
		            entry.erase(entry.begin()); 
		        }
		        else
		        {
		        	//when coverted to its own pthread it needed its arg
		        	//passed as a null pointer.
		        	char currentPid[] = "nothing yet";
		        	strcpy (currentPid, entry.at(1).c_str());
		        	int didItWork = pthread_create(&newThreads[i%5], NULL, cmdnm, (void *)currentPid);

		        	if( didItWork )
		        	{
		        		cout << "Unable to create thread " << endl;
		        	}
		        	else
		        	{
		        		i++;//counter only used to recycle threads
		        	}

		        	//two args used
		        	entry.erase(entry.begin()); entry.erase(entry.begin()); 
		        } //cmdnm(entry2)
		    }
		    else if( entry1 == "systat" )
		    {
		    	//even though its a pthread - it still takes no args.
		        if( pthread_create(&newThreads[i%5], NULL, systat, NULL)) 
		        {
		        	cout << "Unable to create thread " << endl;
		        }//systat();
		        else i++; //recycle threads

		        entry.erase(entry.begin()); 
		    }
		    else if( entry1 == "exit" || entry1 == "quit" ) 
		    {
		        exit(0);
		    }
		    else if( entry1 == "cd" )
		    {
		        if( entry.size() < 2 ) //needs at least 1 arg
		        {
		            cout << "Usage: cd" << endl;
		            cout << "       cd <absolut path>" << endl;
		            cout << "       cd <relative path>" << endl;
		        	entry.erase(entry.begin()); 
		        }
		        else
		        {
		            chdir(entry.at(1).c_str());

		            //two args used
		        	entry.erase(entry.begin()); entry.erase(entry.begin()); 
		        }
		    }
		    else if( entry1 == "pwd" )
		    {
		        cwd();
		        entry.erase(entry.begin()); 
		    }
		    else if( entry1 == "signal" )
		    {
		        if ( entry.size() < 3 ) //needs at least two args
		        {
		            cout << "Usage: signal <signal_num> <pid>" << endl;
		            entry.erase(entry.begin()); //only deletes signal
		        }
		        else 
		        {
		        	signal( stoi(entry.at(1)), stoi(entry.at(2) ));

		        	//three args
		        	entry.erase(entry.begin()); entry.erase(entry.begin()); 
		        	entry.erase(entry.begin()); 
		        }
		    }
		    else if(entry1 == "hb")
		    {
		    	if ( entry.size() < 4)
		    	{
		    		cout << "Usage: hb <increment> <end time> <time unit>" << endl;
		    		entry.erase(entry.begin());
		    	}
		    	else 
		    	{
		    		heartbeat(stoi(entry.at(1)), stoi(entry.at(2)), entry.at(3));
		    		//four args
		        	entry.erase(entry.begin()); entry.erase(entry.begin());
		        	entry.erase(entry.begin()); entry.erase(entry.begin());
		    	}
		    }
			else if (entry1 == "help")
			{
		        help();
		        entry.erase(entry.begin());
			}
		    else //Fork Exec
		    {
		    	//in or out
		    	bool in;
		    	int position;
		    	string filename;
		    	position = checkForRedirect(entry, in );

		    	if (position > 0)
		    	{
		    		filename = entry.at(position+1);
		    		entry.erase(entry.begin() + (position+1));
		    		entry.erase(entry.begin() +(position));
		    	}
		    	bool pipe;
		    	int pipePosition;
		    	pipePosition = checkForPipe(entry, pipe);
		    	if(pipe)
		    	{
		    		handlePipe(entry, pipePosition);
		    	}
		    	else
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

			        int *status = 0;
			        if(position > 0)
			        {
			        	pid_t childId = spawnRedirect( nonConstantString, arg_list,filename,in );
			        	waitpid(childId, status, WUNTRACED);
			        }
			        else
			        {
			        	pid_t childId = spawn( nonConstantString, arg_list );
			        	waitpid(childId, status, WUNTRACED);
			        }
			    }

		        entry.clear();
		    }
		    

		    for(int j = 0; j < 5; j++)
		    {
		    	pthread_join(newThreads[j], &status);
		    }

		}
	    cout << "dsh > ";
	}
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
	// int* status = 0;
  pid_t child_pid;

  /* Duplicate this process.  */
  child_pid = fork();

  if(child_pid < 0) //error check
  {
  	cerr << "fork-execute failed - slow down and try again?" << endl;
  	return child_pid;
  }
  else if (child_pid != 0)
  {
    /* This is the parent process.  */
    //sleep(0);
    // wait(status);
    // sleep(0);
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

int spawnRedirect (char* program, char** arg_list, string filename, bool in)
{
  pid_t child_pid;
  int fileDescripter;

  char fileName[] = "nothing yet";
  strcpy (fileName, filename.c_str());

  /* Duplicate this process.  */
  child_pid = fork();
  if(child_pid < 0) //error check
  {
  	cerr << "fork-execute failed - slow down and try again?" << endl;
  	return child_pid;
  }
  else if (child_pid != 0)
  {
    /* This is the parent process.  */
      // wait();
      // sleep(0);
    //cout << "dsh > ";
    return child_pid;
  }
  else 
  {
  	if(in)
	{
		fileDescripter = open(fileName, O_RDONLY);
		dup2(fileDescripter, STDIN_FILENO);
		close(fileDescripter);
	}
	else //out
	{
		fileDescripter = creat(fileName, 0666);
		dup2(fileDescripter, STDOUT_FILENO);
		close(fileDescripter);
	}
    /* Now execute PROGRAM, searching for it in the path.  */
    execvp (program, arg_list);
    /* The execvp function returns only if an error occurs.  */
    cerr << "No Command " << string(program) << " found" << endl;
    abort();
  }
}

void spawnPipe (char* program1, char** arg_list1, char* program2, char** arg_list2)
{
	cout << "entered spawnPipe" << endl;

	// int* status = 0;
  pid_t child_pid;
  pid_t child_pid2;
  int fileDescripter[2]; // file descriptors
  int * status = 0;

  pipe(fileDescripter);

  /* Duplicate this process.  */
  child_pid = fork();

  if(child_pid < 0) //error check
  {
  	cerr << "fork-execute failed - slow down and try again?" << endl;
  	return;
  }
  else if (child_pid != 0)
  {
    /* This is the parent process.  */
    //waitpid(childId, status, WUNTRACED);
    //needs to spawn child # 2
    child_pid2 = fork();
    if(child_pid2 < 0)
    {
    	cerr << "fork-execute failed - slow down and try again?" << endl;
    	return;
    }
    else if (child_pid2 != 0)
    {
    	//still parent process
    	waitpid(child_pid2, status, WUNTRACED);
    }
    else
    {
    	//this is the second child
    	//open read and write
    	dup2(fileDescripter[1],1);

    	//no reading - close it
    	close(fileDescripter[0]);

    	//Execute the first command here
    	execvp(program1, arg_list1);
    	/* The execvp function returns only if an error occurs.  */
	    cerr << "No Command " << string(program1) << " found" << endl;
	    abort();
    }
  }
  else 
  {
  	//open read/write for pipe
  	dup2(fileDescripter[0],0);

  	//close writing
  	close(fileDescripter[1]);

    /* Now execute PROGRAM, searching for it in the path.  */
    execvp (program2, arg_list2);
    /* The execvp function returns only if an error occurs.  */
    cerr << "No Command " << string(program2) << " found" << endl;
    abort();
  }
}

int checkForRedirect(vector<string> entry, bool &in)
{
	vector<string>::iterator findThis;
	findThis = find(entry.begin(), (entry.end() -1), ">");
	in = false;

	//cout << "in check redirect, findthis = " << findThis << endl;

	int position = 0;

	position = distance(entry.begin(), findThis);
	if(findThis != entry.end() -1)
	{
		return position;
	}

	findThis = find(entry.begin(), (entry.end() -1), "<");
	in = true;

	position = 0;

	//cout << "in check redirect, findthis = " << findThis << endl;

	if(findThis != entry.end() -1)
	{
		position = distance(entry.begin(), findThis);
		return position;
	}
	//postion = ">" - entry.begin();
	//cout << "position = " << position << endl;
	return 0;
}

int checkForPipe(vector<string> entry, bool &pipe)
{
	vector<string>::iterator findThis;
	findThis = find(entry.begin(), (entry.end() -1), "|");
	pipe = false;

	//cout << "in check redirect, findthis = " << findThis << endl;

	int position = 0;

	position = distance(entry.begin(), findThis);
	if(findThis != entry.end() -1)
	{
		pipe = true;
		return position;
	}
	return 0;
}

void handlePipe(vector<string> entry, int position)
{
	cout << "handling pipe" << endl;


	char nonConstantString[300];
	char* arg_list1[300];
	char* arg_list2[300];
	for( int i = 0; i < position; i++ )
	{
	    strcpy(nonConstantString, (entry.at(i)).c_str());
	    arg_list1[i] = nonConstantString;
	}
	for( int i = position+1; i < int(entry.size() ); i++ )
	{
	    strcpy(nonConstantString, (entry.at(i)).c_str());
	    arg_list2[i] = nonConstantString;
	}
	arg_list1[entry.size()] = nullptr;
	arg_list2[entry.size()] = nullptr;

	spawnPipe( arg_list1[0], arg_list1, arg_list2[0], arg_list2 );
	return;
}