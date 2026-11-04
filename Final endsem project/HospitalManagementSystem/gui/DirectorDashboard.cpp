#include "DirectorDashboard.h"
#include "DirectorLogin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QPalette>
#include <QPixmap>
#include <QSqlQuery>
#include <QDate>
#include <QResizeEvent>

static auto cardStyle = [](const QString& color){
    return QString("QGroupBox{border:2px solid %1;border-radius:10px;background:rgba(28,28,28,180);}"
                   "QGroupBox::title{color:%1;font-size:12px;font-weight:bold;}").arg(color);
};

DirectorDashboard::DirectorDashboard(const DirectorData& director, QWidget* parent)
    : QMainWindow(parent), m_director(director)
{
    setWindowTitle("Director Dashboard");
    setMinimumSize(1100,700);
    resize(1280,800);

    centralWidget = new QWidget(this); setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color:rgba(28,28,28,195);");

    auto* mainLay = new QVBoxLayout(centralWidget);
    mainLay->setContentsMargins(20,15,20,15);
    mainLay->setSpacing(10);

    // Header
    auto* hdr = new QHBoxLayout();
    auto* wlc = new QLabel("👑  Welcome, " + director.name);
    wlc->setStyleSheet("color:#C0392B;font-size:20px;font-weight:bold;background:transparent;");
    auto* btnLogout = new QPushButton("Logout");
    btnLogout->setFixedSize(100,36);
    btnLogout->setStyleSheet("background:#555;color:white;border-radius:6px;font-weight:bold;");
    hdr->addWidget(wlc); hdr->addStretch(); hdr->addWidget(btnLogout);
    mainLay->addLayout(hdr);

    // Dashboard cards
    auto* cardRow = new QHBoxLayout();
    cardRow->setSpacing(12);

    auto mkCard = [&](const QString& title, QLabel*& lbl, const QString& color) {
        auto* grp = new QGroupBox(title);
        grp->setStyleSheet(cardStyle(color));
        grp->setFixedHeight(90);
        auto* l = new QVBoxLayout(grp);
        lbl = new QLabel("--"); lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet(QString("color:%1;font-size:22px;font-weight:bold;background:transparent;").arg(color));
        l->addWidget(lbl);
        cardRow->addWidget(grp);
    };
    mkCard("💰 Total Revenue",     lblRevenue,   "#F1C40F");
    mkCard("👥 Active Patients",   lblPatients,  "#2ECC71");
    mkCard("✅ Discharged Today",  lblDischarged,"#3498DB");
    mkCard("⚠️ Low Stock Items",  lblStockAlert,"#E74C3C");
    mainLay->addLayout(cardRow);

    // Tabs
    auto* tabs = new QTabWidget();
    tabs->setStyleSheet("QTabBar::tab{background:#2C2C2C;color:#F0F0F0;padding:8px 16px;border-radius:4px;}"
                        "QTabBar::tab:selected{background:#C0392B;color:white;font-weight:bold;}"
                        "QTabWidget::pane{border:1px solid #C0392B;}");
    mainLay->addWidget(tabs,1);

    // Tab: Transactions
    auto* tabDash = new QWidget();
    setupDashboardTab(tabDash);
    tabs->addTab(tabDash,"📊 Transactions");

    auto* tabPharm = new QWidget();
    setupPharmacyTab(tabPharm);
    tabs->addTab(tabPharm,"💊 Pharmacy");

    auto* tabDoc = new QWidget();
    setupDoctorTab(tabDoc);
    tabs->addTab(tabDoc,"👨‍⚕️ Doctors");

    auto* tabLab = new QWidget();
    setupLabTab(tabLab);
    tabs->addTab(tabLab,"🧪 Lab Tests");

    auto* tabOp = new QWidget();
    setupOpTab(tabOp);
    tabs->addTab(tabOp,"🔪 Operations");

    connect(btnLogout, &QPushButton::clicked, [this](){ auto* w=new DirectorLogin(); w->show(); close(); });

    refreshDashboard();
    refreshPharmacy();
    refreshDoctors();
    refreshLabs();
    refreshOps();
    applyBackground();
}

void DirectorDashboard::setupDashboardTab(QWidget* p) {
    auto* lay = new QVBoxLayout(p);
    tblTxn = new QTableWidget(0,3,p);
    tblTxn->setHorizontalHeaderLabels({"Patient Name","Amount (Rs.)","Date & Time"});
    tblTxn->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblTxn->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lay->addWidget(tblTxn);
}

