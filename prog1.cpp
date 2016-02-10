#include <iostream>
#include <string>

//For Parsing the bloody entry because C++ doesn't seem to have a built in 
//function for such a task
#include <vector>
#include <sstream>

using namespace std;

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
			cout << entry.at(0) << " is entry 0" << endl;
		}

		// Start of "Switch"
		if (entry.size() > 2)
		{
			cout << " Too many commands " << endl;
		}
        else if( entry1 == "\n"){ }
        else if( entry1 == "cmdnm" )
        {
			cout << "Usage: cmdnm <pid>" << endl;
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
			cout << "Usage: cd" << endl;
			cout << "       cd <absolut path>" << endl;
			cout << "       cd <relative path>" << endl;
        }
        else if( entry1 == "pwd" )
        {
			cout << "Usage: pwd" << endl;
        }
        else if( entry1 == "signal" )
        {
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
            cout << "Invalid Command" << endl;
        }

		entry.clear();
        cout << "dsh > ";
    }
}
