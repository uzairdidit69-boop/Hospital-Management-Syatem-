#include "DoctorChatScreen.h"
#include "PatientActionsScreen.h"
#include "OPDScreen.h"
#include "../db/DatabaseManager.h"
#include "../core/PrescriptionManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPalette>
#include <QPixmap>
#include <QResizeEvent>
#include <QScrollBar>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>

DoctorChatScreen::DoctorChatScreen(int patientId,   const QString& patientName,
                                   int doctorId,   const QString& doctorName,
                                   const QString& department,
                                   const QString& prescriptionId,
                                   int patientAge,
                                   const QString& patientGender,
                                   QWidget* parent)
    : QMainWindow(parent),
      m_patientId(patientId), m_patientName(patientName),
      m_doctorId(doctorId),   m_doctorName(doctorName),
      m_department(department), m_prescriptionId(prescriptionId),
      m_patientAge(patientAge), m_patientGender(patientGender)
{
    setWindowTitle("Doctor Consultation");
    setMinimumSize(900,650);
    resize(1100,720);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    overlay = new QWidget(centralWidget);
    overlay->setStyleSheet("background-color:rgba(28,28,28,200);");

    auto* mainLay = new QVBoxLayout(centralWidget);
    mainLay->setContentsMargins(30,20,30,20);
    mainLay->setSpacing(10);

    auto* title = new QLabel("🩺  Consultation with Dr. " + doctorName);
    title->setStyleSheet("color:#C0392B;font-size:22px;font-weight:bold;background:transparent;");
    title->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(title);

    auto* info = new QLabel("Patient: " + patientName +
                            "   |   Department: " + department +
                            "   |   Rx: " + prescriptionId);
    info->setStyleSheet("color:#aaa;font-size:12px;background:transparent;");
    info->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(info);

    chatDisplay = new QTextEdit();
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet("background:#1a1a1a;color:#F0F0F0;border:1px solid #C0392B;"
                               "border-radius:6px;padding:10px;font-size:13px;");
    mainLay->addWidget(chatDisplay,1);

    auto* inputRow = new QHBoxLayout();
    inputLine = new QLineEdit();
    inputLine->setPlaceholderText("Describe your symptoms and press Enter or Send...");
    inputLine->setFixedHeight(40);
    btnSend = new QPushButton("SEND ▶");
    btnSend->setFixedSize(120,40);
    btnSend->setCursor(Qt::PointingHandCursor);
    inputRow->addWidget(inputLine,1);
    inputRow->addWidget(btnSend);
    mainLay->addLayout(inputRow);

    auto* btnBack = new QPushButton("← Back to OPD");
    btnBack->setStyleSheet("background:#555;color:white;border-radius:6px;padding:10px;font-weight:bold;");
    btnBack->setFixedHeight(42);
    mainLay->addWidget(btnBack);

    // Gemini client
    gemini = new GeminiClient(this);
    connect(gemini, &GeminiClient::responseReceived, this, &DoctorChatScreen::onAIResponse);
    connect(gemini, &GeminiClient::errorOccurred,    this, &DoctorChatScreen::onAIError);

    connect(btnSend,   &QPushButton::clicked,       this, &DoctorChatScreen::onSend);
    connect(inputLine, &QLineEdit::returnPressed,   this, &DoctorChatScreen::onSend);
    connect(btnBack,   &QPushButton::clicked, [this](){
        auto reply = QMessageBox::question(this,"Leave Consultation",
            "Leave consultation? Your progress will not be saved.",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            auto* w = new OPDScreen(m_patientId, m_patientName);
            w->show(); close();
        }
    });

    buildSystemPrompt();

    // Auto-start: greet the patient
    appendChat("Dr. " + doctorName,
        "Hello " + patientName + ", I am Dr. " + doctorName +
        " (" + department + "). Please describe your symptoms.",
        "#4FC3F7");

    // Add greeting to history as model turn
    QJsonObject part; part["text"] = "Hello " + patientName +
        ", I am Dr. " + doctorName + " (" + department +
        "). Please describe your symptoms.";
    QJsonObject turn; turn["role"] = "model"; turn["parts"] = QJsonArray{part};
    m_history.append(turn);

    applyBackground();
}

void DoctorChatScreen::buildSystemPrompt() {
    auto& db = DatabaseManager::instance();

    // Collect available medicine/test/operation names
    auto meds = db.getAllMedicines();
    auto labs = db.getAllLabTests();
    auto ops  = db.getAllOperations();

    QStringList medNames, labNames, opNames;
    for (auto& m : meds) medNames << m.name;
    for (auto& l : labs) labNames << l.name;
    for (auto& o : ops)  opNames  << o.name;

    m_systemPrompt = QString(
        "You are Dr. %1, a %2 specialist at Noor Ul Huda Soomro Memorial Hospital.\n"
        "A patient named %3 (Age: %4, Gender: %5) has come to see you.\n\n"
        "Conduct a realistic, professional medical consultation:\n"
        "- Ask about symptoms one step at a time.\n"
        "- Follow up intelligently based on patient responses.\n"
        "- When you have enough information, conclude with a prescription.\n\n"
        "When concluding, output ONLY a valid JSON block — no text before or after it:\n"
        "{\n"
        "  \"medicines\": [{\"name\": \"MedicineName\", \"qty\": 2}],\n"
        "  \"lab_tests\": [\"TestName\"],\n"
        "  \"operation\": \"OperationName or NONE\",\n"
        "  \"notes\": \"Any additional instructions\"\n"
        "}\n\n"
        "Available medicines (use EXACT names): %6\n"
        "Available lab tests (use EXACT names): %7\n"
        "Available operations (use EXACT names): %8\n\n"
        "Keep all non-prescription responses to 2-4 sentences."
    ).arg(m_doctorName, m_department, m_patientName,
          QString::number(m_patientAge), m_patientGender.isEmpty() ? "Not specified" : m_patientGender,
          medNames.join(", "),
          labNames.join(", "),
          opNames.join(", "));
}

