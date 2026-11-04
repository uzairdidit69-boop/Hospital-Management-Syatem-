#include "OPDScreen.h"
#include "DoctorChatScreen.h"
#include "PatientPortal.h"
#include "../db/DatabaseManager.h"
#include <algorithm>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QPalette>
#include <QPixmap>
#include <QResizeEvent>

OPDScreen::OPDScreen(int patientId, const QString& patientName, QWidget* parent)
    : QMainWindow(parent), m_patientId(patientId), m_patientName(patientName)
{
    setWindowTitle("OPD — Select Doctor");
    setMinimumSize(950,650);
    resize(1100,720);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color:rgba(28,28,28,190);");

    auto* lay = new QVBoxLayout(centralWidget);
    lay->setContentsMargins(40,30,40,20);
    lay->setSpacing(12);

    auto* title = new QLabel("🏥  OUTPATIENT DEPARTMENT (OPD)");
    title->setStyleSheet("color:#C0392B;font-size:24px;font-weight:bold;background:transparent;");
    title->setAlignment(Qt::AlignCenter);
    lay->addWidget(title);

    auto* welcome = new QLabel("Welcome, " + patientName + "  |  Patient ID: " + QString::number(patientId));
    welcome->setStyleSheet("color:#F0F0F0;font-size:14px;background:transparent;");
    welcome->setAlignment(Qt::AlignCenter);
    lay->addWidget(welcome);

    auto* instr = new QLabel("Select a doctor from the list below and click CONSULT");
    instr->setStyleSheet("color:#aaa;font-size:13px;background:transparent;");
    instr->setAlignment(Qt::AlignCenter);
    lay->addWidget(instr);
    lay->addSpacing(10);

    tblDoctors = new QTableWidget(0,5,centralWidget);
    tblDoctors->setHorizontalHeaderLabels({"ID","Name","Department","Specialty","Status"});
    tblDoctors->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblDoctors->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblDoctors->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblDoctors->setAlternatingRowColors(true);
    tblDoctors->setStyleSheet("QTableWidget{alternate-background-color:#252525;}");
    lay->addWidget(tblDoctors, 1);

    btnConsult = new QPushButton("🩺  CONSULT SELECTED DOCTOR");
    btnConsult->setFixedHeight(50);
    btnConsult->setEnabled(false);
    btnConsult->setCursor(Qt::PointingHandCursor);

    auto* btnBack = new QPushButton("← Back");
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:10px;font-weight:bold;");
    btnBack->setFixedHeight(44);

    auto* botRow = new QHBoxLayout();
    botRow->addWidget(btnBack);
    botRow->addStretch();
    botRow->addWidget(btnConsult);
    lay->addLayout(botRow);

    connect(tblDoctors, &QTableWidget::itemSelectionChanged, [this](){
        btnConsult->setEnabled(!tblDoctors->selectedItems().isEmpty());
    });
    connect(btnConsult, &QPushButton::clicked, this, &OPDScreen::onConsult);
    connect(btnBack,    &QPushButton::clicked, [this](){
        auto* w = new PatientPortal(); w->show(); close();
    });

    loadDoctors();
    applyBackground();
}

void OPDScreen::loadDoctors() {
    auto doctors = DatabaseManager::instance().getAllDoctors();
    tblDoctors->setRowCount(0);
    // Sort by doctorId ascending so table always shows IDs in order
    std::sort(doctors.begin(), doctors.end(),
              [](const DoctorData& a, const DoctorData& b){ return a.doctorId < b.doctorId; });
    for (const auto& d : doctors) {
        if (!d.available) continue;
        int row = tblDoctors->rowCount();
        tblDoctors->insertRow(row);
        tblDoctors->setItem(row,0,new QTableWidgetItem(QString::number(d.doctorId)));
        tblDoctors->setItem(row,1,new QTableWidgetItem(d.name));
        tblDoctors->setItem(row,2,new QTableWidgetItem(d.department));
        tblDoctors->setItem(row,3,new QTableWidgetItem(d.specialty));
        tblDoctors->setItem(row,4,new QTableWidgetItem("Available"));
    }
}

void OPDScreen::onConsult() {
    int row = tblDoctors->currentRow();
    if (row < 0) return;
    int     doctorId   = tblDoctors->item(row,0)->text().toInt();
    QString doctorName = tblDoctors->item(row,1)->text();
    QString dept       = tblDoctors->item(row,2)->text();
    QString prescId    = QString("P%1D%2")
                         .arg(m_patientId,4,10,QChar('0'))
                         .arg(doctorId,   4,10,QChar('0'));

    // Fetch patient age & gender for richer AI system prompt
    PatientData pd = DatabaseManager::instance().getPatient(m_patientId);

    auto* w = new DoctorChatScreen(m_patientId, m_patientName,
                                   doctorId, doctorName, dept, prescId,
                                   pd.age, pd.gender);
    w->show(); close();
}

void OPDScreen::applyBackground() {
    QPixmap bg(":/assets/bg_patient.png");
    if (!bg.isNull()) {
        QPalette p; p.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(p); centralWidget->setAutoFillBackground(true);
    }
}

void OPDScreen::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
