#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QVector>
#include <QString>

// ── Data Transfer Objects ─────────────────────────────────────────────────────

struct PatientData {
    int     patientId   = 0;
    QString name;
    QString cnic;
    QString guardian;
    QString address;
    QString phone;
    int     age         = 0;
    QString gender;
    QString bloodGroup;
};

struct DoctorData {
    int     doctorId    = 0;
    QString name;
    QString department;
    QString specialty;
    int     available   = 1;
};

struct MedicineData {
    int     medicineId  = 0;
    QString name;
    double  pricePerUnit = 0.0;
    int     stock       = 0;
};

struct LabTestData {
    int     testId      = 0;
    QString name;
    double  price       = 0.0;
    int     available   = 1;
};

struct OperationData {
    int     operationId = 0;
    QString name;
    double  price       = 0.0;
    int     available   = 1;
};

struct PrescriptionData {
    QString prescriptionId;
    int     patientId   = 0;
    int     doctorId    = 0;
    QString medicines;    // JSON string
    QString labTests;     // JSON string
    QString operation;
    QString createdAt;
};

struct BillingData {
    int     billId          = 0;
    int     patientId       = 0;
    double  medicineCost    = 0.0;
    double  labtestCost     = 0.0;
    double  operationCost   = 0.0;
    double  totalCost       = 0.0;
    int     paid            = 0;
};

struct StaffData {
    int     staffId     = 0;
    QString name;
    QString role;
    QString shift;
    QString ward;
    QString username;
    bool    valid       = false;
};

struct DirectorData {
    int     directorId  = 0;
    QString name;
    QString username;
    double  bankBalance = 0.0;
    bool    valid       = false;
};

struct TransactionData {
    int     txnId       = 0;
    int     patientId   = 0;
    QString patientName;
    double  amount      = 0.0;
    QString txnDate;
};

struct PatientStatusData {
    int     statusId        = 0;
    int     patientId       = 0;
    QString patientName;
    QString status;
    QString assignedWard;
    int     assignedDoctor  = 0;
    QString updatedAt;
};

// ── DatabaseManager ───────────────────────────────────────────────────────────

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool initialize(const QString& dbPath);
    bool isConnected() const;

    // Patient CRUD
    int          insertPatient(const PatientData& p);
    bool         patientExists(int patientId);
    PatientData  getPatient(int patientId);
    bool         deletePatient(int patientId);

    // Prescription
    bool             insertPrescription(const PrescriptionData& p);
    PrescriptionData getPrescription(const QString& prescriptionId);

    // Billing
    bool        initBilling(int patientId);
    bool        updateMedicineCost(int patientId, double cost);
    bool        updateLabCost(int patientId, double cost);
    bool        updateOperationCost(int patientId, double cost);
    BillingData getBilling(int patientId);

    // Pharmacy
    QVector<MedicineData> getAllMedicines();
    bool updateMedicineStock(const QString& name, int deductQty);
    bool addMedicine(const MedicineData& m);
    bool updateMedicine(const MedicineData& m);
    bool deleteMedicine(int medicineId);

    // Lab Tests
    QVector<LabTestData> getAllLabTests();
    bool addLabTest(const LabTestData& t);
    bool updateLabTest(const LabTestData& t);
    bool deleteLabTest(int testId);

    // Operations
    QVector<OperationData> getAllOperations();
    bool addOperation(const OperationData& o);
    bool updateOperation(const OperationData& o);
    bool deleteOperation(int opId);

    // Doctors
    QVector<DoctorData> getAllDoctors();
    bool addDoctor(const DoctorData& d);
    bool updateDoctor(const DoctorData& d);
    bool deleteDoctor(int doctorId);

    // Staff
    StaffData authenticateStaff(const QString& username, const QString& password);

    // Director
    DirectorData             authenticateDirector(const QString& username, const QString& password);
    bool                     updateDirectorBalance(double amount);
    QVector<TransactionData> getTransactions();
    DirectorData             getDirector();

    // Discharge
    bool dischargePatient(int patientId, double totalBill, double amountPaid);

    // Patient Status
    bool                        insertPatientStatus(int patientId);
    bool                        updatePatientStatus(int patientId, const QString& status, const QString& ward);
    QVector<PatientStatusData>  getAllPatientStatuses();

    // Testing helpers
    bool testConnection();
    bool testInsertDeletePatient();
    bool testBillingUpdate();
    bool testPrescriptionInsert();

    // Direct DB access for raw queries
    QSqlDatabase& db() { return _db; }

private:
    DatabaseManager() = default;
    QSqlDatabase _db;
    bool createTablesIfNotExist();
    bool seedInitialData();
};

#endif
