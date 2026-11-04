#ifndef STAFFPORTAL_H
#define STAFFPORTAL_H
#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include "../db/DatabaseManager.h"
class StaffPortal : public QMainWindow {
    Q_OBJECT
public:
    explicit StaffPortal(const StaffData& staff, QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent*) override;
private:
    void loadPatients();
    void onRowSelected();
    void onUpdateStatus();
    void applyBackground();
    StaffData     m_staff;
    QTableWidget* tblPatients;
    QLabel*       lblSelected;
    QComboBox*    cmbStatus;
    QLineEdit*    edtWard;
    QWidget*      centralWidget;
    QWidget*      overlay;
    int           m_selectedPatientId = -1;
};
#endif
