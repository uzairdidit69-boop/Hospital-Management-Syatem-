#ifndef PATIENT_H
#define PATIENT_H

#include <iostream>
#include <string>
using namespace std;

class Patient {
private:
    int    pid;
    string pname;
    string cnic;
    string guardian;
    string address;
    string phone;
    int    age;
    string gender;
    string blood_group;

public:
    Patient(int id, string name,
            string cnic = "", string guardian = "",
            string address = "", string phone = "",
            int age = 0, string gender = "",
            string blood_group = "");

    void pdetails() const;
    void pay_charges(double amount) const;

    int    getPid()        const { return pid;        }
    string getPname()      const { return pname;      }
    string getCnic()       const { return cnic;       }
    string getGuardian()   const { return guardian;   }
    string getAddress()    const { return address;    }
    string getPhone()      const { return phone;      }
    int    getAge()        const { return age;        }
    string getGender()     const { return gender;     }
    string getBloodGroup() const { return blood_group;}
};

#endif
