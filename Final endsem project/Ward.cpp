#include "Ward.h"

Ward::Ward(int id, string name) : wid(id), wname(name) {}

void Ward::addStaff(const Wardstaff& s) {
    staff.push_back(s);
}

void Ward::check_availability() const {
    cout << "\n[Ward] Checking availability of " << wname << "...\n";
    cout << "  Status: Available\n";
}

void Ward::allocate_ward() const {
    cout << "[Ward] " << wname << " allocated successfully.\n";
}

void Ward::deallocate_ward() const {
    cout << "[Ward] " << wname << " deallocated.\n";
}

void Ward::displayStaff() const {
    cout << "\n[Ward: " << wname << "] Assigned Staff:\n";
    for (const auto& s : staff)
        s.displayInfo();
}
