#include "Patient.h"

Patient::Patient(int id, string name) : pid(id), pname(name) {}

void Patient::pdetails() const {
    cout << "\n--- Patient Details ---\n";
    cout << "ID   : " << pid   << "\n";
    cout << "Name : " << pname << "\n";
}

void Patient::pay_charges(double amount) const {
    cout << "\n[Patient] " << pname << " paid Rs. " << amount << "\n";
}
