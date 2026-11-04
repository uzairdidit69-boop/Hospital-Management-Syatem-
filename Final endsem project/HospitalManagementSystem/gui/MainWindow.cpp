#include "MainWindow.h"
#include "PatientPortal.h"
#include "DirectorLogin.h"
#include "StaffLogin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QPalette>
#include <QFont>
#include <QApplication>
#include <QScreen>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Hospital Management System");
    setMinimumSize(900, 650);
    resize(1100, 720);
    setupUI();
}

void MainWindow::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Overlay for dark tint over background
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color: rgba(28,28,28,180);");

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(0);

    // Top spacer
    mainLayout->addStretch(2);

    // Logo
    lblLogo = new QLabel(centralWidget);
    QPixmap logo(":/assets/logo.png");
    if (!logo.isNull())
        lblLogo->setPixmap(logo.scaled(120,120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        lblLogo->setText("🏥");
    lblLogo->setAlignment(Qt::AlignCenter);
    lblLogo->setStyleSheet("font-size: 60px; background: transparent;");
    mainLayout->addWidget(lblLogo);

    mainLayout->addSpacing(16);

    // Title
    lblTitle = new QLabel("HOSPITAL MANAGEMENT SYSTEM", centralWidget);
    lblTitle->setAlignment(Qt::AlignCenter);
    lblTitle->setStyleSheet("color: #C0392B; font-size: 30px; font-weight: bold;"
                            " font-family: 'Segoe UI'; letter-spacing: 2px; background: transparent;");
    mainLayout->addWidget(lblTitle);

    // Subtitle
    lblSubtitle = new QLabel("Noor Ul Huda Soomro Memorial Hospital", centralWidget);
    lblSubtitle->setAlignment(Qt::AlignCenter);
    lblSubtitle->setStyleSheet("color: #F0F0F0; font-size: 16px; font-family: 'Segoe UI';"
                               " letter-spacing: 1px; background: transparent;");
    mainLayout->addWidget(lblSubtitle);

    mainLayout->addSpacing(40);

    // Buttons
    auto makeBtn = [&](const QString& text) -> QPushButton* {
        auto* btn = new QPushButton(text, centralWidget);
        btn->setFixedSize(320, 56);
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };

    btnPatient  = makeBtn("🏥   PATIENT");
    btnDirector = makeBtn("🔐   DIRECTOR");
    btnStaff    = makeBtn("👤   STAFF");

    QHBoxLayout* r1 = new QHBoxLayout(); r1->addStretch(); r1->addWidget(btnPatient);  r1->addStretch();
    QHBoxLayout* r2 = new QHBoxLayout(); r2->addStretch(); r2->addWidget(btnDirector); r2->addStretch();
    QHBoxLayout* r3 = new QHBoxLayout(); r3->addStretch(); r3->addWidget(btnStaff);    r3->addStretch();
    mainLayout->addLayout(r1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(r2);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(r3);

    mainLayout->addStretch(3);

    // Version label
    lblVersion = new QLabel("HMS v1.0  —  Noor Ul Huda Soomro Memorial Hospital", centralWidget);
    lblVersion->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    lblVersion->setStyleSheet("color: #888; font-size: 11px; background: transparent; padding: 4px 10px;");
    mainLayout->addWidget(lblVersion);

    // Connections
    connect(btnPatient,  &QPushButton::clicked, [this](){
        PatientPortal* w = new PatientPortal();
        w->show();
        hide();
    });
    connect(btnDirector, &QPushButton::clicked, [this](){
        DirectorLogin* w = new DirectorLogin();
        w->show();
        hide();
    });
    connect(btnStaff,    &QPushButton::clicked, [this](){
        StaffLogin* w = new StaffLogin();
        w->show();
        hide();
    });

    applyBackground();
}

void MainWindow::applyBackground() {
    QPixmap bg(":/assets/bg_main.png");
    if (!bg.isNull()) {
        QPalette pal;
        pal.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(pal);
        centralWidget->setAutoFillBackground(true);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
