#ifndef OPERATIONSCREEN_H
#define OPERATIONSCREEN_H
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
class OperationScreen : public QMainWindow {
    Q_OBJECT
public:
    explicit OperationScreen(int patientId, const QString& prescriptionId, QWidget* parent = nullptr);
private:
    void loadOperations();
    void onCheck();
    QString generateOperationReport(const QString& opName, const QString& surgeon);
    int           m_patientId;
    QString       m_prescriptionId;
    QTableWidget* tbl;
    QLineEdit*    edtPrescId;
};
#endif
