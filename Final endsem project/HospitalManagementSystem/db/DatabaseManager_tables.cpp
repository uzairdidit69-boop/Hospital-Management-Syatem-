#include "DatabaseManager.h"
#include <QDebug>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

bool DatabaseManager::initialize(const QString& dbPath) {
    _db = QSqlDatabase::addDatabase("QSQLITE", "hms_connection");
    _db.setDatabaseName(dbPath);
    if (!_db.open()) {
        qDebug() << "DB open failed:" << _db.lastError().text();
        return false;
    }
    QSqlQuery pq(_db);
    pq.exec("PRAGMA foreign_keys = ON");
    if (!createTablesIfNotExist()) return false;
    seedInitialData();
    return true;
}

bool DatabaseManager::isConnected() const {
    return _db.isOpen();
}

bool DatabaseManager::createTablesIfNotExist() {
    QSqlDatabase db = QSqlDatabase::database("hms_connection");
    QSqlQuery q(db);
    QStringList tables = {
        R"(CREATE TABLE IF NOT EXISTS patients (
            patient_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL, cnic TEXT UNIQUE NOT NULL,
            guardian TEXT, address TEXT, phone TEXT,
            age INTEGER, gender TEXT, blood_group TEXT,
            registered_at DATETIME DEFAULT CURRENT_TIMESTAMP))",
        R"(CREATE TABLE IF NOT EXISTS discharged_patients (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            patient_id INTEGER, name TEXT, cnic TEXT,
            total_bill REAL, amount_paid REAL,
            discharged_at DATETIME DEFAULT CURRENT_TIMESTAMP))",
        R"(CREATE TABLE IF NOT EXISTS doctors (
            doctor_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL, department TEXT NOT NULL,
            specialty TEXT NOT NULL, available INTEGER DEFAULT 1))",
        R"(CREATE TABLE IF NOT EXISTS pharmacy (
            medicine_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL, price_per_unit REAL NOT NULL,
            stock INTEGER NOT NULL))",
        R"(CREATE TABLE IF NOT EXISTS lab_tests (
            test_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL, price REAL NOT NULL,
            available INTEGER DEFAULT 1))",
        R"(CREATE TABLE IF NOT EXISTS operations (
            operation_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL, price REAL NOT NULL,
            available INTEGER DEFAULT 1))",
        R"(CREATE TABLE IF NOT EXISTS prescriptions (
            prescription_id TEXT PRIMARY KEY,
            patient_id INTEGER, doctor_id INTEGER,
            medicines TEXT, lab_tests TEXT, operation TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(patient_id) REFERENCES patients(patient_id),
            FOREIGN KEY(doctor_id) REFERENCES doctors(doctor_id)))",
        R"(CREATE TABLE IF NOT EXISTS billing (
            bill_id INTEGER PRIMARY KEY AUTOINCREMENT,
            patient_id INTEGER, medicine_cost REAL DEFAULT 0,
            labtest_cost REAL DEFAULT 0, operation_cost REAL DEFAULT 0,
            total_cost REAL DEFAULT 0, paid INTEGER DEFAULT 0,
            FOREIGN KEY(patient_id) REFERENCES patients(patient_id)))",
        R"(CREATE TABLE IF NOT EXISTS staff (
            staff_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL, role TEXT NOT NULL,
            shift TEXT NOT NULL, ward TEXT,
            username TEXT UNIQUE NOT NULL, password TEXT NOT NULL))",
        R"(CREATE TABLE IF NOT EXISTS director (
            director_id INTEGER PRIMARY KEY,
            name TEXT NOT NULL, username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL, bank_balance REAL DEFAULT 0))",
        R"(CREATE TABLE IF NOT EXISTS director_transactions (
            txn_id INTEGER PRIMARY KEY AUTOINCREMENT,
            patient_id INTEGER, patient_name TEXT,
            amount REAL, txn_date DATETIME DEFAULT CURRENT_TIMESTAMP))",
        R"(CREATE TABLE IF NOT EXISTS ward (
            ward_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL, capacity INTEGER DEFAULT 10,
            occupied INTEGER DEFAULT 0))",
        R"(CREATE TABLE IF NOT EXISTS patient_status (
            status_id INTEGER PRIMARY KEY AUTOINCREMENT,
            patient_id INTEGER, status TEXT DEFAULT 'Admitted',
            assigned_ward TEXT, assigned_doctor INTEGER,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(patient_id) REFERENCES patients(patient_id)))"
    };
    for (const QString& sql : tables) {
        if (!q.exec(sql)) {
            qDebug() << "Table creation failed:" << q.lastError().text();
            return false;
        }
    }
    return true;
}

