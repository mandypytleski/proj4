#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include<sys/wait.h>
#include <stdlib.h>     /* exit, EXIT_FAILURE */
using namespace std;

//WORKED WITH Ellanor Splinter (section ---) and Abby Gottfried (section ---)

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

    

}