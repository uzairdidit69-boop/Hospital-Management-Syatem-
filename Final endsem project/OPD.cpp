#include "OPD.h"

OPD::OPD(int id) : opdid(id) {}

void OPD::opddetails() const {
    cout << "\n--- OPD Details ---\n";
    cout << "OPD ID  : " << opdid << "\n";
    cout << "Status  : Open\n";
    cout << "Service : General Outpatient Consultation\n";
}
