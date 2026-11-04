#ifndef DISCHARGESCREEN_H
#define DISCHARGESCREEN_H
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QString>
class DischargeScreen : public QMainWindow {
    Q_OBJECT
public:
    explicit DischargeScreen(int patientId, const QString& patientName,
                             const QString& prescriptionId, QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent*) override;
private:
    void loadBill();
    void onPay();
    void applyBackground();
    int       m_patientId;
    QString   m_patientName;
    QString   m_prescriptionId;
    double    m_totalBill = 0;
    QLabel*   lblBill;
    QLineEdit* edtAmount;
    QWidget*  centralWidget;
    QWidget*  overlay;
};
#endif