bool DatabaseManager::seedInitialData() {
    // Use named connection — DO NOT use _db directly (value copy gets detached)
    QSqlDatabase db = QSqlDatabase::database("hms_connection");
    QSqlQuery q(db);
    QSqlQuery chk(db);

    // ── Doctors (10) ─────────────────────────────────────────────────────
    chk.exec("SELECT COUNT(*) FROM doctors");
    if (!chk.next() || chk.value(0).toInt() == 0) {
        const QStringList doctors = {
            "('Dr. Ayesha Khan',   'ENT',         'Ear Nose Throat Specialist',  1)",
            "('Dr. Bilal Ahmed',   'ENT',         'Rhinology Specialist',         1)",
            "('Dr. Sara Malik',    'Cardiology',  'Interventional Cardiologist', 1)",
            "('Dr. Usman Qureshi', 'Cardiology',  'Electrophysiologist',          1)",
            "('Dr. Nadia Hussain', 'Gynecology',  'Obstetrics Specialist',        1)",
            "('Dr. Razia Sultana', 'Gynecology',  'Reproductive Medicine',        1)",
            "('Dr. Kamran Ali',    'Orthopedics', 'Joint Replacement Surgeon',   1)",
            "('Dr. Zafar Iqbal',   'Orthopedics', 'Spine Surgeon',                1)",
            "('Dr. Hina Fatima',   'Pediatrics',  'Neonatology Specialist',       1)",
            "('Dr. Tariq Mehmood', 'Pediatrics',  'Pediatric Cardiologist',       1)"
        };
        for (const QString& d : doctors)
            q.exec("INSERT INTO doctors(name,department,specialty,available) VALUES " + d);
        qDebug() << "Seeded doctors:" << doctors.size();
    }

    // ── Medicines (50) ───────────────────────────────────────────────────
    chk.exec("SELECT COUNT(*) FROM pharmacy");
    if (!chk.next() || chk.value(0).toInt() == 0) {
        const QStringList meds = {
            "('Panadol',100,500)",             "('Ponstan',50,400)",
            "('Amoxicillin',80,600)",          "('Augmentin',200,300)",
            "('Brufen',60,450)",               "('Metformin',40,800)",
            "('Atorvastatin',120,350)",        "('Omeprazole',70,500)",
            "('Cetirizine',30,700)",           "('Azithromycin',180,250)",
            "('Clopidogrel',250,200)",         "('Lisinopril',90,300)",
            "('Aspirin',20,900)",              "('Dexamethasone',150,200)",
            "('Metronidazole',55,600)",        "('Ciprofloxacin',130,400)",
            "('Ranitidine',45,550)",           "('Salbutamol Inhaler',350,150)",
            "('Insulin Glargine',800,100)",    "('Warfarin',110,180)",
            "('Prednisolone',90,300)",         "('Tramadol',160,200)",
            "('Diclofenac',75,450)",           "('Pantoprazole',85,500)",
            "('Folic Acid',25,800)",           "('Amitriptyline',140,180)",
            "('Clonazepam',200,150)",          "('Fluoxetine',180,200)",
            "('Sertraline',160,220)",          "('Loratadine',35,600)",
            "('Montelukast',120,300)",         "('Amlodipine',75,400)",
            "('Bisoprolol',110,280)",          "('Carvedilol',130,220)",
            "('Furosemide',50,500)",           "('Spironolactone',80,350)",
            "('Levothyroxine',60,450)",        "('Glibenclamide',35,600)",
            "('Sitagliptin',350,150)",         "('Esomeprazole',90,400)",
            "('Clarithromycin',220,200)",      "('Doxycycline',100,350)",
            "('Cefixime',180,300)",            "('Vancomycin',800,80)",
            "('Morphine',500,60)",             "('Ondansetron',120,250)",
            "('Domperidone',40,600)",          "('Vitamin D3',30,900)",
            "('Calcium Carbonate',25,800)",    "('Zinc Sulphate',20,700)"
        };
        for (const QString& m : meds)
            q.exec("INSERT INTO pharmacy(name,price_per_unit,stock) VALUES(" + m + ")");
        qDebug() << "Seeded medicines:" << meds.size();
    }

    // ── Lab Tests (30) ───────────────────────────────────────────────────
    chk.exec("SELECT COUNT(*) FROM lab_tests");
    if (!chk.next() || chk.value(0).toInt() == 0) {
        const QStringList tests = {
            "('Blood Complete Count',1500,1)",    "('Urine Analysis',800,1)",
            "('X-Ray Chest',2500,1)",             "('MRI Brain',15000,1)",
            "('ECG',2000,1)",                     "('Ultrasound Abdomen',3500,1)",
            "('Blood Sugar Fasting',500,1)",      "('Liver Function Test',3000,1)",
            "('Kidney Function Test',2800,1)",    "('Thyroid Profile',4000,1)",
            "('CT Scan Head',12000,1)",           "('Hepatitis B Antigen',1800,1)",
            "('Hepatitis C Antibody',2000,1)",    "('HbA1c',2500,1)",
            "('Lipid Profile',2200,1)",           "('Serum Creatinine',1200,1)",
            "('Uric Acid',1100,1)",               "('Stool Routine',700,1)",
            "('Blood Culture',4500,1)",           "('Echocardiography',8000,1)",
            "('Bone Marrow Biopsy',20000,1)",     "('Sputum Culture',3000,1)",
            "('Prothrombin Time',1800,1)",        "('D-Dimer',3500,1)",
            "('Troponin I',4000,1)",              "('PSA Total',3500,1)",
            "('CA-125',5000,1)",                  "('HIV Screening',2500,1)",
            "('Dengue NS1 Antigen',2000,1)",      "('Typhoid IgM',1500,1)"
        };
        for (const QString& t : tests)
            q.exec("INSERT INTO lab_tests(name,price,available) VALUES(" + t + ")");
        qDebug() << "Seeded lab tests:" << tests.size();
    }

    // ── Operations (20) ──────────────────────────────────────────────────
    chk.exec("SELECT COUNT(*) FROM operations");
    if (!chk.next() || chk.value(0).toInt() == 0) {
        const QStringList ops = {
            "('Appendectomy',80000,1)",           "('Cholecystectomy',120000,1)",
            "('Brain Surgery',500000,1)",         "('Knee Replacement',350000,1)",
            "('Cataract Surgery',60000,1)",       "('Tonsillectomy',40000,1)",
            "('Hernia Repair',75000,1)",          "('Coronary Bypass',600000,1)",
            "('Hip Replacement',400000,1)",       "('Thyroidectomy',150000,1)",
            "('C-Section',180000,1)",             "('Spinal Fusion',450000,1)",
            "('Prostatectomy',300000,1)",         "('Hysterectomy',250000,1)",
            "('Kidney Transplant',900000,1)",     "('Liver Transplant',1200000,1)",
            "('Gastric Bypass',400000,1)",        "('Pacemaker Implant',550000,1)",
            "('LASIK Eye Surgery',120000,1)",     "('Rhinoplasty',200000,1)"
        };
        for (const QString& o : ops)
            q.exec("INSERT INTO operations(name,price,available) VALUES(" + o + ")");
        qDebug() << "Seeded operations:" << ops.size();
    }

    // ── Staff (20) ───────────────────────────────────────────────────────
    chk.exec("SELECT COUNT(*) FROM staff");
    if (!chk.next() || chk.value(0).toInt() == 0) {
        const QStringList staff = {
            "('Fatima Noor',     'Nurse',        'Morning','General Ward',     'nurse1',   'pass123')",
            "('Zara Sheikh',     'Nurse',        'Night',  'Surgical Ward',    'nurse2',   'pass123')",
            "('Maria Akram',     'Nurse',        'Evening','Pediatrics Ward',  'nurse3',   'pass123')",
            "('Hassan Khan',     'Nurse',        'Morning','ICU',              'nurse4',   'pass123')",
            "('Sana Mirza',      'Nurse',        'Night',  'General Ward',     'nurse5',   'pass123')",
            "('Rabia Naz',       'Nurse',        'Morning','Cardiology Ward',  'nurse6',   'pass123')",
            "('Amna Bibi',       'Nurse',        'Evening','Maternity Ward',   'nurse7',   'pass123')",
            "('Kiran Akhtar',    'Nurse',        'Night',  'Orthopedics Ward', 'nurse8',   'pass123')",
            "('Ahmed Raza',      'Ward Boy',     'Evening','ICU',              'wardboy1', 'pass123')",
            "('Faisal Rehman',   'Ward Boy',     'Morning','Surgical Ward',    'wardboy2', 'pass123')",
            "('Usman Butt',      'Ward Boy',     'Night',  'General Ward',     'wardboy3', 'pass123')",
            "('Salman Malik',    'Ward Boy',     'Evening','Cardiology Ward',  'wardboy4', 'pass123')",
            "('Tariq Hassan',    'Technician',   'Morning','Lab',              'tech1',    'pass123')",
            "('Nazia Awan',      'Technician',   'Evening','Lab',              'tech2',    'pass123')",
            "('Bilal Saeed',     'Technician',   'Night',  'Radiology',        'tech3',    'pass123')",
            "('Omar Farooq',     'Receptionist', 'Morning','Reception',        'recept1',  'pass123')",
            "('Hira Baig',       'Receptionist', 'Evening','Reception',        'recept2',  'pass123')",
            "('Asad Khan',       'Pharmacist',   'Morning','Pharmacy',         'pharm1',   'pass123')",
            "('Sobia Rashid',    'Pharmacist',   'Evening','Pharmacy',         'pharm2',   'pass123')",
            "('Imran Javed',     'Security',     'Night',  'Main Gate',        'sec1',     'pass123')"
        };
        for (const QString& s : staff)
            q.exec("INSERT INTO staff(name,role,shift,ward,username,password) VALUES(" + s + ")");
        qDebug() << "Seeded staff:" << staff.size();
    }

    // ── Director ─────────────────────────────────────────────────────────
    chk.exec("SELECT COUNT(*) FROM director");
    if (!chk.next() || chk.value(0).toInt() == 0) {
        q.exec("INSERT INTO director(director_id,name,username,password,bank_balance) "
               "VALUES(1,'Noor Ul Huda Soomro','director','nhs2026',0)");
    }

    // ── Wards (8) ────────────────────────────────────────────────────────
    chk.exec("SELECT COUNT(*) FROM ward");
    if (!chk.next() || chk.value(0).toInt() == 0) {
        const QStringList wards = {
            "('General Ward',30,0)",     "('ICU',10,0)",
            "('Surgical Ward',20,0)",    "('Pediatrics Ward',15,0)",
            "('Cardiology Ward',12,0)",  "('Maternity Ward',18,0)",
            "('Orthopedics Ward',14,0)", "('Isolation Ward',8,0)"
        };
        for (const QString& w : wards)
            q.exec("INSERT INTO ward(name,capacity,occupied) VALUES(" + w + ")");
        qDebug() << "Seeded wards:" << wards.size();
    }

    return true;
}

