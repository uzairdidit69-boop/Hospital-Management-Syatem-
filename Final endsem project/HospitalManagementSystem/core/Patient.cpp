#include "Patient.h"

Patient::Patient(int id, string name,
                 string cnic, string guardian,
                 string address, string phone,
                 int age, string gender, string blood_group)
    : pid(id), pname(name), cnic(cnic), guardian(guardian),
      address(address), phone(phone), age(age),
      gender(gender), blood_group(blood_group)
{}

void Patient::pdetails() const {
    cout << "\n--- Patient Details ---\n";
    cout << "ID          : " << pid         << "\n";
    cout << "Name        : " << pname       << "\n";
    cout << "CNIC        : " << cnic        << "\n";
    cout << "Guardian    : " << guardian    << "\n";
    cout << "Address     : " << address     << "\n";
    cout << "Phone       : " << phone       << "\n";
    cout << "Age         : " << age         << "\n";
    cout << "Gender      : " << gender      << "\n";
    cout << "Blood Group : " << blood_group << "\n";
}

void Patient::pay_charges(double amount) const {
    cout << "\n[Patient] " << pname << " paid Rs. " << amount << "\n";
}
