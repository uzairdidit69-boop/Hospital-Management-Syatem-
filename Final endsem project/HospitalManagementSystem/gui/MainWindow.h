#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupUI();
    void applyBackground();

    QLabel*      lblLogo;
    QLabel*      lblTitle;
    QLabel*      lblSubtitle;
    QPushButton* btnPatient;
    QPushButton* btnDirector;
    QPushButton* btnStaff;
    QLabel*      lblVersion;
    QWidget*     centralWidget;
    QWidget*     overlay;
};

#endif
