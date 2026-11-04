#ifndef DOCTOR_H
#define DOCTOR_H

#include <iostream>
#include <string>
#include "Patient.h"
#include "Test.h"
using namespace std;

class Doctor {
private:
    int    did;
    string dname;
public:
    Doctor(int id, string name);
    void ddetails() const;
    void prescribe_medicines(const Patient& p) const;
    void prescribe_tests(const Patient& p) const;
    void check_reports(const Test& t) const;
    int    getDid()   const { return did;   }
    string getDname() const { return dname; }
};

#endif
