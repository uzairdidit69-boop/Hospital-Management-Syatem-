#include "Reception.h"

Reception::Reception(int id, string name) : rid(id), rname(name) {}

void Reception::register_patient(const Patient& p) const {
    cout << "\n[Reception: " << rname << "] Registered patient: " << p.getPname()
         << " (ID: " << p.getPid() << ")\n";
}

void Reception::receive_payment(const Patient& p, double amount) const {
    cout << "\n[Reception: " << rname << "] Payment of Rs. " << amount
         << " received from " << p.getPname() << "\n";
}

void Reception::patient_status(const Patient& p) const {
    cout << "[Reception] Status check for: " << p.getPname() << " — Admitted\n";
}

void Reception::allocate_ward(const Patient& p, const Ward& w) const {
    cout << "[Reception] " << p.getPname() << " allocated to ward: "
         << w.getWname() << " (Ward ID: " << w.getWid() << ")\n";
}
