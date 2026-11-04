#ifndef PHARMACYSCREEN_H
#define PHARMACYSCREEN_H
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
class PharmacyScreen : public QMainWindow {
    Q_OBJECT
public:
    explicit PharmacyScreen(int patientId, const QString& prescriptionId, QWidget* parent = nullptr);
private:
    void loadMedicines();
    void onProcess();
    int           m_patientId;
    QString       m_prescriptionId;
    QTableWidget* tbl;
    QLineEdit*    edtPrescId;
    QPushButton*  btnProcess;
};
#endif
