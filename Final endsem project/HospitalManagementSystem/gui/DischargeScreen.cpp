#include "DischargeScreen.h"
#include "MainWindow.h"
#include "PatientActionsScreen.h"
#include "../db/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QPalette>
#include <QPixmap>
#include <QResizeEvent>

DischargeScreen::DischargeScreen(int patientId, const QString& patientName,
                                 const QString& prescriptionId, QWidget* parent)
    : QMainWindow(parent), m_patientId(patientId),
      m_patientName(patientName), m_prescriptionId(prescriptionId)
{
    setWindowTitle("Discharge & Billing");
    setMinimumSize(860,600);
    resize(1000,680);

    centralWidget = new QWidget(this); setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color:rgba(28,28,28,205);");

    auto* lay = new QVBoxLayout(centralWidget);
    lay->setContentsMargins(40,20,40,20); lay->setSpacing(14);

    auto* title = new QLabel("🏠  DISCHARGE & BILLING");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#C0392B;font-size:24px;font-weight:bold;background:transparent;");
    lay->addWidget(title);

    auto* info = new QLabel("Patient: " + patientName + "   |   ID: " + QString::number(patientId));
    info->setAlignment(Qt::AlignCenter);
    info->setStyleSheet("color:#F0F0F0;font-size:13px;background:transparent;");
    lay->addWidget(info);

    auto* grp = new QGroupBox("Bill Summary");
    grp->setStyleSheet("QGroupBox{border:1px solid #C0392B;border-radius:8px;margin-top:12px;"
                       "color:#C0392B;font-weight:bold;background:rgba(28,28,28,160);}"
                       "QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 4px;}");
    auto* gl = new QVBoxLayout(grp);
    lblBill = new QLabel("Loading...");
    lblBill->setStyleSheet("color:#F0F0F0;font-family:Courier;font-size:13px;background:transparent;");
    lblBill->setAlignment(Qt::AlignLeft);
    gl->addWidget(lblBill);
    lay->addWidget(grp,1);

    auto* payRow = new QHBoxLayout();
    auto* lbl = new QLabel("Enter Amount (Rs.):");
    lbl->setStyleSheet("color:#F0F0F0;font-size:14px;background:transparent;");
    edtAmount = new QLineEdit(); edtAmount->setFixedHeight(38);
    edtAmount->setPlaceholderText("Enter payment amount");
    auto* btnPay = new QPushButton("💳  PAY");
    btnPay->setFixedSize(160,44); btnPay->setCursor(Qt::PointingHandCursor);
    payRow->addWidget(lbl); payRow->addWidget(edtAmount,1); payRow->addWidget(btnPay);
    lay->addLayout(payRow);

    auto* btnBack = new QPushButton("← Back");
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:8px;font-weight:bold;");
    lay->addWidget(btnBack);

    connect(btnPay,  &QPushButton::clicked, this, &DischargeScreen::onPay);
    connect(btnBack, &QPushButton::clicked, [this](){
        auto* w = new PatientActionsScreen(m_patientId, m_patientName, m_prescriptionId);
        w->show(); close();
    });

    loadBill();
    applyBackground();
}

void DischargeScreen::loadBill() {
    auto& db = DatabaseManager::instance();
    BillingData b = db.getBilling(m_patientId);
    PrescriptionData pd = db.getPrescription(m_prescriptionId);
    m_totalBill = b.totalCost;

    QString txt;
    txt += "============== MEDICINE ===============\n";
    txt += QString("Medicine Cost    :  Rs. %1\n").arg(b.medicineCost,0,'f',0);
    txt += "============= LAB TESTS ===============\n";
    txt += QString("Lab Test Cost    :  Rs. %1\n").arg(b.labtestCost,0,'f',0);
    txt += "============= OPERATION ===============\n";
    txt += QString("Operation Cost   :  Rs. %1\n").arg(b.operationCost,0,'f',0);
    txt += "============= TOTAL BILL ==============\n";
    txt += QString("TOTAL AMOUNT     :  Rs. %1").arg(b.totalCost,0,'f',0);
    lblBill->setText(txt);
}

void DischargeScreen::onPay() {
    bool ok;
    double entered = edtAmount->text().trimmed().toDouble(&ok);
    if (!ok || entered < 0) {
        QMessageBox::warning(this,"Error","Please enter a valid payment amount.");
        return;
    }

    auto& db = DatabaseManager::instance();
    BillingData b = db.getBilling(m_patientId);
    double total  = b.totalCost;

    if (entered < total) {
        QMessageBox::warning(this,"Insufficient Funds",
            QString("❌ Insufficient funds.\nRequired: Rs. %1\nEntered: Rs. %2\nPlease try again.")
            .arg(total,0,'f',0).arg(entered,0,'f',0));
        return;
    }

    double change = entered - total;
    db.dischargePatient(m_patientId, total, entered);

    QString msg;
    if (change > 0)
        msg = QString("✅ Bill Paid!\nYour change is Rs. %1\n\nThanks for visiting Hospital Management System! 🏥❤️").arg(change,0,'f',0);
    else
        msg = "✅ Bill Paid!\n\nThanks for visiting Hospital Management System! 🏥❤️";

    QMessageBox::information(this,"Payment Successful", msg);
    auto* w = new MainWindow(); w->show(); close();
}

void DischargeScreen::applyBackground() {
    QPixmap bg(":/assets/bg_patient.png");
    if (!bg.isNull()) {
        QPalette p; p.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(p); centralWidget->setAutoFillBackground(true);
    }
}

void DischargeScreen::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