void DirectorDashboard::setupPharmacyTab(QWidget* p) {
    auto* lay = new QVBoxLayout(p);
    auto* btnAdd = new QPushButton("➕ ADD NEW MEDICINE");
    btnAdd->setFixedHeight(38); lay->addWidget(btnAdd);
    tblPharm = new QTableWidget(0,5,p);
    tblPharm->setHorizontalHeaderLabels({"ID","Name","Price/Unit","Stock","Actions"});
    tblPharm->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblPharm->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lay->addWidget(tblPharm,1);
    connect(btnAdd, &QPushButton::clicked, [this](){
        auto* dlg = new QDialog(this); dlg->setWindowTitle("Add Medicine"); dlg->setMinimumWidth(380);
        auto* fl = new QFormLayout(dlg);
        auto* eName = new QLineEdit(); auto* ePrice = new QDoubleSpinBox();
        auto* eStock = new QSpinBox(); ePrice->setRange(0,999999); ePrice->setDecimals(0);
        eStock->setRange(0,99999);
        fl->addRow("Name:",  eName); fl->addRow("Price:", ePrice); fl->addRow("Stock:", eStock);
        auto* ok = new QPushButton("Add"); fl->addRow(ok);
        connect(ok, &QPushButton::clicked, [=](){
            if (eName->text().trimmed().isEmpty()) return;
            MedicineData m; m.name=eName->text().trimmed(); m.pricePerUnit=ePrice->value(); m.stock=eStock->value();
            DatabaseManager::instance().addMedicine(m);
            refreshPharmacy(); dlg->accept();
        });
        dlg->exec();
    });
}

void DirectorDashboard::setupDoctorTab(QWidget* p) {
    auto* lay = new QVBoxLayout(p);
    auto* btnAdd = new QPushButton("➕ ADD NEW DOCTOR");
    btnAdd->setFixedHeight(38); lay->addWidget(btnAdd);
    tblDoctors = new QTableWidget(0,6,p);
    tblDoctors->setHorizontalHeaderLabels({"ID","Name","Department","Specialty","Available","Actions"});
    tblDoctors->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblDoctors->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lay->addWidget(tblDoctors,1);
    connect(btnAdd, &QPushButton::clicked, [this](){
        auto* dlg = new QDialog(this); dlg->setWindowTitle("Add Doctor"); dlg->setMinimumWidth(400);
        auto* fl  = new QFormLayout(dlg);
        auto* eName=new QLineEdit(); auto* eDept=new QLineEdit(); auto* eSpec=new QLineEdit();
        fl->addRow("Name:",eName); fl->addRow("Department:",eDept); fl->addRow("Specialty:",eSpec);
        auto* ok=new QPushButton("Add"); fl->addRow(ok);
        connect(ok,&QPushButton::clicked,[=](){
            if (eName->text().trimmed().isEmpty()) return;
            DoctorData d; d.name=eName->text().trimmed(); d.department=eDept->text().trimmed();
            d.specialty=eSpec->text().trimmed(); d.available=1;
            DatabaseManager::instance().addDoctor(d); refreshDoctors(); dlg->accept();
        });
        dlg->exec();
    });
}

void DirectorDashboard::setupLabTab(QWidget* p) {
    auto* lay = new QVBoxLayout(p);
    auto* btnAdd = new QPushButton("➕ ADD NEW LAB TEST");
    btnAdd->setFixedHeight(38); lay->addWidget(btnAdd);
    tblLabs = new QTableWidget(0,4,p);
    tblLabs->setHorizontalHeaderLabels({"ID","Name","Price (Rs.)","Actions"});
    tblLabs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblLabs->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lay->addWidget(tblLabs,1);
    connect(btnAdd, &QPushButton::clicked, [this](){
        auto* dlg=new QDialog(this); dlg->setWindowTitle("Add Lab Test"); dlg->setMinimumWidth(360);
        auto* fl=new QFormLayout(dlg);
        auto* eName=new QLineEdit(); auto* ePrice=new QDoubleSpinBox();
        ePrice->setRange(0,999999); ePrice->setDecimals(0);
        fl->addRow("Name:",eName); fl->addRow("Price:",ePrice);
        auto* ok=new QPushButton("Add"); fl->addRow(ok);
        connect(ok,&QPushButton::clicked,[=](){
            if(eName->text().trimmed().isEmpty()) return;
            LabTestData t; t.name=eName->text().trimmed(); t.price=ePrice->value(); t.available=1;
            DatabaseManager::instance().addLabTest(t); refreshLabs(); dlg->accept();
        });
        dlg->exec();
    });
}

