#include <iostream>
#include <string>
using namespace std;

//used AI to get test pages

int main() {

    string line;
    int count = 0;

    while(getline(cin, line)){
        count++;
    }

    cout << "Total lines: " << count << endl;

    return 0;
}