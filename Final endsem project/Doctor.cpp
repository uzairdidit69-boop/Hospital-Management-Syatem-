#include "Doctor.h"

Doctor::Doctor(int id, string name) : did(id), dname(name) {}

void Doctor::ddetails() const {
    cout << "\n--- Doctor Details ---\n";
    cout << "ID   : " << did   << "\n";
    cout << "Name : " << dname << "\n";
}

void Doctor::prescribe_medicines(const Patient& p) const {
    cout << "\n[Doctor] " << dname << " prescribed medicines to " << p.getPname() << "\n";
}

void Doctor::prescribe_tests(const Patient& p) const {
    cout << "[Doctor] " << dname << " prescribed tests for " << p.getPname() << "\n";
}

void Doctor::check_reports(const Test& t) const {
    cout << "[Doctor] " << dname << " checked report for test: " << t.getTname() << "\n";
}
