#include "Wardstaff.h"

Wardstaff::Wardstaff(int id, string name, string duty)
    : sid(id), sname(name), sduty(duty) {}

void Wardstaff::displayInfo() const {
    cout << "  Staff  : " << sname << " | Duty: " << sduty << " | ID: " << sid << "\n";
}
