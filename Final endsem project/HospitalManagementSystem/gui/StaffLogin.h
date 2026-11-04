#ifndef STAFFLOGIN_H
#define STAFFLOGIN_H
#include <QMainWindow>
#include <QLineEdit>
class StaffLogin : public QMainWindow {
    Q_OBJECT
public:
    explicit StaffLogin(QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent*) override;
private:
    void onLogin();
    void applyBackground();
    QLineEdit* edtUser;
    QLineEdit* edtPass;
    QWidget*   centralWidget;
    QWidget*   overlay;
};
#endif