void DirectorDashboard::setupOpTab(QWidget* p) {
    auto* lay = new QVBoxLayout(p);
    auto* btnAdd = new QPushButton("➕ ADD NEW OPERATION");
    btnAdd->setFixedHeight(38); lay->addWidget(btnAdd);
    tblOps = new QTableWidget(0,4,p);
    tblOps->setHorizontalHeaderLabels({"ID","Name","Price (Rs.)","Actions"});
    tblOps->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblOps->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lay->addWidget(tblOps,1);
    connect(btnAdd, &QPushButton::clicked, [this](){
        auto* dlg=new QDialog(this); dlg->setWindowTitle("Add Operation"); dlg->setMinimumWidth(360);
        auto* fl=new QFormLayout(dlg);
        auto* eName=new QLineEdit(); auto* ePrice=new QDoubleSpinBox();
        ePrice->setRange(0,9999999); ePrice->setDecimals(0);
        fl->addRow("Name:",eName); fl->addRow("Price:",ePrice);
        auto* ok=new QPushButton("Add"); fl->addRow(ok);
        connect(ok,&QPushButton::clicked,[=](){
            if(eName->text().trimmed().isEmpty()) return;
            OperationData o; o.name=eName->text().trimmed(); o.price=ePrice->value(); o.available=1;
            DatabaseManager::instance().addOperation(o); refreshOps(); dlg->accept();
        });
        dlg->exec();
    });
}

void DirectorDashboard::refreshDashboard() {
    auto& db = DatabaseManager::instance();
    DirectorData d = db.getDirector();
    lblRevenue->setText("Rs. " + QString::number(d.bankBalance,'f',0));

    QSqlQuery q(db.db());
    q.exec("SELECT COUNT(*) FROM patients");
    lblPatients->setText(q.next() ? q.value(0).toString() : "0");
    q.exec(QString("SELECT COUNT(*) FROM discharged_patients WHERE date(discharged_at)='%1'")
           .arg(QDate::currentDate().toString("yyyy-MM-dd")));
    lblDischarged->setText(q.next() ? q.value(0).toString() : "0");
    q.exec("SELECT COUNT(*) FROM pharmacy WHERE stock < 10");
    lblStockAlert->setText(q.next() ? q.value(0).toString() : "0");

    auto txns = db.getTransactions();
    tblTxn->setRowCount(0);
    for (const auto& t : txns) {
        int r = tblTxn->rowCount(); tblTxn->insertRow(r);
        tblTxn->setItem(r,0,new QTableWidgetItem(t.patientName));
        tblTxn->setItem(r,1,new QTableWidgetItem("Rs. " + QString::number(t.amount,'f',0)));
        tblTxn->setItem(r,2,new QTableWidgetItem(t.txnDate));
    }
}

