#ifndef OPDSCREEN_H
#define OPDSCREEN_H
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QString>
class OPDScreen : public QMainWindow {
    Q_OBJECT
public:
    explicit OPDScreen(int patientId, const QString& patientName, QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent*) override;
private:
    void loadDoctors();
    void onConsult();
    void applyBackground();
    int           m_patientId;
    QString       m_patientName;
    QTableWidget* tblDoctors;
    QPushButton*  btnConsult;
    QWidget*      centralWidget;
    QWidget*      overlay;
};
#endif
