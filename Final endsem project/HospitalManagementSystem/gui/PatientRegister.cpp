#include "PatientRegister.h"
#include "PatientPortal.h"
#include "OPDScreen.h"
#include "../db/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QPalette>
#include <QPixmap>

PatientRegister::PatientRegister(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Patient Registration");
    setMinimumSize(900,700);
    resize(1100,750);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color: rgba(28,28,28,210);");

    auto* scroll = new QScrollArea(centralWidget);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background:transparent;border:none;");

    auto* form = new QWidget();
    form->setStyleSheet("background:transparent;");
    auto* formLay = new QVBoxLayout(form);
    formLay->setContentsMargins(60,30,60,30);
    formLay->setSpacing(14);

    auto* title = new QLabel("📋  PATIENT REGISTRATION");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#C0392B;font-size:24px;font-weight:bold;background:transparent;");
    formLay->addWidget(title);
    formLay->addSpacing(10);

    auto* grp = new QGroupBox("Personal Information");
    grp->setStyleSheet("QGroupBox{border:1px solid #C0392B;border-radius:8px;margin-top:12px;"
                       "color:#C0392B;font-weight:bold;font-size:14px;background:rgba(28,28,28,160);}"
                       "QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 4px;}");
    auto* fl = new QFormLayout(grp);
    fl->setSpacing(10);
    fl->setContentsMargins(20,20,20,20);

    auto lbl = [](const QString& t){ auto* l=new QLabel(t); l->setStyleSheet("color:#F0F0F0;background:transparent;"); return l; };
    auto edt = [](){ auto* e=new QLineEdit(); e->setFixedHeight(36); return e; };

    edtName     = edt(); fl->addRow(lbl("Full Name *"),     edtName);
    edtCnic     = edt(); edtCnic->setPlaceholderText("XXXXX-XXXXXXX-X");
                         fl->addRow(lbl("CNIC *"),           edtCnic);
    edtGuardian = edt(); fl->addRow(lbl("Guardian Name *"),  edtGuardian);
    edtAddress  = new QTextEdit(); edtAddress->setFixedHeight(70);
                         fl->addRow(lbl("Address *"),         edtAddress);
    edtPhone    = edt(); edtPhone->setPlaceholderText("03XXXXXXXXX");
                         fl->addRow(lbl("Phone Number *"),    edtPhone);
    spnAge      = new QSpinBox(); spnAge->setRange(0,120); spnAge->setFixedHeight(36);
                         fl->addRow(lbl("Age *"),             spnAge);
    cmbGender   = new QComboBox(); cmbGender->addItems({"Male","Female","Other"});
    cmbGender->setFixedHeight(36); fl->addRow(lbl("Gender *"), cmbGender);
    cmbBlood    = new QComboBox(); cmbBlood->addItems({"A+","A-","B+","B-","AB+","AB-","O+","O-"});
    cmbBlood->setFixedHeight(36);  fl->addRow(lbl("Blood Group *"), cmbBlood);

    formLay->addWidget(grp);
    formLay->addSpacing(10);

    btnSubmit = new QPushButton("✅  REGISTER PATIENT");
    btnSubmit->setFixedHeight(50);
    btnSubmit->setCursor(Qt::PointingHandCursor);
    formLay->addWidget(btnSubmit);

    auto* btnBack = new QPushButton("← Back");
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:10px;font-weight:bold;");
    btnBack->setFixedHeight(42);
    formLay->addWidget(btnBack);

    scroll->setWidget(form);

    auto* mainLay = new QVBoxLayout(centralWidget);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->addWidget(scroll);

    connect(btnSubmit, &QPushButton::clicked, this, &PatientRegister::onSubmit);
    connect(btnBack,   &QPushButton::clicked, [this](){
        new PatientPortal(); static_cast<PatientPortal*>(sender()->parent());
        auto* w = new PatientPortal(); w->show(); close();
    });

    applyBackground();
}

void PatientRegister::onSubmit() {
    // Validate
    if (edtName->text().trimmed().isEmpty() ||
        edtCnic->text().trimmed().isEmpty() ||
        edtGuardian->text().trimmed().isEmpty() ||
        edtAddress->toPlainText().trimmed().isEmpty() ||
        edtPhone->text().trimmed().isEmpty()) {
        QMessageBox::warning(this,"Validation","Please fill in all required fields.");
        return;
    }
    QRegularExpression cnicRx("^\\d{5}-\\d{7}-\\d$");
    if (!cnicRx.match(edtCnic->text().trimmed()).hasMatch()) {
        QMessageBox::warning(this,"Validation","CNIC format must be XXXXX-XXXXXXX-X");
        return;
    }

    PatientData p;
    p.name       = edtName->text().trimmed();
    p.cnic       = edtCnic->text().trimmed();
    p.guardian   = edtGuardian->text().trimmed();
    p.address    = edtAddress->toPlainText().trimmed();
    p.phone      = edtPhone->text().trimmed();
    p.age        = spnAge->value();
    p.gender     = cmbGender->currentText();
    p.bloodGroup = cmbBlood->currentText();

    auto& db = DatabaseManager::instance();
    int pid = db.insertPatient(p);
    if (pid < 0) {
        QMessageBox::critical(this,"Error","Registration failed. CNIC may already exist.");
        return;
    }
    db.initBilling(pid);
    db.insertPatientStatus(pid);

    QMessageBox::information(this,"Registration Successful",
        QString("✅ Registration Successful!\nYour Patient ID: %1\nPlease remember this ID for future visits.")
        .arg(pid, 4, 10, QChar('0')));

    auto* w = new OPDScreen(pid, p.name); w->show(); close();
}

void PatientRegister::applyBackground() {
    QPixmap bg(":/assets/bg_patient.png");
    if (!bg.isNull()) {
        QPalette pal;
        pal.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(pal); centralWidget->setAutoFillBackground(true);
    }
}

void PatientRegister::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
