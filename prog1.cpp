#include <iostream>
#include <string>

using namespace std;

int main()
{
    string entry1;
    string entry2;
    cout << "dsh >";
    while(getline(cin,entry1))
    {
        if( entry1 == "\n")
        {
            break;  
        }
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
