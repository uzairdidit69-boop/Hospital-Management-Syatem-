#include "Operation.h"

Operation::Operation(int id, string name, string date, string time)
    : oid(id), oname(name), odate(date), otime(time) {}

void Operation::displayInfo() const {
    cout << "\n--- Operation Info ---\n";
    cout << "ID    : " << oid   << "\n";
    cout << "Name  : " << oname << "\n";
    cout << "Date  : " << odate << "\n";
    cout << "Time  : " << otime << "\n";
}
