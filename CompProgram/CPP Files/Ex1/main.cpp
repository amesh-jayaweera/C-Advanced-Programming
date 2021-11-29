#include <iostream>
#include <fstream>
#include <string>
#include <cstring>  
#include <sstream>
#include "Manager.h"

using namespace std;

int main ()
{
    cout << "Welcome!\n";

    ifstream file("managers.txt");
    string line; 
    while (std::getline(file, line))
    {
        cout << line << endl;
        string arr[3];
        int i = 0;
        stringstream ssin(line);
        while (ssin.good() && i < 3){
            ssin >> arr[i];
            ++i;
        }
        Manager manager = Manager(arr[1] + " " + arr[0], stoi(arr[0]));
        cout << manager.getName() << endl;
        cout << manager.getRegNo() << endl;
    }
}