void DirectorDashboard::refreshPharmacy() {
    auto meds = DatabaseManager::instance().getAllMedicines();
    tblPharm->setRowCount(0);
    for (const auto& m : meds) {
        int r = tblPharm->rowCount(); tblPharm->insertRow(r);
        tblPharm->setItem(r,0,new QTableWidgetItem(QString::number(m.medicineId)));
        tblPharm->setItem(r,1,new QTableWidgetItem(m.name));
        tblPharm->setItem(r,2,new QTableWidgetItem(QString::number(m.pricePerUnit,'f',0)));
        tblPharm->setItem(r,3,new QTableWidgetItem(QString::number(m.stock)));
        auto* actWid = new QWidget();
        auto* h = new QHBoxLayout(actWid); h->setContentsMargins(2,2,2,2); h->setSpacing(4);
        auto* btnE = new QPushButton("Edit"); auto* btnD = new QPushButton("Del");
        btnE->setFixedHeight(26); btnD->setFixedHeight(26);
        btnD->setStyleSheet("background:#922B21;color:white;border-radius:3px;font-size:11px;");
        btnE->setStyleSheet("background:#1A5276;color:white;border-radius:3px;font-size:11px;");
        h->addWidget(btnE); h->addWidget(btnD);
        tblPharm->setCellWidget(r,4,actWid);
        MedicineData med = m;
        connect(btnE,&QPushButton::clicked,[this,med](){
            auto* dlg=new QDialog(this); dlg->setWindowTitle("Edit Medicine"); dlg->setMinimumWidth(360);
            auto* fl=new QFormLayout(dlg);
            auto* eName=new QLineEdit(med.name); auto* ePrice=new QDoubleSpinBox();
            auto* eStock=new QSpinBox(); ePrice->setRange(0,999999); ePrice->setValue(med.pricePerUnit);
            eStock->setRange(0,99999); eStock->setValue(med.stock);
            fl->addRow("Name:",eName); fl->addRow("Price:",ePrice); fl->addRow("Stock:",eStock);
            auto* ok=new QPushButton("Save"); fl->addRow(ok);
            connect(ok,&QPushButton::clicked,[=](){
                MedicineData u=med; u.name=eName->text().trimmed();
                u.pricePerUnit=ePrice->value(); u.stock=eStock->value();
                DatabaseManager::instance().updateMedicine(u); refreshPharmacy(); dlg->accept();
            }); dlg->exec();
        });
        connect(btnD,&QPushButton::clicked,[this,med](){
            if(QMessageBox::question(this,"Delete","Delete "+med.name+"?")==QMessageBox::Yes){
                DatabaseManager::instance().deleteMedicine(med.medicineId); refreshPharmacy();
            }
        });
    }
}

void DirectorDashboard::refreshDoctors() {
    auto docs = DatabaseManager::instance().getAllDoctors();
    tblDoctors->setRowCount(0);
    for (const auto& d : docs) {
        int r = tblDoctors->rowCount(); tblDoctors->insertRow(r);
        tblDoctors->setItem(r,0,new QTableWidgetItem(QString::number(d.doctorId)));
        tblDoctors->setItem(r,1,new QTableWidgetItem(d.name));
        tblDoctors->setItem(r,2,new QTableWidgetItem(d.department));
        tblDoctors->setItem(r,3,new QTableWidgetItem(d.specialty));
        tblDoctors->setItem(r,4,new QTableWidgetItem(d.available?"Yes":"No"));
        auto* aw=new QWidget(); auto* h=new QHBoxLayout(aw); h->setContentsMargins(2,2,2,2); h->setSpacing(4);
        auto* be=new QPushButton("Edit"); auto* bd=new QPushButton("Del");
        be->setFixedHeight(26); bd->setFixedHeight(26);
        be->setStyleSheet("background:#1A5276;color:white;border-radius:3px;font-size:11px;");
        bd->setStyleSheet("background:#922B21;color:white;border-radius:3px;font-size:11px;");
        h->addWidget(be); h->addWidget(bd);
        tblDoctors->setCellWidget(r,5,aw);
        DoctorData doc=d;
        connect(be,&QPushButton::clicked,[this,doc](){
            auto* dlg=new QDialog(this); dlg->setWindowTitle("Edit Doctor"); dlg->setMinimumWidth(380);
            auto* fl=new QFormLayout(dlg);
            auto* en=new QLineEdit(doc.name); auto* ed=new QLineEdit(doc.department); auto* es=new QLineEdit(doc.specialty);
            fl->addRow("Name:",en); fl->addRow("Department:",ed); fl->addRow("Specialty:",es);
            auto* ok=new QPushButton("Save"); fl->addRow(ok);
            connect(ok,&QPushButton::clicked,[=](){
                DoctorData u=doc; u.name=en->text().trimmed(); u.department=ed->text().trimmed(); u.specialty=es->text().trimmed();
                DatabaseManager::instance().updateDoctor(u); refreshDoctors(); dlg->accept();
            }); dlg->exec();
        });
        connect(bd,&QPushButton::clicked,[this,doc](){
            if(QMessageBox::question(this,"Delete","Delete Dr. "+doc.name+"?")==QMessageBox::Yes){
                DatabaseManager::instance().deleteDoctor(doc.doctorId); refreshDoctors();
            }
        });
    }
}

