#ifndef WARDSTAFF_H
#define WARDSTAFF_H

#include <iostream>
#include <string>
using namespace std;

class Wardstaff {
private:
    int    sid;
    string sname;
    string sduty;
public:
    Wardstaff(int id, string name, string duty);
    string getSname() const { return sname; }
    string getSduty() const { return sduty; }
    void   displayInfo() const;
};

#endif
