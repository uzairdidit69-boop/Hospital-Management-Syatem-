#include "Test.h"

Test::Test(int id, string name) : tid(id), tname(name) {}

void Test::collect_sample() {
    cout << "\n[Test] Sample collected for: " << tname << " (ID: " << tid << ")\n";
}

void Test::report_generation() {
    cout << "[Test] Report generated for: " << tname << " (ID: " << tid << ")\n";
}
