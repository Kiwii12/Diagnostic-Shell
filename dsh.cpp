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
 * @todo_bugs_modification_section
 * @Todo -	Remote Pipes
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
   03-12-2016	descoveried overwrite error			
   				after overwrite error, pipes worked, then hit segfault error
   				with threads, fixed that. documentation refatoring next.
   				hb function finished as well - submitted
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
	void* ptstatus;
	bool isThread = false;


	//initial dsh
	cout << "dsh > ";

    //start of "switch" statement
    while(true)
    {
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
		        	int didItWork = pthread_create(&newThreads[ 0], NULL, cmdnm, (void *)currentPid);

		        	if( didItWork )
		        	{
		        		cout << "Unable to create thread " << endl;
		        	}
		        	else
		        	{
		        		isThread=true;//counter only used to recycle threads
		        	}

		        	//two args used
		        	entry.erase(entry.begin()); entry.erase(entry.begin()); 
		        } //cmdnm(entry2)
		    }
		    else if( entry1 == "systat" )
		    {
		    	//even though its a pthread - it still takes no args.
		        if( pthread_create(&newThreads[ 0], NULL, systat, NULL)) 
		        {
		        	cout << "Unable to create thread " << endl;
		        }//systat();
		        else isThread=true; //recycle threads

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
		    	//if less than 3 args given
		    	if ( entry.size() < 4)
		    	{
		    		cout << "Usage: hb <increment> <end time> <time unit>" << endl;
		    		entry.erase(entry.begin()); // just erase hb
		    	}
		    	else 
		    	{
		    		//fill structure to pass args
		    		HbArgs * hbargs;
		    		hbargs = (HbArgs*)malloc(sizeof(struct HbArgs));
		    		hbargs->tinc = stoi(entry.at(1));
		    		hbargs->tend = stoi(entry.at(2));
		    		hbargs->tval = entry.at(3);
		    		int didItWork = pthread_create(&newThreads[ 0], NULL, heartbeatThread, (void *)hbargs);

		        	if( didItWork )
		        	{
		        		cout << "Unable to create thread " << endl;
		        	}
		        	else
		        	{
		        		isThread=true;//counter only used to recycle threads
		        	}
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
		    	int position; //position of redirect
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
			        char* arg_list[300];
			        int count = 0; // keep i for deletion
			        while(!entry.empty())
			        {
			        	arg_list[count] = (char*)malloc(strlen(entry.at(0).c_str()));
			            strcpy(arg_list[count], (entry.at(0)).c_str());
			            count++;
			            entry.erase(entry.begin());
			        }
			        int *status = 0;
			        if(position > 0)
			        {
			        	pid_t childId = spawnRedirect( arg_list[0], arg_list,filename,in );
			        	waitpid(childId, status, WUNTRACED);
			        }
			        else
			        {
			        	pid_t childId = spawn( arg_list[0], arg_list );
			        	waitpid(childId, status, WUNTRACED); //fixed dsh error
			        }
			        for( int k = 0; k < count; k++)
					{
						delete[] arg_list[k];
					}
			    }
		        entry.clear();
		    }
		    // for(int j = 0; j < 5; j++)
		    // {
		    if(isThread)
		    	pthread_join(newThreads[0], &ptstatus);
		    // }

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
 * @author Johnny Ackerman	- based off of spawn code 
 * 
 * @par Description: redirects command input or output from or to a file
 *
 * @param[in]   char* program 	-	the command in question
 * @param[in]	char** arg_list - 	the arguments for the command
 * @param[in]	string filename -	the file being read in from or output to
 * @param[in]	bool in 		-	true if into a file, false if from a file
 *
 * @returns child_pid 			-	id of the generated thread
*******************************************************************************/
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


/***************************************************************************//**
 * @author Johnny Ackerman	- based off of spawn code 
 * 
 * @par Description: takes the output of the first command and gives it as 
 *		input to the second command
 *
 * @param[in]   char* program1 	-	the first command in question
 * @param[in]	char** arg_list1 - 	the first set of arguments for the command
 * @param[in]   char* program2 	-	the second command in question
 * @param[in]	char** arg_list2 - 	the second set of arguments for the command
*******************************************************************************/
void spawnPipe (char* program1, char** arg_list1, char* program2, char** arg_list2)
{
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

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: takes the output of the first command and gives it as 
 *		input to the second command
 *
 * @param[in]   vector<string>	entry 	-	vector holding user inputed commands
 *											and args
 * @param[in,out] bool in 	- 	determines direction of redirect
 *
 * return 0	- no redirect found
 * return position - location of redirect in entry
*******************************************************************************/
int checkForRedirect(vector<string> entry, bool &in)
{
	vector<string>::iterator findThis;
	//places itterator at found position or end of entry
	findThis = find(entry.begin(), (entry.end() -1), ">");
	in = false;

	//cout << "in check redirect, findthis = " << findThis << endl;

	int position = 0;

	//set position to redirection if found
	position = distance(entry.begin(), findThis);
	if(findThis != entry.end() -1)
	{
		return position;
	}

	//places itterator at found position or end of entry
	findThis = find(entry.begin(), (entry.end() -1), "<");
	in = true;

	position = 0;

	//set position to redirection if found
	if(findThis != entry.end() -1)
	{
		position = distance(entry.begin(), findThis);
		return position;
	}
	return 0;
}

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: looks for pipe in entry
 *
 * @param[in]   vector<string>	entry 	-	vector holding user inputed commands
 *											and args
 * @param[in,out] bool pipe 	- 	true if pipe, otherwise false
 *
 * return 0	- no redirect found
 * return position - location of redirect in entry
*******************************************************************************/
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

/***************************************************************************//**
 * @author Johnny Ackerman
 * 
 * @par Description: sets up the arguments for spawnPipe
 *
 * @param[in]   vector<string>	entry 	-	vector holding user inputed commands
 *											and args
 * @param[in]	int position 	- 	position of pipe in entry
*******************************************************************************/
void handlePipe(vector<string> entry, int position)
{
	char* arg_list1[300];
	int i = 0; // keep i for memory deletion later
	//set first arg array
	for( i = 0; i < position; i++ )
	{
		arg_list1[i] = (char*)malloc(strlen(entry.at(i).c_str()));
	    strcpy(arg_list1[i], entry.at(i).c_str());
	}

	int j = 0; //also used for deletion
	char* arg_list2[301];
	//set second arg array
	for( int k = position + 1; k < int(entry.size() ); k++ )
	{
		arg_list2[j] = (char*)malloc(strlen(entry.at(k).c_str()));
	    strcpy(arg_list2[j], (entry.at(k)).c_str());
	    j++;
	}

	//run commands
	spawnPipe( arg_list1[0], arg_list1, arg_list2[0], arg_list2 );

	//clear memory
	for( int k = 0; k < i; k++)
	{
		delete[] arg_list1[k];
	}
	for( int k = 0; k < j; k++)
	{
		delete[] arg_list2[k];
	}
	return;
}