void DoctorChatScreen::onSend() {
    QString msg = inputLine->text().trimmed();
    if (msg.isEmpty()) return;
    inputLine->clear();
    btnSend->setEnabled(false);
    inputLine->setEnabled(false);
    btnSend->setText("⏳");

    appendChat("You", msg, "#81C784");

    // Add user turn to history
    QJsonObject part; part["text"] = msg;
    QJsonObject turn; turn["role"] = "user"; turn["parts"] = QJsonArray{part};
    m_history.append(turn);

    gemini->sendMessage(m_systemPrompt, m_history);
}

void DoctorChatScreen::onAIResponse(const QString& response) {
    btnSend->setEnabled(true);
    inputLine->setEnabled(true);
    btnSend->setText("SEND ▶");

    // Add model turn to history
    QJsonObject part; part["text"] = response;
    QJsonObject turn; turn["role"] = "model"; turn["parts"] = QJsonArray{part};
    m_history.append(turn);

    // Detect JSON prescription block
    QString trimmed = response.trimmed();
    int jsonStart = trimmed.indexOf('{');
    int jsonEnd   = trimmed.lastIndexOf('}');

    if (jsonStart != -1 && jsonEnd > jsonStart) {
        QString jsonPart = trimmed.mid(jsonStart, jsonEnd - jsonStart + 1);
        QJsonDocument testDoc = QJsonDocument::fromJson(jsonPart.toUtf8());
        if (!testDoc.isNull() && testDoc.object().contains("medicines")) {
            appendChat("Dr. " + m_doctorName,
                "<i>Consultation complete. Generating your prescription...</i>",
                "#4FC3F7");
            processPrescriptionJson(jsonPart);
            return;
        }
    }

    // Normal chat reply
    appendChat("Dr. " + m_doctorName, response.toHtmlEscaped(), "#4FC3F7");
}

void DoctorChatScreen::onAIError(const QString& error) {
    btnSend->setEnabled(true);
    inputLine->setEnabled(true);
    btnSend->setText("SEND ▶");
    appendChat("System","⚠️ AI Error: " + error, "#EF5350");
}

void DoctorChatScreen::processPrescriptionJson(const QString& jsonText) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8());
    if (doc.isNull()) {
        QMessageBox::warning(this,"Parse Error","Could not parse prescription from AI.");
        return;
    }

    QJsonObject obj    = doc.object();
    QJsonArray  medsArr = obj["medicines"].toArray();
    QJsonArray  labsArr = obj["lab_tests"].toArray();
    QString     opName  = obj["operation"].toString();
    QString     notes   = obj["notes"].toString();

    if (opName.toLower() == "null") opName = "NONE";

    // Write .txt prescription file
    vector<pair<string,int>> meds;
    for (const auto& mv : medsArr) {
        QJsonObject mo = mv.toObject();
        meds.push_back({mo["name"].toString().toStdString(), mo["qty"].toInt()});
    }
    vector<string> labs;
    for (const auto& lv : labsArr) labs.push_back(lv.toString().toStdString());

    PrescriptionManager::writePrescriptionFile(
        m_prescriptionId.toStdString(),
        m_patientName.toStdString(),
        ("Dr. " + m_doctorName).toStdString(),
        meds, labs, opName.toStdString());

    // Insert into prescriptions table
    PrescriptionData pd;
    pd.prescriptionId = m_prescriptionId;
    pd.patientId      = m_patientId;
    pd.doctorId       = m_doctorId;
    pd.medicines      = QJsonDocument(medsArr).toJson(QJsonDocument::Compact);
    pd.labTests       = QJsonDocument(labsArr).toJson(QJsonDocument::Compact);
    pd.operation      = opName;
    DatabaseManager::instance().insertPrescription(pd);

    QMessageBox::information(this,"✅ Prescription Ready",
        QString("Consultation complete!\n\n"
                "Prescription ID: %1\n\n"
                "Note this ID — you will need it at:\n"
                "  • Pharmacy\n  • Lab Tests\n  • Operations\n  • Discharge")
        .arg(m_prescriptionId));

    auto* w = new PatientActionsScreen(m_patientId, m_patientName, m_prescriptionId);
    w->show(); close();
}

void DoctorChatScreen::appendChat(const QString& speaker, const QString& msg, const QString& color) {
    chatDisplay->append(
        QString("<p><b style='color:%1'>%2:</b> <span style='color:#E0E0E0'>%3</span></p>")
        .arg(color, speaker, msg));
    chatDisplay->verticalScrollBar()->setValue(chatDisplay->verticalScrollBar()->maximum());
}

void DoctorChatScreen::applyBackground() {
    QPixmap bg(":/assets/bg_patient.png");
    if (!bg.isNull()) {
        QPalette p;
        p.setBrush(QPalette::Window,
            QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        centralWidget->setPalette(p);
        centralWidget->setAutoFillBackground(true);
    }
}

void DoctorChatScreen::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (overlay) overlay->resize(centralWidget->size());
    applyBackground();
}
