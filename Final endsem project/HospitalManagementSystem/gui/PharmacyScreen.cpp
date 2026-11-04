#include "PharmacyScreen.h"
#include "PatientActionsScreen.h"
#include "../db/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

PharmacyScreen::PharmacyScreen(int patientId, const QString& prescriptionId, QWidget* parent)
    : QMainWindow(parent), m_patientId(patientId), m_prescriptionId(prescriptionId)
{
    setWindowTitle("Pharmacy");
    setMinimumSize(900,600);
    resize(1050,680);
    auto* cw = new QWidget(this);
    setCentralWidget(cw);
    cw->setStyleSheet("background:#1C1C1C;");

    auto* lay = new QVBoxLayout(cw);
    lay->setContentsMargins(30,20,30,20);
    lay->setSpacing(12);

    auto* title = new QLabel("💊  PHARMACY");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#C0392B;font-size:24px;font-weight:bold;");
    lay->addWidget(title);

    tbl = new QTableWidget(0,3,cw);
    tbl->setHorizontalHeaderLabels({"Medicine Name","Price/Unit (Rs.)","Stock"});
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    lay->addWidget(tbl,1);

    auto* row2 = new QHBoxLayout();
    auto* lbl  = new QLabel("Prescription ID:");
    lbl->setStyleSheet("color:#F0F0F0;");
    edtPrescId = new QLineEdit(prescriptionId);
    edtPrescId->setFixedHeight(36);
    btnProcess = new QPushButton("💊  PROCESS PRESCRIPTION");
    btnProcess->setFixedHeight(40);
    btnProcess->setCursor(Qt::PointingHandCursor);
    row2->addWidget(lbl);
    row2->addWidget(edtPrescId,1);
    row2->addWidget(btnProcess);
    lay->addLayout(row2);

    auto* btnBack = new QPushButton("← Back");
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:8px;font-weight:bold;");
    lay->addWidget(btnBack);

    connect(btnProcess, &QPushButton::clicked, this, &PharmacyScreen::onProcess);
    connect(btnBack,    &QPushButton::clicked, [this](){
        auto* w = new PatientActionsScreen(m_patientId,"",m_prescriptionId);
        w->show(); close();
    });

    loadMedicines();
}

void PharmacyScreen::loadMedicines() {
    auto meds = DatabaseManager::instance().getAllMedicines();
    tbl->setRowCount(0);
    for (const auto& m : meds) {
        int r = tbl->rowCount(); tbl->insertRow(r);
        tbl->setItem(r,0,new QTableWidgetItem(m.name));
        tbl->setItem(r,1,new QTableWidgetItem(QString::number(m.pricePerUnit,'f',0)));
        tbl->setItem(r,2,new QTableWidgetItem(QString::number(m.stock)));
    }
}

void PharmacyScreen::onProcess() {
    QString pid = edtPrescId->text().trimmed();
    if (pid.isEmpty()) { QMessageBox::warning(this,"Error","Enter prescription ID."); return; }

    auto& db = DatabaseManager::instance();
    PrescriptionData pd = db.getPrescription(pid);
    if (pd.prescriptionId.isEmpty()) {
        QMessageBox::warning(this,"Not Found","Prescription not found in database.");
        return;
    }

    QJsonArray medsArr = QJsonDocument::fromJson(pd.medicines.toUtf8()).array();
    if (medsArr.isEmpty()) {
        QMessageBox::information(this,"Info","No medicines in this prescription.");
        return;
    }

    double total = 0;
    QString receipt = "========== PHARMACY RECEIPT ==========\n";
    QStringList warnings;

    for (const auto& mv : medsArr) {
        QJsonObject mo = mv.toObject();
        QString name = mo["name"].toString();
        int     qty  = mo["qty"].toInt();

        // find price
        double price = 0;
        auto allMeds = db.getAllMedicines();
        for (auto& m : allMeds) if (m.name.compare(name,Qt::CaseInsensitive)==0) { price=m.pricePerUnit; break; }

        if (!db.updateMedicineStock(name, qty))
            warnings << "⚠️ Insufficient stock for: " + name;
        else {
            double cost = price * qty;
            total += cost;
            receipt += QString("%1  x%2  :  Rs. %3  (1 tab = Rs. %4)\n")
                       .arg(name,-18).arg(qty).arg(cost,0,'f',0).arg(price,0,'f',0);
        }
    }

    receipt += "--------------------------------------\n";
    receipt += QString("Medicine Total   :  Rs. %1\n").arg(total,0,'f',0);
    receipt += "======================================";

    for (const QString& w : warnings)
        QMessageBox::warning(this,"Stock Warning", w);

    db.updateMedicineCost(m_patientId, total);
    loadMedicines();

    // Show receipt dialog
    auto* dlg = new QDialog(this);
    dlg->setWindowTitle("Pharmacy Receipt");
    dlg->setMinimumSize(420,300);
    auto* dl = new QVBoxLayout(dlg);
    auto* te = new QTextEdit(receipt); te->setReadOnly(true);
    te->setStyleSheet("font-family:Courier;font-size:13px;background:#1a1a1a;color:#F0F0F0;");
    auto* ok = new QPushButton("OK"); ok->setFixedHeight(38);
    connect(ok,&QPushButton::clicked,dlg,&QDialog::accept);
    dl->addWidget(te); dl->addWidget(ok);
    dlg->exec();
}
