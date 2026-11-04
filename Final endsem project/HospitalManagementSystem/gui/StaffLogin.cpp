#include "StaffLogin.h"
#include "StaffPortal.h"
#include "MainWindow.h"
#include "../db/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QPalette>
#include <QPixmap>
#include <QResizeEvent>

StaffLogin::StaffLogin(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Staff Login");
    setMinimumSize(800,500);
    resize(950,580);

    centralWidget = new QWidget(this); setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color:rgba(28,28,28,210);");

    auto* lay = new QVBoxLayout(centralWidget);
    lay->setAlignment(Qt::AlignCenter);
    lay->addStretch(2);

    auto* logo = new QLabel("👤");
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet("font-size:56px;background:transparent;");
    lay->addWidget(logo);

    auto* title = new QLabel("STAFF LOGIN");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#C0392B;font-size:26px;font-weight:bold;background:transparent;letter-spacing:2px;");
    lay->addWidget(title);
    lay->addSpacing(30);

    edtUser = new QLineEdit(); edtUser->setPlaceholderText("Username");
    edtUser->setFixedSize(320,40);
    edtPass = new QLineEdit(); edtPass->setPlaceholderText("Password");
    edtPass->setFixedSize(320,40); edtPass->setEchoMode(QLineEdit::Password);

    auto* btnLogin = new QPushButton("🔐  LOGIN");
    btnLogin->setFixedSize(320,48); btnLogin->setCursor(Qt::PointingHandCursor);
    auto* btnBack  = new QPushButton("← Back");
    btnBack->setFixedSize(320,42);
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:8px;font-weight:bold;");

    auto row = [](QWidget* w){ auto* h=new QHBoxLayout(); h->addStretch(); h->addWidget(w); h->addStretch(); return h; };
    lay->addLayout(row(edtUser));
    lay->addSpacing(10);
    lay->addLayout(row(edtPass));
    lay->addSpacing(16);
    lay->addLayout(row(btnLogin));
    lay->addSpacing(8);
    lay->addLayout(row(btnBack));
    lay->addStretch(3);

    connect(btnLogin, &QPushButton::clicked, this, &StaffLogin::onLogin);
    connect(edtPass,  &QLineEdit::returnPressed, this, &StaffLogin::onLogin);
    connect(btnBack,  &QPushButton::clicked, [this](){ auto* w=new MainWindow(); w->show(); close(); });

    applyBackground();
}

void StaffLogin::onLogin() {
    QString u = edtUser->text().trimmed();
    QString p = edtPass->text().trimmed();
    if (u.isEmpty()||p.isEmpty()) { QMessageBox::warning(this,"Error","Enter credentials."); return; }
    StaffData s = DatabaseManager::instance().authenticateStaff(u,p);
    if (!s.valid) {
        QMessageBox::warning(this,"Login Failed","❌ Incorrect credentials.");
        edtPass->clear(); return;
    }
    auto* w = new StaffPortal(s); w->show(); close();
}

void StaffLogin::applyBackground() {
    QPixmap bg(":/assets/bg_staff.png");
    if (!bg.isNull()) {
        QPalette p; p.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(p); centralWidget->setAutoFillBackground(true);
    }
}

void StaffLogin::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
