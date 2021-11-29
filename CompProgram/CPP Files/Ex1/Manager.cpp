#include <string.h>
#include "Manager.h"
#include "Person.h"

using namespace std;

Manager::Manager(const string &name, int regNo):Person(name)
{
    this->name = name;
    this->regNo = regNo;
}

string Manager::getRegNo() const
{  
    return regNo;
}

