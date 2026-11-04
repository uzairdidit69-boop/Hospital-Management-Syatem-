#include "DatabaseManager.h"
#include <QDebug>
#include <QDate>

// ── Staff ─────────────────────────────────────────────────────────────────────

StaffData DatabaseManager::authenticateStaff(const QString& username, const QString& password) {
    StaffData s;
    QSqlQuery q(_db);
    q.prepare("SELECT * FROM staff WHERE username=:u AND password=:p");
    q.bindValue(":u", username);
    q.bindValue(":p", password);
    if (q.exec() && q.next()) {
        s.staffId  = q.value("staff_id").toInt();
        s.name     = q.value("name").toString();
        s.role     = q.value("role").toString();
        s.shift    = q.value("shift").toString();
        s.ward     = q.value("ward").toString();
        s.username = q.value("username").toString();
        s.valid    = true;
    }
    return s;
}

// ── Director ──────────────────────────────────────────────────────────────────

DirectorData DatabaseManager::authenticateDirector(const QString& username, const QString& password) {
    DirectorData d;
    QSqlQuery q(_db);
    q.prepare("SELECT * FROM director WHERE username=:u AND password=:p");
    q.bindValue(":u", username);
    q.bindValue(":p", password);
    if (q.exec() && q.next()) {
        d.directorId  = q.value("director_id").toInt();
        d.name        = q.value("name").toString();
        d.username    = q.value("username").toString();
        d.bankBalance = q.value("bank_balance").toDouble();
        d.valid       = true;
    }
    return d;
}

DirectorData DatabaseManager::getDirector() {
    DirectorData d;
    QSqlQuery q(_db);
    q.exec("SELECT * FROM director LIMIT 1");
    if (q.next()) {
        d.directorId  = q.value("director_id").toInt();
        d.name        = q.value("name").toString();
        d.username    = q.value("username").toString();
        d.bankBalance = q.value("bank_balance").toDouble();
        d.valid       = true;
    }
    return d;
}

bool DatabaseManager::updateDirectorBalance(double amount) {
    QSqlQuery q(_db);
    q.prepare("UPDATE director SET bank_balance=bank_balance+:a WHERE director_id=1");
    q.bindValue(":a", amount);
    return q.exec();
}

QVector<TransactionData> DatabaseManager::getTransactions() {
    QVector<TransactionData> list;
    QSqlQuery q(_db);
    q.exec("SELECT * FROM director_transactions ORDER BY txn_date DESC");
    while (q.next()) {
        TransactionData t;
        t.txnId       = q.value("txn_id").toInt();
        t.patientId   = q.value("patient_id").toInt();
        t.patientName = q.value("patient_name").toString();
        t.amount      = q.value("amount").toDouble();
        t.txnDate     = q.value("txn_date").toString();
        list.append(t);
    }
    return list;
}

// ── Discharge ─────────────────────────────────────────────────────────────────

bool DatabaseManager::dischargePatient(int patientId, double totalBill, double amountPaid) {
    QSqlQuery q(_db);
    PatientData p = getPatient(patientId);

    // Archive patient
    q.prepare("INSERT INTO discharged_patients(patient_id,name,cnic,total_bill,amount_paid) "
              "VALUES(:id,:n,:c,:t,:a)");
    q.bindValue(":id", patientId);
    q.bindValue(":n",  p.name);
    q.bindValue(":c",  p.cnic);
    q.bindValue(":t",  totalBill);
    q.bindValue(":a",  amountPaid);
    if (!q.exec()) { qDebug() << q.lastError(); return false; }

    // Log transaction
    q.prepare("INSERT INTO director_transactions(patient_id,patient_name,amount) VALUES(:id,:n,:a)");
    q.bindValue(":id", patientId);
    q.bindValue(":n",  p.name);
    q.bindValue(":a",  totalBill);
    q.exec();

    // Update director balance
    updateDirectorBalance(totalBill);

    // Mark billing paid
    q.prepare("UPDATE billing SET paid=1 WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    q.exec();

    // Delete patient and status
    q.prepare("DELETE FROM patient_status WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    q.exec();

    q.prepare("DELETE FROM patients WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    q.exec();

    return true;
}

// ── Patient Status ────────────────────────────────────────────────────────────

bool DatabaseManager::insertPatientStatus(int patientId) {
    QSqlQuery q(_db);
    q.prepare("INSERT INTO patient_status(patient_id,status) VALUES(:id,'Admitted')");
    q.bindValue(":id", patientId);
    return q.exec();
}

bool DatabaseManager::updatePatientStatus(int patientId, const QString& status, const QString& ward) {
    QSqlQuery q(_db);
    q.prepare("UPDATE patient_status SET status=:s,assigned_ward=:w,"
              "updated_at=datetime('now') WHERE patient_id=:id");
    q.bindValue(":s",  status);
    q.bindValue(":w",  ward);
    q.bindValue(":id", patientId);
    return q.exec();
}

QVector<PatientStatusData> DatabaseManager::getAllPatientStatuses() {
    QVector<PatientStatusData> list;
    QSqlQuery q(_db);
    q.exec("SELECT ps.*, p.name as patient_name FROM patient_status ps "
           "JOIN patients p ON ps.patient_id=p.patient_id ORDER BY ps.updated_at DESC");
    while (q.next()) {
        PatientStatusData ps;
        ps.statusId      = q.value("status_id").toInt();
        ps.patientId     = q.value("patient_id").toInt();
        ps.patientName   = q.value("patient_name").toString();
        ps.status        = q.value("status").toString();
        ps.assignedWard  = q.value("assigned_ward").toString();
        ps.assignedDoctor= q.value("assigned_doctor").toInt();
        ps.updatedAt     = q.value("updated_at").toString();
        list.append(ps);
    }
    return list;
}

// ── Testing ───────────────────────────────────────────────────────────────────

bool DatabaseManager::testConnection() { return isConnected(); }

bool DatabaseManager::testInsertDeletePatient() {
    PatientData p;
    p.name="TEST_PATIENT"; p.cnic="99999-9999999-9";
    p.guardian="Test"; p.address="Test"; p.phone="0000000000";
    p.age=25; p.gender="Male"; p.bloodGroup="O+";
    int id = insertPatient(p);
    if (id < 0) return false;
    bool ok = patientExists(id);
    deletePatient(id);
    return ok && !patientExists(id);
}

bool DatabaseManager::testBillingUpdate() {
    PatientData p;
    p.name="BILL_TEST"; p.cnic="88888-8888888-8";
    p.guardian="Test"; p.address="Test"; p.phone="1111111111";
    p.age=30; p.gender="Female"; p.bloodGroup="A+";
    int id = insertPatient(p);
    if (id < 0) return false;
    initBilling(id);
    updateMedicineCost(id, 500.0);
    BillingData b = getBilling(id);
    bool ok = (b.medicineCost == 500.0);
    deletePatient(id);
    return ok;
}

bool DatabaseManager::testPrescriptionInsert() {
    PrescriptionData p;
    p.prescriptionId = "P9999D9999";
    p.patientId = 9999; p.doctorId = 9999;
    p.medicines = "[]"; p.labTests = "[]"; p.operation = "NONE";
    bool ins = insertPrescription(p);
    if (!ins) return false;
    PrescriptionData got = getPrescription("P9999D9999");
    QSqlQuery q(_db);
    q.exec("DELETE FROM prescriptions WHERE prescription_id='P9999D9999'");
    return got.prescriptionId == "P9999D9999";
}

