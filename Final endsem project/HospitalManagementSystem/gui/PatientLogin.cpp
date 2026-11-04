#include "PatientLogin.h"
#include "PatientPortal.h"
#include "OPDScreen.h"
#include "../db/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QPalette>
#include <QPixmap>

PatientLogin::PatientLogin(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Patient Login");
    setMinimumSize(700,450);
    resize(800,500);

    auto* cw = new QWidget(this);
    setCentralWidget(cw);

    QPixmap bg(":/assets/bg_patient.png");
    if (!bg.isNull()) {
        QPalette p; p.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        cw->setPalette(p); cw->setAutoFillBackground(true);
    }

    auto* ov = new QWidget(cw);
    ov->setStyleSheet("background-color:rgba(28,28,28,200);");
    ov->resize(size());

    auto* lay = new QVBoxLayout(cw);
    lay->setAlignment(Qt::AlignCenter);
    lay->addStretch(2);

    auto* title = new QLabel("🔑  PATIENT LOGIN");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#C0392B;font-size:26px;font-weight:bold;background:transparent;");
    lay->addWidget(title);

    auto* sub = new QLabel("Enter your Patient ID to continue");
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color:#F0F0F0;font-size:14px;background:transparent;");
    lay->addWidget(sub);
    lay->addSpacing(30);

    edtId = new QLineEdit();
    edtId->setPlaceholderText("Enter Patient ID (e.g. 42)");
    edtId->setFixedSize(300,40);
    edtId->setAlignment(Qt::AlignCenter);

    btnLogin = new QPushButton("🔐  LOGIN");
    btnLogin->setFixedSize(300,48);
    btnLogin->setCursor(Qt::PointingHandCursor);

    auto* btnBack = new QPushButton("← Back");
    btnBack->setFixedSize(300,42);
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:8px;font-weight:bold;");

    auto row = [](QWidget* w){ auto* h=new QHBoxLayout(); h->addStretch(); h->addWidget(w); h->addStretch(); return h; };
    lay->addLayout(row(edtId));
    lay->addSpacing(12);
    lay->addLayout(row(btnLogin));
    lay->addSpacing(8);
    lay->addLayout(row(btnBack));
    lay->addStretch(3);

    connect(btnLogin, &QPushButton::clicked, this, &PatientLogin::onLogin);
    connect(edtId,    &QLineEdit::returnPressed, this, &PatientLogin::onLogin);
    connect(btnBack,  &QPushButton::clicked, [this](){
        auto* w = new PatientPortal(); w->show(); close();
    });
}

void PatientLogin::onLogin() {
    bool ok;
    int pid = edtId->text().trimmed().toInt(&ok);
    if (!ok || pid <= 0) {
        QMessageBox::warning(this,"Error","Please enter a valid numeric Patient ID.");
        return;
    }
    auto& db = DatabaseManager::instance();
    if (!db.patientExists(pid)) {
        QMessageBox::warning(this,"Not Found","Patient ID not found. Please register first.");
        return;
    }
    PatientData p = db.getPatient(pid);
    auto* w = new OPDScreen(pid, p.name); w->show(); close();
}
