#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <string.h>
#include <unistd.h>  //getcwd
#include <sstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

//WORKED WITH Ellanor Splinter (section ---) and Abby Gottfried (section ---)
//Shared file on GitHub

/*What he talked about in class
main(){
vector<string>history = getHistroy("hit.txt");
string line;
while(true){
if(line == "exit")
exit(0);
else if(line=="pwd")
cout << pwd()<<"~$"
getline(cin, line);
history.push_back(line); //updates history file
}
}

vector<string>getHistory(string){
string pwd pwd(){}
}

create function to change standard output
elseif(stdOutRed(line)>-1{}

same with in

cmd > File.name
cmd < File.name

elseif(hasPiping(line)>-1){
cmd1 | cmd2 //see example 10(?)


else(simple command) use execvp
*/

vector<string> splitString(string str){


    vector<string> words;
    string word;
    for (char c : str)
    {
        if (c == ' ')
        {
            if (!word.empty())
            {
                words.push_back(word);
                word.clear();
            }
        }
        else
        {
            word += c;
        }
    }
    if (!word.empty())
    {
        words.push_back(word);
    }
    return words;
}

void createArray(const vector<string>& words, char* command[]) {

    int n = words.size();

    for (int i = 0; i < n; i++) {
        command[i] = (char*) words[i].c_str();
    }

    command[n] = NULL;  
}

void addToHistory(string line){
	string fileName = "history.txt";
	int outFd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
	int orgOut = dup(1);
	dup2(outFd, 1);
	cout<<line<<endl;
	dup2(orgOut, 1);
	close(outFd);
    close(orgOut);
}

int main(){
//creating two pipes
	int fdparent2child[2], fdchild2parent[2];

	if (pipe(fdparent2child) < 0)
	{
		cout << "Cannot create the pipe\n";
		exit(1);
	}
	if (pipe(fdchild2parent) < 0)
	{
		cout << "Cannot create the pipe\n";
		exit(2);
	}

	string input = "";
	vector<string> words;
	
	while(input != "exit" && input != "EXIT"){
		cout << "$";
		getline(cin, input);

		//print to history
		addToHistory(input);
		
		//splits into array of words
		words = splitString(input);
		int n = words.size();

		//initialize c_string array
		char* command[n+1]; 
		createArray(words, command);
	}

    

}