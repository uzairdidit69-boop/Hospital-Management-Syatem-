#include "StaffPortal.h"
#include "StaffLogin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QPalette>
#include <QPixmap>
#include <QResizeEvent>

StaffPortal::StaffPortal(const StaffData& staff, QWidget* parent)
    : QMainWindow(parent), m_staff(staff)
{
    setWindowTitle("Staff Portal");
    setMinimumSize(1050,650);
    resize(1200,720);

    centralWidget = new QWidget(this); setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color:rgba(28,28,28,190);");

    auto* mainLay = new QVBoxLayout(centralWidget);
    mainLay->setContentsMargins(20,15,20,15);
    mainLay->setSpacing(10);

    // Header
    auto* hdr = new QHBoxLayout();
    auto* wlc = new QLabel("👤  Welcome, " + staff.name +
                           "   |   Role: " + staff.role +
                           "   |   Shift: " + staff.shift);
    wlc->setStyleSheet("color:#C0392B;font-size:16px;font-weight:bold;background:transparent;");
    auto* btnLogout = new QPushButton("Logout");
    btnLogout->setFixedSize(100,34);
    btnLogout->setStyleSheet("background:#555;color:white;border-radius:6px;font-weight:bold;");
    hdr->addWidget(wlc); hdr->addStretch(); hdr->addWidget(btnLogout);
    mainLay->addLayout(hdr);

    // Splitter
    auto* splitter = new QSplitter(Qt::Horizontal);
    splitter->setStyleSheet("QSplitter::handle{background:#C0392B;width:2px;}");

    // Left — patient list
    auto* leftWid = new QGroupBox("Patient List");
    leftWid->setStyleSheet("QGroupBox{border:1px solid #C0392B;border-radius:6px;margin-top:10px;"
                           "color:#C0392B;font-weight:bold;background:rgba(28,28,28,140);}"
                           "QGroupBox::title{subcontrol-origin:margin;left:10px;padding:0 4px;}");
    auto* leftLay = new QVBoxLayout(leftWid);

    auto* btnRefresh = new QPushButton("🔄 Refresh");
    btnRefresh->setFixedHeight(34);
    leftLay->addWidget(btnRefresh);

    tblPatients = new QTableWidget(0,4,leftWid);
    tblPatients->setHorizontalHeaderLabels({"ID","Patient Name","Ward","Status"});
    tblPatients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblPatients->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblPatients->setEditTriggers(QAbstractItemView::NoEditTriggers);
    leftLay->addWidget(tblPatients,1);

    // Right — update status
    auto* rightWid = new QGroupBox("Update Patient Status");
    rightWid->setStyleSheet("QGroupBox{border:1px solid #C0392B;border-radius:6px;margin-top:10px;"
                            "color:#C0392B;font-weight:bold;background:rgba(28,28,28,140);}"
                            "QGroupBox::title{subcontrol-origin:margin;left:10px;padding:0 4px;}");
    auto* rightLay = new QVBoxLayout(rightWid);

    lblSelected = new QLabel("No patient selected");
    lblSelected->setStyleSheet("color:#FFD54F;font-size:13px;background:transparent;");
    rightLay->addWidget(lblSelected);
    rightLay->addSpacing(10);

    auto* lbl1 = new QLabel("New Status:"); lbl1->setStyleSheet("color:#F0F0F0;background:transparent;");
    rightLay->addWidget(lbl1);
    cmbStatus = new QComboBox();
    cmbStatus->addItems({"Admitted","In Treatment","Recovering","Pending Discharge"});
    cmbStatus->setFixedHeight(36);
    rightLay->addWidget(cmbStatus);
    rightLay->addSpacing(10);

    auto* lbl2 = new QLabel("Ward Assignment:"); lbl2->setStyleSheet("color:#F0F0F0;background:transparent;");
    rightLay->addWidget(lbl2);
    edtWard = new QLineEdit(); edtWard->setFixedHeight(36);
    edtWard->setPlaceholderText("e.g. General Ward");
    rightLay->addWidget(edtWard);
    rightLay->addSpacing(20);

    auto* btnUpdate = new QPushButton("✅  UPDATE STATUS");
    btnUpdate->setFixedHeight(46); btnUpdate->setCursor(Qt::PointingHandCursor);
    rightLay->addWidget(btnUpdate);
    rightLay->addStretch();

    splitter->addWidget(leftWid);
    splitter->addWidget(rightWid);
    splitter->setStretchFactor(0,3);
    splitter->setStretchFactor(1,2);
    mainLay->addWidget(splitter,1);

    auto* btnLogout2 = new QPushButton("← Logout");
    btnLogout2->setFixedHeight(40);
    btnLogout2->setStyleSheet("background:#555;color:white;border-radius:6px;font-weight:bold;");
    mainLay->addWidget(btnLogout2);

    connect(btnRefresh, &QPushButton::clicked, this, &StaffPortal::loadPatients);
    connect(tblPatients,&QTableWidget::itemSelectionChanged, this, &StaffPortal::onRowSelected);
    connect(btnUpdate,  &QPushButton::clicked, this, &StaffPortal::onUpdateStatus);
    connect(btnLogout,  &QPushButton::clicked, [this](){ auto* w=new StaffLogin(); w->show(); close(); });
    connect(btnLogout2, &QPushButton::clicked, [this](){ auto* w=new StaffLogin(); w->show(); close(); });

    loadPatients();
    applyBackground();
}

void StaffPortal::loadPatients() {
    auto statuses = DatabaseManager::instance().getAllPatientStatuses();
    tblPatients->setRowCount(0);
    for (const auto& ps : statuses) {
        int r = tblPatients->rowCount(); tblPatients->insertRow(r);
        tblPatients->setItem(r,0,new QTableWidgetItem(QString::number(ps.patientId)));
        tblPatients->setItem(r,1,new QTableWidgetItem(ps.patientName));
        tblPatients->setItem(r,2,new QTableWidgetItem(ps.assignedWard));
        tblPatients->setItem(r,3,new QTableWidgetItem(ps.status));
    }
}

void StaffPortal::onRowSelected() {
    int row = tblPatients->currentRow();
    if (row < 0) return;
    m_selectedPatientId = tblPatients->item(row,0)->text().toInt();
    QString name  = tblPatients->item(row,1)->text();
    QString ward  = tblPatients->item(row,2)->text();
    QString stat  = tblPatients->item(row,3)->text();
    lblSelected->setText("Selected: " + name + "  (ID: " + QString::number(m_selectedPatientId) + ")");
    edtWard->setText(ward);
    int idx = cmbStatus->findText(stat);
    if (idx>=0) cmbStatus->setCurrentIndex(idx);
}

void StaffPortal::onUpdateStatus() {
    if (m_selectedPatientId < 0) {
        QMessageBox::warning(this,"Error","Please select a patient first."); return;
    }
    QString status = cmbStatus->currentText();
    QString ward   = edtWard->text().trimmed();
    bool ok = DatabaseManager::instance().updatePatientStatus(m_selectedPatientId, status, ward);
    if (ok) {
        QMessageBox::information(this,"Updated","✅ Patient status updated successfully.");
        loadPatients();
    } else {
        QMessageBox::critical(this,"Error","Failed to update patient status.");
    }
}

void StaffPortal::applyBackground() {
    QPixmap bg(":/assets/bg_staff.png");
    if (!bg.isNull()) {
        QPalette p; p.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(p); centralWidget->setAutoFillBackground(true);
    }
}

void StaffPortal::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
