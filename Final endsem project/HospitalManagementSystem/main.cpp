#include "gui/MainWindow.h"
#include "db/DatabaseManager.h"
#include <QApplication>
#include <QDir>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Hospital Management System");
    app.setOrganizationName("Noor Ul Huda Soomro Memorial Hospital");

    // ── Global stylesheet ─────────────────────────────────────────
    app.setStyleSheet(R"(
        QMainWindow, QDialog, QWidget {
            background-color: #1C1C1C;
            color: #F0F0F0;
            font-family: 'Segoe UI';
            font-size: 14px;
        }
        QPushButton {
            background-color: #C0392B;
            color: white;
            border-radius: 6px;
            padding: 10px 20px;
            font-weight: bold;
            font-size: 14px;
        }
        QPushButton:hover  { background-color: #E74C3C; }
        QPushButton:pressed{ background-color: #922B21; }
        QPushButton:disabled{ background-color: #555; color: #888; }
        QLineEdit, QTextEdit, QComboBox, QSpinBox, QDoubleSpinBox {
            background-color: #2C2C2C;
            color: #F0F0F0;
            border: 1px solid #C0392B;
            border-radius: 4px;
            padding: 6px;
        }
        QLabel { color: #F0F0F0; }
        QTableWidget {
            background-color: #2C2C2C;
            color: #F0F0F0;
            gridline-color: #C0392B;
            selection-background-color: #C0392B;
        }
        QHeaderView::section {
            background-color: #C0392B;
            color: white;
            font-weight: bold;
            padding: 6px;
        }
        QGroupBox {
            border: 1px solid #C0392B;
            border-radius: 6px;
            margin-top: 10px;
            color: #C0392B;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 4px;
        }
        QScrollBar:vertical {
            background: #2C2C2C;
            width: 10px;
        }
        QScrollBar::handle:vertical {
            background: #C0392B;
            border-radius: 5px;
        }
        QTabBar::tab {
            background: #2C2C2C;
            color: #F0F0F0;
            padding: 8px 16px;
            border-radius: 4px;
        }
        QTabBar::tab:selected {
            background: #C0392B;
            color: white;
            font-weight: bold;
        }
        QComboBox QAbstractItemView {
            background: #2C2C2C;
            color: #F0F0F0;
            selection-background-color: #C0392B;
        }
        QMessageBox { background-color: #1C1C1C; }
        QScrollArea { border: none; background: transparent; }
    )");

    // ── Initialise database ───────────────────────────────────────
    QString dbPath = QCoreApplication::applicationDirPath() + "/db/hospital.db";
    QDir().mkpath(QCoreApplication::applicationDirPath() + "/db");
    QDir().mkpath(QCoreApplication::applicationDirPath() + "/prescriptions");

    qDebug() << "=== DATABASE PATH ===" << dbPath;

    if (!DatabaseManager::instance().initialize(dbPath)) {
        QMessageBox::critical(nullptr, "Database Error",
            "Failed to open hospital.db\nPath: " + dbPath +
            "\nPlease ensure the db/ directory is writable.");
        return 1;
    }
    qDebug() << "=== DB INIT OK — seeding done ===";

    // ── Launch main window ────────────────────────────────────────
    MainWindow w;
    w.show();

    return app.exec();
}
