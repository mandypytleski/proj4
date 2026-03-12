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
 
//WORKED WITH Ellanor Splinter (section 003) and Abby Gottfried (section 001) and Mandy Pytleski (section 002)
//Shared file on GitHub

 
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
 
void printHistory(){
    string fileName = "history.txt";
    ifstream inFile(fileName);
    string line;
    while (getline(inFile, line)) {
        cout << line << endl;
    }
    inFile.close();
}
 
//did research beforehand
void pipeRecursive(vector<string> words){
    int pipeIndex = -1;
    //used to find where the pipe is
    for(int i = 0; i < words.size(); i++){
        if(words[i] == "|"){
            pipeIndex = i;
            break;
        }
    }
 
    //base case where it's the last one
    if(pipeIndex == -1) {
        char* cmd[words.size()+1];
        createArray(words, cmd);
 
        execvp(cmd[0], cmd);
        perror("execvp failed");
        exit(0);
    }
 
    // Split before and after the pipe
    vector<string> before(words.begin(), words.begin()+pipeIndex);
    vector<string> after(words.begin()+pipeIndex+1, words.end());
 
    int fd[2];
    pipe(fd);
 
    //creating child
    int pid = fork();
    if(pid == 0){ // CHILD
 
        //output to pipe
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
 
        char* cmd[before.size()+1];
        createArray(before, cmd);
 
        execvp(cmd[0], cmd);
        perror("execvp failed");
        exit(1);
    }
 
    else{ // PARENT
 
        //input from pipe
        dup2(fd[0], 0);
        close(fd[1]);
        close(fd[0]);
 
        wait(0);
 
        // recursive call to do the rest of the pipes
        pipeRecursive(after);
    }
 
}
 
 
void changeOutPut(string fileName){
    int outFd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    int orgOut = dup(1);
    dup2(outFd, 1);
    close(outFd);
    close(orgOut);
}
void changeInput(string fileName){
    int inFd = open(fileName.c_str(), O_RDONLY, 0666);
    int orgIn = dup(0);
    dup2(inFd, 0);
    close(inFd);
    close(orgIn);
}
 
void originalStdOut(int orgOut){
    dup2(orgOut, 1);
    close(orgOut);
}
void originalStdIn(int orgIn){
    dup2(orgIn, 0);
    close(orgIn);
}
 
 
int main(){
 
    string input = "";
    vector<string> words;
   
    while(input != "exit" && input != "EXIT"){
        cout << "$";
        getline(cin, input);
 
        int orgIn = dup(0);
        int orgOut = dup(1);
 
        //print to history
        addToHistory(input);
 
        if(input == "history"){
            printHistory();
            continue;
        }
       
        // splits into array of words
        words = splitString(input);
 
        // skip empty input
        if (words.size() == 0) {
            originalStdIn(orgIn);
            originalStdOut(orgOut);
            continue;
        }
 
        // detect piping and simple handling: if there's a pipe, hand off to pipeRecursive
        bool hasPipe = false;
        for (const auto &w : words) if (w == "|") hasPipe = true;
 
        if (hasPipe) {
            int pid = fork();
            if (pid == 0) {
                pipeRecursive(words);
                // pipeRecursive should not return
                exit(0);
            } else {
                wait(NULL);
                // restore originals and continue to next command
                originalStdIn(orgIn);
                originalStdOut(orgOut);
                continue;
            }
        }
 
        // detect I/O redirection (< or >). We remove the tokens and filename from words
        bool hasIn = false, hasOut = false;
        string inFile = "", outFile = "";
        for (size_t i = 0; i < words.size(); ) {
            if (words[i] == ">" && i + 1 < words.size()) {
                hasOut = true;
                outFile = words[i+1];
                // erase the '>' and filename
                words.erase(words.begin() + i, words.begin() + i + 2);
            } else if (words[i] == "<" && i + 1 < words.size()) {
                hasIn = true;
                inFile = words[i+1];
                words.erase(words.begin() + i, words.begin() + i + 2);
            } else {
                i++;
            }
        }
 
        // create argv array for exec
        int n = words.size();
        char* command[n+1];
        createArray(words, command);
 
        // fork and execute the simple command, with redirection applied in the child
        pid_t pid = fork();
        if (pid == 0) {
            if (hasOut) {
                int outFd = open(outFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (outFd < 0) {
                    perror("open output file");
                    exit(1);
                }
                if (dup2(outFd, STDOUT_FILENO) < 0) {
                    perror("dup2 output");
                    exit(1);
                }
                close(outFd);
            }
            if (hasIn) {
                int inFd = open(inFile.c_str(), O_RDONLY);
                if (inFd < 0) {
                    perror("open input file");
                    exit(1);
                }
                if (dup2(inFd, STDIN_FILENO) < 0) {
                    perror("dup2 input");
                    exit(1);
                }
                close(inFd);
            }
 
            execvp(command[0], command);
            perror("exec failed");
            exit(1);
        } else {
            wait(NULL);
            // restore original std fds in the parent
            originalStdIn(orgIn);
            originalStdOut(orgOut);
        }
    }
}