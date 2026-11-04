#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <iostream>
#include <string>
using namespace std;

class Department {
private:
    int    dept_id;
    string d_location;
public:
    Department(int id, string loc);
    virtual void dept_details() const = 0;
    virtual ~Department() = default;
    int    getDeptId()    const { return dept_id;    }
    string getDLocation() const { return d_location; }
};

#endif
