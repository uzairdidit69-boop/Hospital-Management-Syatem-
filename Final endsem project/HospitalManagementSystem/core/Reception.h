#ifndef RECEPTION_H
#define RECEPTION_H

#include <iostream>
#include <string>
#include "Patient.h"
#include "Ward.h"
using namespace std;

class Reception {
private:
    int    rid;
    string rname;
public:
    Reception(int id, string name);
    void register_patient(const Patient& p) const;
    void receive_payment(const Patient& p, double amount) const;
    void patient_status(const Patient& p) const;
    void allocate_ward(const Patient& p, const Ward& w) const;
};

#endif
