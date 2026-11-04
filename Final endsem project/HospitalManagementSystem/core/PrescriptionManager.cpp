#include "PrescriptionManager.h"
#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

string PrescriptionManager::getPrescriptionPath(const string& prescriptionId) {
    QString appDir = QCoreApplication::applicationDirPath();
    QString path = appDir + "/prescriptions/" + QString::fromStdString(prescriptionId) + ".txt";
    return path.toStdString();
}

string PrescriptionManager::generatePrescriptionId(int patientId, int doctorId) {
    ostringstream oss;
    oss << "P" << setw(4) << setfill('0') << patientId
        << "D" << setw(4) << setfill('0') << doctorId;
    return oss.str();
}

void PrescriptionManager::writePrescriptionFile(
    const string& prescriptionId,
    const string& patientName,
    const string& doctorName,
    const vector<pair<string,int>>& medicines,
    const vector<string>& labTests,
    const string& operation)
{
    // Ensure prescriptions directory exists
    QString appDir = QCoreApplication::applicationDirPath();
    QDir dir(appDir);
    dir.mkpath("prescriptions");

    string path = getPrescriptionPath(prescriptionId);

    // Get current date
    time_t now = time(nullptr);
    char dateBuf[16];
    strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", localtime(&now));

    ofstream out(path);
    if (!out.is_open()) return;

    out << "========================================\n";
    out << "     HOSPITAL MANAGEMENT SYSTEM\n";
    out << "         DOCTOR'S PRESCRIPTION\n";
    out << "========================================\n";
    out << "Prescription ID : " << prescriptionId << "\n";
    out << "Patient Name    : " << patientName    << "\n";
    out << "Doctor Name     : " << doctorName     << "\n";
    out << "Date            : " << dateBuf        << "\n";
    out << "----------------------------------------\n";
    out << "MEDICINES:\n";
    if (medicines.empty()) {
        out << "  NONE\n";
    } else {
        for (const auto& m : medicines)
            out << "  - " << m.first << " x " << m.second << "\n";
    }
    out << "----------------------------------------\n";
    out << "LAB TESTS:\n";
    if (labTests.empty()) {
        out << "  NONE\n";
    } else {
        for (const auto& lt : labTests)
            out << "  - " << lt << "\n";
    }
    out << "----------------------------------------\n";
    out << "OPERATION:\n";
    if (operation.empty() || operation == "NONE") {
        out << "  NONE\n";
    } else {
        out << "  - " << operation << "\n";
    }
    out << "========================================\n";
    out.close();
}

string PrescriptionManager::readPrescriptionFile(const string& prescriptionId) {
    string path = getPrescriptionPath(prescriptionId);
    ifstream in(path);
    if (!in.is_open()) return "";
    ostringstream oss;
    oss << in.rdbuf();
    return oss.str();
}

bool PrescriptionManager::prescriptionExists(const string& prescriptionId) {
    string path = getPrescriptionPath(prescriptionId);
    ifstream in(path);
    return in.good();
}
