#include "DatabaseManager.h"
#include <QDebug>

// ── Patient CRUD ──────────────────────────────────────────────────────────────

int DatabaseManager::insertPatient(const PatientData& p) {
    QSqlQuery q(_db);
    q.prepare("INSERT INTO patients(name,cnic,guardian,address,phone,age,gender,blood_group) "
              "VALUES(:n,:c,:g,:a,:ph,:ag,:ge,:bg)");
    q.bindValue(":n",  p.name);
    q.bindValue(":c",  p.cnic);
    q.bindValue(":g",  p.guardian);
    q.bindValue(":a",  p.address);
    q.bindValue(":ph", p.phone);
    q.bindValue(":ag", p.age);
    q.bindValue(":ge", p.gender);
    q.bindValue(":bg", p.bloodGroup);
    if (!q.exec()) { qDebug() << q.lastError(); return -1; }
    return q.lastInsertId().toInt();
}

bool DatabaseManager::patientExists(int patientId) {
    QSqlQuery q(_db);
    q.prepare("SELECT COUNT(*) FROM patients WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    q.exec();
    return q.next() && q.value(0).toInt() > 0;
}

PatientData DatabaseManager::getPatient(int patientId) {
    PatientData p;
    QSqlQuery q(_db);
    q.prepare("SELECT * FROM patients WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    if (q.exec() && q.next()) {
        p.patientId  = q.value("patient_id").toInt();
        p.name       = q.value("name").toString();
        p.cnic       = q.value("cnic").toString();
        p.guardian   = q.value("guardian").toString();
        p.address    = q.value("address").toString();
        p.phone      = q.value("phone").toString();
        p.age        = q.value("age").toInt();
        p.gender     = q.value("gender").toString();
        p.bloodGroup = q.value("blood_group").toString();
    }
    return p;
}

bool DatabaseManager::deletePatient(int patientId) {
    QSqlQuery q(_db);
    q.prepare("DELETE FROM patients WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    return q.exec();
}

// ── Prescription ──────────────────────────────────────────────────────────────

bool DatabaseManager::insertPrescription(const PrescriptionData& p) {
    QSqlQuery q(_db);
    q.prepare("INSERT INTO prescriptions(prescription_id,patient_id,doctor_id,medicines,lab_tests,operation) "
              "VALUES(:pid,:pat,:doc,:med,:lab,:op)");
    q.bindValue(":pid", p.prescriptionId);
    q.bindValue(":pat", p.patientId);
    q.bindValue(":doc", p.doctorId);
    q.bindValue(":med", p.medicines);
    q.bindValue(":lab", p.labTests);
    q.bindValue(":op",  p.operation);
    if (!q.exec()) { qDebug() << q.lastError(); return false; }
    return true;
}

PrescriptionData DatabaseManager::getPrescription(const QString& prescriptionId) {
    PrescriptionData p;
    QSqlQuery q(_db);
    q.prepare("SELECT * FROM prescriptions WHERE prescription_id=:id");
    q.bindValue(":id", prescriptionId);
    if (q.exec() && q.next()) {
        p.prescriptionId = q.value("prescription_id").toString();
        p.patientId      = q.value("patient_id").toInt();
        p.doctorId       = q.value("doctor_id").toInt();
        p.medicines      = q.value("medicines").toString();
        p.labTests       = q.value("lab_tests").toString();
        p.operation      = q.value("operation").toString();
        p.createdAt      = q.value("created_at").toString();
    }
    return p;
}

// ── Billing ───────────────────────────────────────────────────────────────────

bool DatabaseManager::initBilling(int patientId) {
    QSqlQuery q(_db);
    q.prepare("INSERT INTO billing(patient_id) VALUES(:id)");
    q.bindValue(":id", patientId);
    return q.exec();
}

bool DatabaseManager::updateMedicineCost(int patientId, double cost) {
    QSqlQuery q(_db);
    q.prepare("UPDATE billing SET medicine_cost=:c, "
              "total_cost=medicine_cost+:c+labtest_cost+operation_cost "
              "WHERE patient_id=:id");
    q.bindValue(":c",  cost);
    q.bindValue(":id", patientId);
    if (!q.exec()) return false;
    q.prepare("UPDATE billing SET total_cost=medicine_cost+labtest_cost+operation_cost WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    return q.exec();
}

bool DatabaseManager::updateLabCost(int patientId, double cost) {
    QSqlQuery q(_db);
    q.prepare("UPDATE billing SET labtest_cost=:c WHERE patient_id=:id");
    q.bindValue(":c",  cost);
    q.bindValue(":id", patientId);
    if (!q.exec()) return false;
    q.prepare("UPDATE billing SET total_cost=medicine_cost+labtest_cost+operation_cost WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    return q.exec();
}

bool DatabaseManager::updateOperationCost(int patientId, double cost) {
    QSqlQuery q(_db);
    q.prepare("UPDATE billing SET operation_cost=:c WHERE patient_id=:id");
    q.bindValue(":c",  cost);
    q.bindValue(":id", patientId);
    if (!q.exec()) return false;
    q.prepare("UPDATE billing SET total_cost=medicine_cost+labtest_cost+operation_cost WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    return q.exec();
}

BillingData DatabaseManager::getBilling(int patientId) {
    BillingData b;
    QSqlQuery q(_db);
    q.prepare("SELECT * FROM billing WHERE patient_id=:id");
    q.bindValue(":id", patientId);
    if (q.exec() && q.next()) {
        b.billId        = q.value("bill_id").toInt();
        b.patientId     = patientId;
        b.medicineCost  = q.value("medicine_cost").toDouble();
        b.labtestCost   = q.value("labtest_cost").toDouble();
        b.operationCost = q.value("operation_cost").toDouble();
        b.totalCost     = q.value("total_cost").toDouble();
        b.paid          = q.value("paid").toInt();
    }
    return b;
}

