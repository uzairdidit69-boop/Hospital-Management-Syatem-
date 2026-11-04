#ifndef PATIENTREGISTER_H
#define PATIENTREGISTER_H
#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
class PatientRegister : public QMainWindow {
    Q_OBJECT
public:
    explicit PatientRegister(QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent*) override;
private:
    void applyBackground();
    void onSubmit();
    QWidget*     centralWidget;
    QWidget*     overlay;
    QLineEdit*   edtName;
    QLineEdit*   edtCnic;
    QLineEdit*   edtGuardian;
    QTextEdit*   edtAddress;
    QLineEdit*   edtPhone;
    QSpinBox*    spnAge;
    QComboBox*   cmbGender;
    QComboBox*   cmbBlood;
    QPushButton* btnSubmit;
};
#endif
