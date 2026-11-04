#ifndef PATIENTPORTAL_H
#define PATIENTPORTAL_H
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
class PatientPortal : public QMainWindow {
    Q_OBJECT
public:
    explicit PatientPortal(QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent*) override;
private:
    void applyBackground();
    QWidget* centralWidget;
    QWidget* overlay;
};
#endif
