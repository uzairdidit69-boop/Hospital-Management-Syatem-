#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <string>
using namespace std;

class Test {
private:
    int    tid;
    string tname;
public:
    Test(int id, string name);
    void report_generation();
    void collect_sample();
    int    getTid()   const { return tid;   }
    string getTname() const { return tname; }
};

#endif
