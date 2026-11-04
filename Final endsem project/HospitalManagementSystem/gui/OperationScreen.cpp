#include "OperationScreen.h"
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
#include <QRandomGenerator>

OperationScreen::OperationScreen(int patientId, const QString& prescriptionId, QWidget* parent)
    : QMainWindow(parent), m_patientId(patientId), m_prescriptionId(prescriptionId)
{
    setWindowTitle("Operation Theatre");
    setMinimumSize(900,560);
    resize(1050,640);
    auto* cw = new QWidget(this); setCentralWidget(cw);
    cw->setStyleSheet("background:#1C1C1C;");

    auto* lay = new QVBoxLayout(cw);
    lay->setContentsMargins(30,20,30,20); lay->setSpacing(12);

    auto* title = new QLabel("🔪  OPERATION THEATRE");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#C0392B;font-size:24px;font-weight:bold;");
    lay->addWidget(title);

    tbl = new QTableWidget(0,3,cw);
    tbl->setHorizontalHeaderLabels({"Operation Name","Price (Rs.)","Available"});
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lay->addWidget(tbl,1);

    auto* row2 = new QHBoxLayout();
    auto* lbl  = new QLabel("Prescription ID:"); lbl->setStyleSheet("color:#F0F0F0;");
    edtPrescId = new QLineEdit(prescriptionId); edtPrescId->setFixedHeight(36);
    auto* btnChk = new QPushButton("🔪  CHECK OPERATION");
    btnChk->setFixedHeight(40); btnChk->setCursor(Qt::PointingHandCursor);
    row2->addWidget(lbl); row2->addWidget(edtPrescId,1); row2->addWidget(btnChk);
    lay->addLayout(row2);

    auto* btnBack = new QPushButton("← Back");
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:8px;font-weight:bold;");
    lay->addWidget(btnBack);

    connect(btnChk,  &QPushButton::clicked, this, &OperationScreen::onCheck);
    connect(btnBack, &QPushButton::clicked, [this](){
        auto* w = new PatientActionsScreen(m_patientId,"",m_prescriptionId); w->show(); close();
    });
    loadOperations();
}

void OperationScreen::loadOperations() {
    auto ops = DatabaseManager::instance().getAllOperations();
    tbl->setRowCount(0);
    for (const auto& o : ops) {
        int r = tbl->rowCount(); tbl->insertRow(r);
        tbl->setItem(r,0,new QTableWidgetItem(o.name));
        tbl->setItem(r,1,new QTableWidgetItem(QString::number(o.price,'f',0)));
        tbl->setItem(r,2,new QTableWidgetItem(o.available?"Yes":"No"));
    }
}

QString OperationScreen::generateOperationReport(const QString& opName, const QString& surgeon) {
    auto rng = QRandomGenerator::global();
    int hours = 1 + rng->bounded(4);
    int mins  = rng->bounded(60);
    QString name = opName.toLower();

    QString notes;
    if (name.contains("appendectomy"))
        notes = "Patient responded well to anesthesia.\nNo complications observed.\nRecommended rest: 5-7 days.";
    else if (name.contains("brain"))
        notes = "Tumor successfully resected with clear margins.\nNeurological functions preserved.\nICU monitoring recommended for 48 hours.";
    else if (name.contains("bypass") || name.contains("coronary"))
        notes = "Three vessel bypass performed successfully.\nCardiopulmonary bypass time: 95 minutes.\nCardiac function stable post-operation.";
    else if (name.contains("knee") || name.contains("replacement"))
        notes = "Prosthetic implant seated correctly.\nRange of motion satisfactory.\nPhysiotherapy recommended: 6 weeks.";
    else if (name.contains("cataract"))
        notes = "Lens replaced with intraocular implant.\nVision clarity restored.\nAnti-inflammatory drops prescribed for 4 weeks.";
    else if (name.contains("hernia"))
        notes = "Mesh repair completed without complications.\nBowel integrity confirmed.\nAvoid heavy lifting for 6 weeks.";
    else if (name.contains("tonsil"))
        notes = "Bilateral tonsillectomy completed.\nHemostasis achieved.\nSoft diet recommended for 10 days.";
    else
        notes = "Procedure completed without complications.\nPatient vitals stable throughout.\nRecommended follow-up in 1 week.";

    return QString("========== OPERATION REPORT ==========\n"
                   "Operation   : %1\n"
                   "Surgeon     : %2\n"
                   "Duration    : %3 hours %4 minutes\n"
                   "Status      : ✅ Operation Successful\n"
                   "Notes       : %5\n"
                   "======================================")
           .arg(opName, surgeon).arg(hours).arg(mins, 2, 10, QChar('0')).arg(notes);
}

void OperationScreen::onCheck() {
    QString pid = edtPrescId->text().trimmed();
    if (pid.isEmpty()) { QMessageBox::warning(this,"Error","Enter prescription ID."); return; }

    auto& db = DatabaseManager::instance();
    PrescriptionData pd = db.getPrescription(pid);
    if (pd.prescriptionId.isEmpty()) { QMessageBox::warning(this,"Not Found","Prescription not found."); return; }

    if (pd.operation.isEmpty() || pd.operation == "NONE") {
        QMessageBox::information(this,"Info","No operation prescribed by your doctor."); return;
    }

    double price = 0;
    auto allOps  = db.getAllOperations();
    for (auto& o : allOps) if (o.name.compare(pd.operation,Qt::CaseInsensitive)==0) { price=o.price; break; }

    db.updateOperationCost(m_patientId, price);

    QMessageBox::information(this,"Operation Scheduled",
        QString("✅ Operation Scheduled!\n\nOperation: %1\nCost: Rs. %2\n\nPlease proceed to Operation Theatre.")
        .arg(pd.operation).arg(price,0,'f',0));

    DoctorData doc = {};
    auto docs = db.getAllDoctors();
    if (!docs.isEmpty()) doc = docs.first();

    QString report = generateOperationReport(pd.operation, doc.name.isEmpty() ? "Dr. On-Call" : doc.name);
    auto* dlg = new QDialog(this); dlg->setWindowTitle("Operation Report"); dlg->setMinimumSize(480,300);
    auto* l = new QVBoxLayout(dlg);
    auto* te = new QTextEdit(report); te->setReadOnly(true);
    te->setStyleSheet("font-family:Courier;font-size:12px;background:#1a1a1a;color:#F0F0F0;");
    auto* ok = new QPushButton("OK"); ok->setFixedHeight(36);
    connect(ok,&QPushButton::clicked,dlg,&QDialog::accept);
    l->addWidget(te); l->addWidget(ok); dlg->exec();
}
