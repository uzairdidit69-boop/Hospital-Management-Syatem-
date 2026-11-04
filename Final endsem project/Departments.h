#ifndef DEPARTMENTS_H
#define DEPARTMENTS_H

#include "Department.h"

class ENT : public Department {
public:
    using Department::Department;
    void dept_details() const override;
};

class Cardiology : public Department {
public:
    using Department::Department;
    void dept_details() const override;
};

class Gynecology : public Department {
public:
    using Department::Department;
    void dept_details() const override;
};

class Orthopedics : public Department {
public:
    using Department::Department;
    void dept_details() const override;
};

class Pediatrics : public Department {
public:
    using Department::Department;
    void dept_details() const override;
};

#endif
