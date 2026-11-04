#ifndef DIRECTORDASHBOARD_H
#define DIRECTORDASHBOARD_H
#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include "../db/DatabaseManager.h"

class DirectorDashboard : public QMainWindow {
    Q_OBJECT
public:
    explicit DirectorDashboard(const DirectorData& director, QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent*) override;
private:
    void setupDashboardTab(QWidget* parent);
    void setupPharmacyTab(QWidget* parent);
    void setupDoctorTab(QWidget* parent);
    void setupLabTab(QWidget* parent);
    void setupOpTab(QWidget* parent);
    void refreshDashboard();
    void refreshPharmacy();
    void refreshDoctors();
    void refreshLabs();
    void refreshOps();
    void applyBackground();

    DirectorData  m_director;
    QLabel*       lblRevenue;
    QLabel*       lblPatients;
    QLabel*       lblDischarged;
    QLabel*       lblStockAlert;
    QTableWidget* tblTxn;
    QTableWidget* tblPharm;
    QTableWidget* tblDoctors;
    QTableWidget* tblLabs;
    QTableWidget* tblOps;
    QWidget*      centralWidget;
    QWidget*      overlay;
};
#endif
