#include "LabTestScreen.h"
#include "PatientActionsScreen.h"
#include "../db/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QTextEdit>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>

LabTestScreen::LabTestScreen(int patientId, const QString& prescriptionId, QWidget* parent)
    : QMainWindow(parent), m_patientId(patientId), m_prescriptionId(prescriptionId)
{
    setWindowTitle("Laboratory");
    setMinimumSize(900,580);
    resize(1050,660);
    auto* cw = new QWidget(this); setCentralWidget(cw);
    cw->setStyleSheet("background:#1C1C1C;");

    auto* lay = new QVBoxLayout(cw);
    lay->setContentsMargins(30,20,30,20); lay->setSpacing(12);

    auto* title = new QLabel("🧪  LABORATORY");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#C0392B;font-size:24px;font-weight:bold;");
    lay->addWidget(title);

    tbl = new QTableWidget(0,3,cw);
    tbl->setHorizontalHeaderLabels({"Test Name","Price (Rs.)","Available"});
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lay->addWidget(tbl,1);

    auto* row2 = new QHBoxLayout();
    auto* lbl  = new QLabel("Prescription ID:"); lbl->setStyleSheet("color:#F0F0F0;");
    edtPrescId = new QLineEdit(prescriptionId); edtPrescId->setFixedHeight(36);
    auto* btnProc = new QPushButton("🧪  PROCESS LAB TEST");
    btnProc->setFixedHeight(40); btnProc->setCursor(Qt::PointingHandCursor);
    row2->addWidget(lbl); row2->addWidget(edtPrescId,1); row2->addWidget(btnProc);
    lay->addLayout(row2);

    auto* btnBack = new QPushButton("← Back");
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:8px;font-weight:bold;");
    lay->addWidget(btnBack);

    connect(btnProc, &QPushButton::clicked, this, &LabTestScreen::onProcess);
    connect(btnBack, &QPushButton::clicked, [this](){
        auto* w = new PatientActionsScreen(m_patientId,"",m_prescriptionId); w->show(); close();
    });
    loadTests();
}

void LabTestScreen::loadTests() {
    auto tests = DatabaseManager::instance().getAllLabTests();
    tbl->setRowCount(0);
    for (const auto& t : tests) {
        int r = tbl->rowCount(); tbl->insertRow(r);
        tbl->setItem(r,0,new QTableWidgetItem(t.name));
        tbl->setItem(r,1,new QTableWidgetItem(QString::number(t.price,'f',0)));
        tbl->setItem(r,2,new QTableWidgetItem(t.available ? "Yes":"No"));
    }
}

