#ifndef LABTESTSCREEN_H
#define LABTESTSCREEN_H
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
class LabTestScreen : public QMainWindow {
    Q_OBJECT
public:
    explicit LabTestScreen(int patientId, const QString& prescriptionId, QWidget* parent = nullptr);
private:
    void loadTests();
    void onProcess();
    QString generateTestResult(const QString& testName);
    int           m_patientId;
    QString       m_prescriptionId;
    QTableWidget* tbl;
    QLineEdit*    edtPrescId;
};
#endif
