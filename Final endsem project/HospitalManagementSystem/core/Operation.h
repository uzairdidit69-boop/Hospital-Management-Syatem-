#ifndef OPERATION_H
#define OPERATION_H

#include <iostream>
#include <string>
using namespace std;

class Operation {
private:
    int    oid;
    string oname;
    string odate;
    string otime;
public:
    Operation(int id, string name, string date, string time);
    void displayInfo() const;
    int    getOid()   const { return oid;   }
    string getOname() const { return oname; }
    string getOdate() const { return odate; }
    string getOtime() const { return otime; }
};

#endif
