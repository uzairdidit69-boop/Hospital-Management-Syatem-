#include "DatabaseManager.h"
#include <QDebug>

// ── Pharmacy ──────────────────────────────────────────────────────────────────

QVector<MedicineData> DatabaseManager::getAllMedicines() {
    QVector<MedicineData> list;
    QSqlQuery q(_db);
    q.exec("SELECT * FROM pharmacy ORDER BY name");
    while (q.next()) {
        MedicineData m;
        m.medicineId   = q.value("medicine_id").toInt();
        m.name         = q.value("name").toString();
        m.pricePerUnit = q.value("price_per_unit").toDouble();
        m.stock        = q.value("stock").toInt();
        list.append(m);
    }
    return list;
}

bool DatabaseManager::updateMedicineStock(const QString& name, int deductQty) {
    QSqlQuery q(_db);
    q.prepare("UPDATE pharmacy SET stock=stock-:qty WHERE name=:n AND stock>=:qty");
    q.bindValue(":qty", deductQty);
    q.bindValue(":n",   name);
    return q.exec() && q.numRowsAffected() > 0;
}

bool DatabaseManager::addMedicine(const MedicineData& m) {
    QSqlQuery q(_db);
    q.prepare("INSERT INTO pharmacy(name,price_per_unit,stock) VALUES(:n,:p,:s)");
    q.bindValue(":n", m.name);
    q.bindValue(":p", m.pricePerUnit);
    q.bindValue(":s", m.stock);
    return q.exec();
}

bool DatabaseManager::updateMedicine(const MedicineData& m) {
    QSqlQuery q(_db);
    q.prepare("UPDATE pharmacy SET name=:n,price_per_unit=:p,stock=:s WHERE medicine_id=:id");
    q.bindValue(":n",  m.name);
    q.bindValue(":p",  m.pricePerUnit);
    q.bindValue(":s",  m.stock);
    q.bindValue(":id", m.medicineId);
    return q.exec();
}

bool DatabaseManager::deleteMedicine(int medicineId) {
    QSqlQuery q(_db);
    q.prepare("DELETE FROM pharmacy WHERE medicine_id=:id");
    q.bindValue(":id", medicineId);
    return q.exec();
}

// ── Lab Tests ─────────────────────────────────────────────────────────────────

QVector<LabTestData> DatabaseManager::getAllLabTests() {
    QVector<LabTestData> list;
    QSqlQuery q(_db);
    q.exec("SELECT * FROM lab_tests ORDER BY name");
    while (q.next()) {
        LabTestData t;
        t.testId    = q.value("test_id").toInt();
        t.name      = q.value("name").toString();
        t.price     = q.value("price").toDouble();
        t.available = q.value("available").toInt();
        list.append(t);
    }
    return list;
}

bool DatabaseManager::addLabTest(const LabTestData& t) {
    QSqlQuery q(_db);
    q.prepare("INSERT INTO lab_tests(name,price,available) VALUES(:n,:p,:a)");
    q.bindValue(":n", t.name);
    q.bindValue(":p", t.price);
    q.bindValue(":a", t.available);
    return q.exec();
}

bool DatabaseManager::updateLabTest(const LabTestData& t) {
    QSqlQuery q(_db);
    q.prepare("UPDATE lab_tests SET name=:n,price=:p,available=:a WHERE test_id=:id");
    q.bindValue(":n",  t.name);
    q.bindValue(":p",  t.price);
    q.bindValue(":a",  t.available);
    q.bindValue(":id", t.testId);
    return q.exec();
}

bool DatabaseManager::deleteLabTest(int testId) {
    QSqlQuery q(_db);
    q.prepare("DELETE FROM lab_tests WHERE test_id=:id");
    q.bindValue(":id", testId);
    return q.exec();
}

// ── Operations ────────────────────────────────────────────────────────────────

QVector<OperationData> DatabaseManager::getAllOperations() {
    QVector<OperationData> list;
    QSqlQuery q(_db);
    q.exec("SELECT * FROM operations ORDER BY name");
    while (q.next()) {
        OperationData o;
        o.operationId = q.value("operation_id").toInt();
        o.name        = q.value("name").toString();
        o.price       = q.value("price").toDouble();
        o.available   = q.value("available").toInt();
        list.append(o);
    }
    return list;
}

bool DatabaseManager::addOperation(const OperationData& o) {
    QSqlQuery q(_db);
    q.prepare("INSERT INTO operations(name,price,available) VALUES(:n,:p,:a)");
    q.bindValue(":n", o.name);
    q.bindValue(":p", o.price);
    q.bindValue(":a", o.available);
    return q.exec();
}

bool DatabaseManager::updateOperation(const OperationData& o) {
    QSqlQuery q(_db);
    q.prepare("UPDATE operations SET name=:n,price=:p,available=:a WHERE operation_id=:id");
    q.bindValue(":n",  o.name);
    q.bindValue(":p",  o.price);
    q.bindValue(":a",  o.available);
    q.bindValue(":id", o.operationId);
    return q.exec();
}

bool DatabaseManager::deleteOperation(int opId) {
    QSqlQuery q(_db);
    q.prepare("DELETE FROM operations WHERE operation_id=:id");
    q.bindValue(":id", opId);
    return q.exec();
}

// ── Doctors ───────────────────────────────────────────────────────────────────

QVector<DoctorData> DatabaseManager::getAllDoctors() {
    QVector<DoctorData> list;
    QSqlQuery q(_db);
    q.exec("SELECT * FROM doctors ORDER BY name");
    while (q.next()) {
        DoctorData d;
        d.doctorId   = q.value("doctor_id").toInt();
        d.name       = q.value("name").toString();
        d.department = q.value("department").toString();
        d.specialty  = q.value("specialty").toString();
        d.available  = q.value("available").toInt();
        list.append(d);
    }
    return list;
}

bool DatabaseManager::addDoctor(const DoctorData& d) {
    QSqlQuery q(_db);
    q.prepare("INSERT INTO doctors(name,department,specialty,available) VALUES(:n,:dep,:sp,:a)");
    q.bindValue(":n",   d.name);
    q.bindValue(":dep", d.department);
    q.bindValue(":sp",  d.specialty);
    q.bindValue(":a",   d.available);
    return q.exec();
}

bool DatabaseManager::updateDoctor(const DoctorData& d) {
    QSqlQuery q(_db);
    q.prepare("UPDATE doctors SET name=:n,department=:dep,specialty=:sp,available=:a WHERE doctor_id=:id");
    q.bindValue(":n",   d.name);
    q.bindValue(":dep", d.department);
    q.bindValue(":sp",  d.specialty);
    q.bindValue(":a",   d.available);
    q.bindValue(":id",  d.doctorId);
    return q.exec();
}

bool DatabaseManager::deleteDoctor(int doctorId) {
    QSqlQuery q(_db);
    q.prepare("DELETE FROM doctors WHERE doctor_id=:id");
    q.bindValue(":id", doctorId);
    return q.exec();
}

