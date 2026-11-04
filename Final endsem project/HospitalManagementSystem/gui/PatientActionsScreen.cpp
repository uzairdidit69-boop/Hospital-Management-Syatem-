#include "PatientActionsScreen.h"
#include "PharmacyScreen.h"
#include "LabTestScreen.h"
#include "OperationScreen.h"
#include "DischargeScreen.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QPalette>
#include <QPixmap>
#include <QResizeEvent>

PatientActionsScreen::PatientActionsScreen(int patientId, const QString& patientName,
                                           const QString& prescriptionId, QWidget* parent)
    : QMainWindow(parent), m_patientId(patientId),
      m_patientName(patientName), m_prescriptionId(prescriptionId)
{
    setWindowTitle("Patient Actions");
    setMinimumSize(900,600);
    resize(1100,700);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color:rgba(28,28,28,185);");

    auto* lay = new QVBoxLayout(centralWidget);
    lay->setContentsMargins(50,30,50,30);
    lay->setSpacing(16);
    lay->addStretch(1);

    auto* title = new QLabel("⚕️  PATIENT ACTIONS");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#C0392B;font-size:26px;font-weight:bold;background:transparent;");
    lay->addWidget(title);

    auto* info = new QLabel("Patient: " + patientName + "   |   ID: " + QString::number(patientId));
    info->setAlignment(Qt::AlignCenter);
    info->setStyleSheet("color:#F0F0F0;font-size:14px;background:transparent;");
    lay->addWidget(info);

    auto* prescLbl = new QLabel("Your Prescription ID: " + prescriptionId);
    prescLbl->setAlignment(Qt::AlignCenter);
    prescLbl->setStyleSheet("color:#FFD54F;font-size:15px;font-weight:bold;background:transparent;");
    lay->addWidget(prescLbl);
    lay->addSpacing(20);

    auto makeBtn = [&](const QString& t) {
        auto* b = new QPushButton(t, centralWidget);
        b->setFixedSize(280,80);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet("QPushButton{background:#C0392B;color:white;border-radius:10px;"
                         "font-size:16px;font-weight:bold;}"
                         "QPushButton:hover{background:#E74C3C;}");
        return b;
    };

    auto* btnPharm  = makeBtn("💊\nPHARMACY");
    auto* btnLab    = makeBtn("🧪\nLAB TESTS");
    auto* btnOp     = makeBtn("🔪\nOPERATION");
    auto* btnDisch  = makeBtn("🏠\nDISCHARGE");

    auto* grid = new QGridLayout();
    grid->setSpacing(20);
    grid->addWidget(btnPharm, 0,0);
    grid->addWidget(btnLab,   0,1);
    grid->addWidget(btnOp,    1,0);
    grid->addWidget(btnDisch, 1,1);

    auto* gridHolder = new QHBoxLayout();
    gridHolder->addStretch(); gridHolder->addLayout(grid); gridHolder->addStretch();
    lay->addLayout(gridHolder);
    lay->addStretch(2);

    connect(btnPharm, &QPushButton::clicked, [this](){ auto* w=new PharmacyScreen(m_patientId,m_prescriptionId); w->show(); close(); });
    connect(btnLab,   &QPushButton::clicked, [this](){ auto* w=new LabTestScreen(m_patientId,m_prescriptionId); w->show(); close(); });
    connect(btnOp,    &QPushButton::clicked, [this](){ auto* w=new OperationScreen(m_patientId,m_prescriptionId); w->show(); close(); });
    connect(btnDisch, &QPushButton::clicked, [this](){ auto* w=new DischargeScreen(m_patientId,m_patientName,m_prescriptionId); w->show(); close(); });

    applyBackground();
}

void PatientActionsScreen::applyBackground() {
    QPixmap bg(":/assets/bg_patient.png");
    if (!bg.isNull()) {
        QPalette p; p.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(p); centralWidget->setAutoFillBackground(true);
    }
}

void PatientActionsScreen::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
