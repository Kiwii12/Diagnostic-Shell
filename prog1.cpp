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
    cout << "dsh >";

    while(true)
    {
		// Grab cin inside the infinant loop in the off chance the user wants
		// to maybe use a second command
		getline(cin, entry1);
		istringstream myStringStream(entry1);
		while (getline(myStringStream, entry2, ' '))
		{
			entry.push_back(entry1);
		}
		if (!entry.empty())
		{
			entry1 = entry[0];
		}

		// Start of "Switch"
		if (entry.size() > 2)
		{
			cout << " Too many commands " << endl;
		}
        else if( entry1 == "\n")
        { }
        else if( entry1 == "cmdnm" )
        {
            
        }
        else if( entry1 == "systat" )
        {
            
        }
        else if( entry1 == "exit" )
        {
            exit(0);
        }
        else if( entry1 == "cd" )
        {
            
        }
        else if( entry1 == "pwd" )
        {
            
        }
        else if( entry1 == "signal" )
        {
            
        }
        else
        {
            cout << "Invalid Command" << endl;
        }

        cout << "dsh >";
    }
}
