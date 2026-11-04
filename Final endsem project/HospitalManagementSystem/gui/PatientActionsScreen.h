#ifndef PATIENTACTIONSSCREEN_H
#define PATIENTACTIONSSCREEN_H
#include <QMainWindow>
#include <QString>
class PatientActionsScreen : public QMainWindow {
    Q_OBJECT
public:
    explicit PatientActionsScreen(int patientId, const QString& patientName,
                                  const QString& prescriptionId,
                                  QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent*) override;
private:
    void applyBackground();
    int     m_patientId;
    QString m_patientName;
    QString m_prescriptionId;
    QWidget* centralWidget;
    QWidget* overlay;
};
#endif
