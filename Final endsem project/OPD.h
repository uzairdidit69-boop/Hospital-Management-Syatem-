#ifndef OPD_H
#define OPD_H

#include <iostream>
using namespace std;

class OPD {
private:
    int opdid;
public:
    OPD(int id);
    void opddetails() const;
};

#endif
