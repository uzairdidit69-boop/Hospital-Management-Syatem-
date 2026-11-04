#include "../db/DatabaseManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QString>

static void printResult(const QString& test, bool passed) {
    qDebug().noquote() << QString("[TEST] %-26s : %1").arg(test).arg(passed ? "✅ PASS" : "❌ FAIL");
}

void runDatabaseTests() {
    qDebug() << "\n========== DATABASE TESTS ==========";

    QString dbPath = QCoreApplication::applicationDirPath() + "/db/hospital.db";
    QDir().mkpath(QCoreApplication::applicationDirPath() + "/db");

    auto& db = DatabaseManager::instance();

    // 1. Connection test
    bool conn = db.initialize(dbPath) && db.isConnected();
    printResult("DB Connection", conn);
    if (!conn) { qDebug() << "Aborting — no DB connection."; return; }

    // 2. Insert / Delete patient
    bool insertDel = db.testInsertDeletePatient();
    printResult("Patient Insert/Delete", insertDel);

    // 3. Billing update
    bool billing = db.testBillingUpdate();
    printResult("Billing Update", billing);

    // 4. Prescription CRUD
    bool presc = db.testPrescriptionInsert();
    printResult("Prescription CRUD", presc);

    // 5. Stock deduction
    auto meds = db.getAllMedicines();
    bool stockOk = false;
    if (!meds.isEmpty()) {
        MedicineData& m = meds[0];
        int originalStock = m.stock;
        stockOk = db.updateMedicineStock(m.name, 1);
        if (stockOk) {
            // Restore stock
            MedicineData restored = m;
            restored.stock = originalStock;
            db.updateMedicine(restored);
        }
    }
    printResult("Stock Deduction", stockOk);

    // 6. Director balance
    DirectorData before = db.getDirector();
    bool balOk = db.updateDirectorBalance(1000.0);
    if (balOk) {
        db.updateDirectorBalance(-1000.0); // restore
    }
    printResult("Director Balance", balOk);

    qDebug() << "====================================\n";
}
