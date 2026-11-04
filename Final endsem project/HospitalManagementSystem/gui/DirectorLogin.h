#ifndef DIRECTORLOGIN_H
#define DIRECTORLOGIN_H
#include <QMainWindow>
#include <QLineEdit>
class DirectorLogin : public QMainWindow {
    Q_OBJECT
public:
    explicit DirectorLogin(QWidget* parent = nullptr);
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