QString LabTestScreen::generateTestResult(const QString& testName) {
    auto rng = QRandomGenerator::global();
    QString name = testName.toLower();
    if (name.contains("blood") || name.contains("cbc") || name.contains("complete")) {
        double wbc = 4.5 + rng->generateDouble()*5.5;
        double rbc = 4.0 + rng->generateDouble()*2.0;
        double hgb = 12.0 + rng->generateDouble()*4.0;
        int    plt = 150 + rng->bounded(200);
        return QString("WBC: %1 K/uL (Normal)  |  RBC: %2 M/uL (Normal)\n"
                       "Hemoglobin: %3 g/dL (Normal)  |  Platelets: %4 K/uL (Normal)\n"
                       "Status: ✅ All values within normal range")
               .arg(wbc,0,'f',1).arg(rbc,0,'f',1).arg(hgb,0,'f',1).arg(plt);
    } else if (name.contains("urine")) {
        return "Color: Yellow (Normal)  |  pH: 6.5 (Normal)\n"
               "Protein: Negative  |  Glucose: Negative\nStatus: ✅ Normal urinalysis";
    } else if (name.contains("x-ray") || name.contains("xray")) {
        return "Impression: No acute cardiopulmonary process identified.\n"
               "Lungs: Clear  |  Heart: Normal size\nStatus: ✅ Normal chest X-Ray";
    } else if (name.contains("mri")) {
        return "No intracranial mass, hemorrhage or midline shift.\n"
               "Ventricles and sulci: Normal for age\nStatus: ✅ Normal MRI";
    } else if (name.contains("ecg") || name.contains("ekg")) {
        int hr = 65 + rng->bounded(25);
        return QString("Heart Rate: %1 bpm (Normal)  |  Rhythm: Sinus\n"
                       "PR Interval: 160 ms  |  QT Interval: 380 ms\nStatus: ✅ Normal ECG").arg(hr);
    } else if (name.contains("sugar") || name.contains("glucose")) {
        int bs = 75 + rng->bounded(30);
        return QString("Fasting Blood Sugar: %1 mg/dL (Normal: 70–100)\nStatus: ✅ Normal").arg(bs);
    } else if (name.contains("liver")) {
        return "ALT: 28 U/L (Normal)  |  AST: 24 U/L (Normal)\n"
               "Bilirubin: 0.8 mg/dL  |  ALP: 75 U/L\nStatus: ✅ Normal liver function";
    } else if (name.contains("kidney") || name.contains("renal")) {
        double cr = 0.7 + rng->generateDouble()*0.5;
        return QString("Creatinine: %1 mg/dL (Normal)  |  BUN: 14 mg/dL (Normal)\n"
                       "eGFR: >90 mL/min  |  Uric Acid: 5.2 mg/dL\nStatus: ✅ Normal renal function")
               .arg(cr,0,'f',1);
    } else if (name.contains("thyroid") || name.contains("tsh")) {
        double tsh = 1.5 + rng->generateDouble()*2.0;
        return QString("TSH: %1 mIU/L (Normal: 0.4–4.0)\nT3: 1.2 ng/mL  |  T4: 8.5 ug/dL\nStatus: ✅ Normal thyroid function")
               .arg(tsh,0,'f',2);
    }
    return "Result: Within normal limits.\nStatus: ✅ Normal";
}

void LabTestScreen::onProcess() {
    QString pid = edtPrescId->text().trimmed();
    if (pid.isEmpty()) { QMessageBox::warning(this,"Error","Enter prescription ID."); return; }

    auto& db = DatabaseManager::instance();
    PrescriptionData pd = db.getPrescription(pid);
    if (pd.prescriptionId.isEmpty()) { QMessageBox::warning(this,"Not Found","Prescription not found."); return; }

    QJsonArray labsArr = QJsonDocument::fromJson(pd.labTests.toUtf8()).array();
    if (labsArr.isEmpty() || pd.labTests == "[]" || pd.labTests == "NONE") {
        QMessageBox::information(this,"Info","No lab tests prescribed by your doctor."); return;
    }

    double total = 0;
    QString receipt = "========== LAB TEST RECEIPT ==========\n";
    QString report  = "========== LAB TEST REPORT ===========\n";
    auto allTests   = db.getAllLabTests();

    for (const auto& lv : labsArr) {
        QString name = lv.toString();
        double price = 0;
        for (auto& t : allTests) if (t.name.compare(name,Qt::CaseInsensitive)==0) { price=t.price; break; }
        total += price;
        receipt += QString("%1  :  Rs. %2\n").arg(name,-30).arg(price,0,'f',0);
        report  += QString("Test   : %1\nResult : %2\n\n").arg(name, generateTestResult(name));
    }
    receipt += QString("--------------------------------------\nTotal  :  Rs. %1\n======================================").arg(total,0,'f',0);
    report  += "======================================";

    db.updateLabCost(m_patientId, total);

    auto showDlg = [this](const QString& title, const QString& text){
        auto* dlg = new QDialog(this); dlg->setWindowTitle(title); dlg->setMinimumSize(480,320);
        auto* l = new QVBoxLayout(dlg);
        auto* te = new QTextEdit(text); te->setReadOnly(true);
        te->setStyleSheet("font-family:Courier;font-size:12px;background:#1a1a1a;color:#F0F0F0;");
        auto* ok = new QPushButton("OK"); ok->setFixedHeight(36);
        connect(ok,&QPushButton::clicked,dlg,&QDialog::accept);
        l->addWidget(te); l->addWidget(ok); dlg->exec();
    };
    showDlg("Lab Test Receipt", receipt);
    showDlg("Lab Test Report",  report);
}
