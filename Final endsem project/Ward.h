#ifndef WARD_H
#define WARD_H

#include <iostream>
#include <string>
#include <vector>
#include "Wardstaff.h"
using namespace std;

class Ward {
private:
    int    wid;
    string wname;
    vector<Wardstaff> staff;
public:
    Ward(int id, string name);
    int    getWid()   const { return wid;   }
    string getWname() const { return wname; }
    void addStaff(const Wardstaff& s);
    void check_availability() const;
    void allocate_ward() const;
    void deallocate_ward() const;
    void displayStaff() const;
};

#endif
