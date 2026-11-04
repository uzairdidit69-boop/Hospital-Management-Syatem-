#ifndef PATIENTLOGIN_H
#define PATIENTLOGIN_H
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
class PatientLogin : public QMainWindow {
    Q_OBJECT
public:
    explicit PatientLogin(QWidget* parent = nullptr);
private:
    void onLogin();
    QLineEdit*   edtId;
    QPushButton* btnLogin;
};
#endif