void DirectorDashboard::refreshLabs() {
    auto labs = DatabaseManager::instance().getAllLabTests();
    tblLabs->setRowCount(0);
    for (const auto& t : labs) {
        int r=tblLabs->rowCount(); tblLabs->insertRow(r);
        tblLabs->setItem(r,0,new QTableWidgetItem(QString::number(t.testId)));
        tblLabs->setItem(r,1,new QTableWidgetItem(t.name));
        tblLabs->setItem(r,2,new QTableWidgetItem(QString::number(t.price,'f',0)));
        auto* aw=new QWidget(); auto* h=new QHBoxLayout(aw); h->setContentsMargins(2,2,2,2); h->setSpacing(4);
        auto* be=new QPushButton("Edit"); auto* bd=new QPushButton("Del");
        be->setFixedHeight(26); bd->setFixedHeight(26);
        be->setStyleSheet("background:#1A5276;color:white;border-radius:3px;font-size:11px;");
        bd->setStyleSheet("background:#922B21;color:white;border-radius:3px;font-size:11px;");
        h->addWidget(be); h->addWidget(bd); tblLabs->setCellWidget(r,3,aw);
        LabTestData lt=t;
        connect(be,&QPushButton::clicked,[this,lt](){
            auto* dlg=new QDialog(this); dlg->setWindowTitle("Edit Lab Test"); dlg->setMinimumWidth(340);
            auto* fl=new QFormLayout(dlg); auto* en=new QLineEdit(lt.name);
            auto* ep=new QDoubleSpinBox(); ep->setRange(0,999999); ep->setValue(lt.price);
            fl->addRow("Name:",en); fl->addRow("Price:",ep);
            auto* ok=new QPushButton("Save"); fl->addRow(ok);
            connect(ok,&QPushButton::clicked,[=](){ LabTestData u=lt; u.name=en->text().trimmed(); u.price=ep->value();
                DatabaseManager::instance().updateLabTest(u); refreshLabs(); dlg->accept(); }); dlg->exec();
        });
        connect(bd,&QPushButton::clicked,[this,lt](){
            if(QMessageBox::question(this,"Delete","Delete "+lt.name+"?")==QMessageBox::Yes){
                DatabaseManager::instance().deleteLabTest(lt.testId); refreshLabs(); }
        });
    }
}

void DirectorDashboard::refreshOps() {
    auto ops = DatabaseManager::instance().getAllOperations();
    tblOps->setRowCount(0);
    for (const auto& o : ops) {
        int r=tblOps->rowCount(); tblOps->insertRow(r);
        tblOps->setItem(r,0,new QTableWidgetItem(QString::number(o.operationId)));
        tblOps->setItem(r,1,new QTableWidgetItem(o.name));
        tblOps->setItem(r,2,new QTableWidgetItem(QString::number(o.price,'f',0)));
        auto* aw=new QWidget(); auto* h=new QHBoxLayout(aw); h->setContentsMargins(2,2,2,2); h->setSpacing(4);
        auto* be=new QPushButton("Edit"); auto* bd=new QPushButton("Del");
        be->setFixedHeight(26); bd->setFixedHeight(26);
        be->setStyleSheet("background:#1A5276;color:white;border-radius:3px;font-size:11px;");
        bd->setStyleSheet("background:#922B21;color:white;border-radius:3px;font-size:11px;");
        h->addWidget(be); h->addWidget(bd); tblOps->setCellWidget(r,3,aw);
        OperationData op=o;
        connect(be,&QPushButton::clicked,[this,op](){
            auto* dlg=new QDialog(this); dlg->setWindowTitle("Edit Operation"); dlg->setMinimumWidth(360);
            auto* fl=new QFormLayout(dlg); auto* en=new QLineEdit(op.name);
            auto* ep=new QDoubleSpinBox(); ep->setRange(0,9999999); ep->setValue(op.price);
            fl->addRow("Name:",en); fl->addRow("Price:",ep);
            auto* ok=new QPushButton("Save"); fl->addRow(ok);
            connect(ok,&QPushButton::clicked,[=](){ OperationData u=op; u.name=en->text().trimmed(); u.price=ep->value();
                DatabaseManager::instance().updateOperation(u); refreshOps(); dlg->accept(); }); dlg->exec();
        });
        connect(bd,&QPushButton::clicked,[this,op](){
            if(QMessageBox::question(this,"Delete","Delete "+op.name+"?")==QMessageBox::Yes){
                DatabaseManager::instance().deleteOperation(op.operationId); refreshOps(); }
        });
    }
}

void DirectorDashboard::applyBackground() {
    QPixmap bg(":/assets/bg_director.png");
    if (!bg.isNull()) {
        QPalette p; p.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(p); centralWidget->setAutoFillBackground(true);
    }
}

void DirectorDashboard::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
