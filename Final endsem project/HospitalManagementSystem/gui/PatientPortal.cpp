#include "PatientPortal.h"
#include "PatientRegister.h"
#include "PatientLogin.h"
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPalette>
#include <QPixmap>
#include <QResizeEvent>

PatientPortal::PatientPortal(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Patient Portal");
    setMinimumSize(900,650);
    resize(1100,720);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color: rgba(28,28,28,180);");

    QVBoxLayout* lay = new QVBoxLayout(centralWidget);
    lay->setAlignment(Qt::AlignCenter);
    lay->addStretch(2);

    auto* lbl = new QLabel("🏥  PATIENT PORTAL", centralWidget);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("color:#C0392B;font-size:28px;font-weight:bold;background:transparent;letter-spacing:2px;");
    lay->addWidget(lbl);
    lay->addSpacing(10);

    auto* sub = new QLabel("Welcome — Please select an option below", centralWidget);
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color:#F0F0F0;font-size:14px;background:transparent;");
    lay->addWidget(sub);
    lay->addSpacing(40);

    auto makeBtn = [&](const QString& t) {
        auto* b = new QPushButton(t, centralWidget);
        b->setFixedSize(380,56); b->setCursor(Qt::PointingHandCursor);
        return b;
    };
    auto* btnReg   = makeBtn("📋   NEW PATIENT — REGISTER");
    auto* btnLogin = makeBtn("🔑   RETURNING PATIENT — LOGIN");
    auto* btnBack  = makeBtn("← Back");
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:10px;font-weight:bold;");

    auto row = [](QWidget* w) {
        auto* h = new QHBoxLayout(); h->addStretch(); h->addWidget(w); h->addStretch();
        return h;
    };
    lay->addLayout(row(btnReg));
    lay->addSpacing(14);
    lay->addLayout(row(btnLogin));
    lay->addStretch(3);
    lay->addLayout(row(btnBack));
    lay->addSpacing(10);

    connect(btnReg,   &QPushButton::clicked, [this](){
        auto* w = new PatientRegister(); w->show(); hide();
    });
    connect(btnLogin, &QPushButton::clicked, [this](){
        auto* w = new PatientLogin(); w->show(); hide();
    });
    connect(btnBack,  &QPushButton::clicked, [this](){
        auto* w = new MainWindow(); w->show(); close();
    });

    applyBackground();
}

void PatientPortal::applyBackground() {
    QPixmap bg(":/assets/bg_patient.png");
    if (!bg.isNull()) {
        QPalette p; p.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(p); centralWidget->setAutoFillBackground(true);
    }
}

void PatientPortal::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
