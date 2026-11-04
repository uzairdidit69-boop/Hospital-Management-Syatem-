#include "hospital.h"

int main() {
    cout << "========================================\n";
    cout << "   Hospital Management System - C++\n";
    cout << "========================================\n";

    // ── Patient Input ─────────────────────────
    int    pid;    string pname;
    cout << "\nEnter Patient ID   : "; cin >> pid;
    cin.ignore();
    cout << "Enter Patient Name : "; getline(cin, pname);
    Patient p1(pid, pname);
    p1.pdetails();

    // ── Doctor Input ──────────────────────────
    int    did;    string dname;
    cout << "\nEnter Doctor ID   : "; cin >> did;
    cin.ignore();
    cout << "Enter Doctor Name : "; getline(cin, dname);
    Doctor d1(did, dname);
    d1.ddetails();

    // ── Doctor actions ────────────────────────
    d1.prescribe_medicines(p1);

    // ── Lab Test Input ────────────────────────
    int    tid;    string tname;
    cout << "\nEnter Test ID   : "; cin >> tid;
    cin.ignore();
    cout << "Enter Test Name : "; getline(cin, tname);
    Test t1(tid, tname);
    t1.collect_sample();
    d1.prescribe_tests(p1);
    d1.check_reports(t1);
    t1.report_generation();

    // ── Reception Input ───────────────────────
    int    rid;    string rname;
    cout << "\nEnter Reception ID   : "; cin >> rid;
    cin.ignore();
    cout << "Enter Reception Name : "; getline(cin, rname);
    Reception rec(rid, rname);
    rec.register_patient(p1);

    // ── Ward Input ────────────────────────────
    int    wid;    string wname;
    cout << "\nEnter Ward ID   : "; cin >> wid;
    cin.ignore();
    cout << "Enter Ward Name : "; getline(cin, wname);
    Ward w1(wid, wname);
    w1.check_availability();

    // ── Ward Staff Input ──────────────────────
    int    sid;    string sname, sduty;
    cout << "\nEnter Staff ID   : "; cin >> sid;
    cin.ignore();
    cout << "Enter Staff Name : "; getline(cin, sname);
    cout << "Enter Duty/Shift : "; getline(cin, sduty);
    Wardstaff ws1(sid, sname, sduty);
    w1.addStaff(ws1);
    w1.displayStaff();
    rec.allocate_ward(p1, w1);

    // ── Operation Input ───────────────────────
    int    oid;    string oname, odate, otime;
    cout << "\nEnter Operation ID              : "; cin >> oid;
    cin.ignore();
    cout << "Enter Operation Name            : "; getline(cin, oname);
    cout << "Enter Operation Date (YYYY-MM-DD): "; getline(cin, odate);
    cout << "Enter Operation Time (HH:MM AM/PM): "; getline(cin, otime);
    Operation op1(oid, oname, odate, otime);
    op1.displayInfo();

    // ── OPD Input ─────────────────────────────
    int opdid;
    cout << "\nEnter OPD ID : "; cin >> opdid;
    OPD opd1(opdid);
    opd1.opddetails();

    // ── Departments (polymorphism) ─────────────
    vector<Department*> depts = {
        new ENT(1,         "Floor 1"),
        new Cardiology(2,  "Floor 2"),
        new Gynecology(3,  "Floor 3"),
        new Orthopedics(4, "Floor 4"),
        new Pediatrics(5,  "Floor 5")
    };
    cout << "\n--- All Departments ---\n";
    for (auto* dept : depts) {
        dept->dept_details();
        cout << "\n";
    }
    for (auto* dept : depts) delete dept;

    // ── Payment ───────────────────────────────
    double amount;
    cout << "Enter payment amount (Rs.): "; cin >> amount;
    rec.receive_payment(p1, amount);
    p1.pay_charges(amount);

    cout << "\n========================================\n";
    cout << "   System Shutdown. Have a good day!\n";
    cout << "========================================\n";
    return 0;
}
