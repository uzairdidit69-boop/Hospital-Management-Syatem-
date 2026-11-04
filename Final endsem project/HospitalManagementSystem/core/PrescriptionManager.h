#ifndef PRESCRIPTIONMANAGER_H
#define PRESCRIPTIONMANAGER_H

#include <string>
#include <vector>
using namespace std;

class PrescriptionManager {
public:
    // Generate prescription ID: "P" + zero-padded patient_id + "D" + zero-padded doctor_id
    static string generatePrescriptionId(int patientId, int doctorId);

    // Write prescription to prescriptions/P{pid}D{did}.txt
    static void writePrescriptionFile(
        const string& prescriptionId,
        const string& patientName,
        const string& doctorName,
        const vector<pair<string,int>>& medicines,   // {name, qty}
        const vector<string>& labTests,
        const string& operation
    );

    // Read and return full prescription text from file
    static string readPrescriptionFile(const string& prescriptionId);

    // Check if prescription file exists
    static bool prescriptionExists(const string& prescriptionId);

private:
    static string getPrescriptionPath(const string& prescriptionId);
};

#endif
