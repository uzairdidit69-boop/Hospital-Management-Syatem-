#ifndef PATIENT_H
#define PATIENT_H

#include <iostream>
#include <string>
using namespace std;

class Patient {
private:
    int    pid;
    string pname;
public:
    Patient(int id, string name);
    void pdetails() const;
    void pay_charges(double amount) const;
    int    getPid()   const { return pid;   }
    string getPname() const { return pname; }
};

#endif
