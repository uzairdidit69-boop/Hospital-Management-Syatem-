#include "DirectorLogin.h"
#include "DirectorDashboard.h"
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

DirectorLogin::DirectorLogin(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Director Login");
    setMinimumSize(800,520);
    resize(950,600);

    centralWidget = new QWidget(this); setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color:rgba(28,28,28,210);");

    auto* lay = new QVBoxLayout(centralWidget);
    lay->setAlignment(Qt::AlignCenter);
    lay->addStretch(2);

    auto* logo = new QLabel("🔐");
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet("font-size:56px;background:transparent;");
    lay->addWidget(logo);

    auto* title = new QLabel("DIRECTOR LOGIN");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#C0392B;font-size:26px;font-weight:bold;background:transparent;letter-spacing:2px;");
    lay->addWidget(title);

    auto* sub = new QLabel("Restricted Access — Authorized Personnel Only");
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color:#aaa;font-size:13px;background:transparent;");
    lay->addWidget(sub);
    lay->addSpacing(30);

    auto mkEdt = [](const QString& ph, bool pwd=false){
        auto* e = new QLineEdit(); e->setPlaceholderText(ph);
        e->setFixedSize(320,40);
        if (pwd) e->setEchoMode(QLineEdit::Password);
        return e;
    };
    edtUser = mkEdt("Username");
    edtPass = mkEdt("Password", true);

    auto* btnLogin = new QPushButton("🔐  LOGIN");
    btnLogin->setFixedSize(320,48); btnLogin->setCursor(Qt::PointingHandCursor);

    auto* btnBack = new QPushButton("← Back");
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

    connect(btnLogin, &QPushButton::clicked, this, &DirectorLogin::onLogin);
    connect(edtPass,  &QLineEdit::returnPressed, this, &DirectorLogin::onLogin);
    connect(btnBack,  &QPushButton::clicked, [this](){ auto* w=new MainWindow(); w->show(); close(); });

    applyBackground();
}

void DirectorLogin::onLogin() {
    QString u = edtUser->text().trimmed();
    QString p = edtPass->text().trimmed();
    if (u.isEmpty() || p.isEmpty()) { QMessageBox::warning(this,"Error","Enter username and password."); return; }
    DirectorData d = DatabaseManager::instance().authenticateDirector(u, p);
    if (!d.valid) {
        QMessageBox::warning(this,"Login Failed","❌ Incorrect username or password.");
        edtPass->clear();
        return;
    }
    auto* w = new DirectorDashboard(d); w->show(); close();
}

void DirectorLogin::applyBackground() {
    QPixmap bg(":/assets/bg_director.png");
    if (!bg.isNull()) {
        QPalette p; p.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(p); centralWidget->setAutoFillBackground(true);
    }
}

void DirectorLogin::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
