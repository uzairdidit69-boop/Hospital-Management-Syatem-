-- ============================================================
-- HMS Full Seed — Run in DB Browser → Execute SQL tab
-- Clears & repopulates: staff, pharmacy, lab_tests, operations,
--                        doctors, ward, director
-- ============================================================

-- ── Clear tables ─────────────────────────────────────────────
DELETE FROM staff;
DELETE FROM pharmacy;
DELETE FROM lab_tests;
DELETE FROM operations;
DELETE FROM doctors;
DELETE FROM ward;
DELETE FROM sqlite_sequence WHERE name IN
  ('staff','pharmacy','lab_tests','operations','doctors','ward');

-- ── DOCTORS (10) ─────────────────────────────────────────────
INSERT INTO doctors(name,department,specialty,available) VALUES
  ('Dr. Ayesha Khan',   'ENT',         'Ear Nose Throat Specialist',  1),
  ('Dr. Bilal Ahmed',   'ENT',         'Rhinology Specialist',         1),
  ('Dr. Sara Malik',    'Cardiology',  'Interventional Cardiologist', 1),
  ('Dr. Usman Qureshi', 'Cardiology',  'Electrophysiologist',          1),
  ('Dr. Nadia Hussain', 'Gynecology',  'Obstetrics Specialist',        1),
  ('Dr. Razia Sultana', 'Gynecology',  'Reproductive Medicine',        1),
  ('Dr. Kamran Ali',    'Orthopedics', 'Joint Replacement Surgeon',   1),
  ('Dr. Zafar Iqbal',   'Orthopedics', 'Spine Surgeon',                1),
  ('Dr. Hina Fatima',   'Pediatrics',  'Neonatology Specialist',       1),
  ('Dr. Tariq Mehmood', 'Pediatrics',  'Pediatric Cardiologist',       1);

-- ── STAFF (20) ───────────────────────────────────────────────
INSERT INTO staff(name,role,shift,ward,username,password) VALUES
  ('Fatima Noor',      'Nurse',        'Morning', 'General Ward',     'nurse1',    'pass123'),
  ('Zara Sheikh',      'Nurse',        'Night',   'Surgical Ward',    'nurse2',    'pass123'),
  ('Maria Akram',      'Nurse',        'Evening', 'Pediatrics Ward',  'nurse3',    'pass123'),
  ('Hassan Khan',      'Nurse',        'Morning', 'ICU',              'nurse4',    'pass123'),
  ('Sana Mirza',       'Nurse',        'Night',   'General Ward',     'nurse5',    'pass123'),
  ('Rabia Naz',        'Nurse',        'Morning', 'Cardiology Ward',  'nurse6',    'pass123'),
  ('Amna Bibi',        'Nurse',        'Evening', 'Maternity Ward',   'nurse7',    'pass123'),
  ('Kiran Akhtar',     'Nurse',        'Night',   'Orthopedics Ward', 'nurse8',    'pass123'),
  ('Ahmed Raza',       'Ward Boy',     'Evening', 'ICU',              'wardboy1',  'pass123'),
  ('Faisal Rehman',    'Ward Boy',     'Morning', 'Surgical Ward',    'wardboy2',  'pass123'),
  ('Usman Butt',       'Ward Boy',     'Night',   'General Ward',     'wardboy3',  'pass123'),
  ('Salman Malik',     'Ward Boy',     'Evening', 'Cardiology Ward',  'wardboy4',  'pass123'),
  ('Tariq Hassan',     'Technician',   'Morning', 'Lab',              'tech1',     'pass123'),
  ('Nazia Awan',       'Technician',   'Evening', 'Lab',              'tech2',     'pass123'),
  ('Bilal Saeed',      'Technician',   'Night',   'Radiology',        'tech3',     'pass123'),
  ('Omar Farooq',      'Receptionist', 'Morning', 'Reception',        'recept1',   'pass123'),
  ('Hira Baig',        'Receptionist', 'Evening', 'Reception',        'recept2',   'pass123'),
  ('Asad Khan',        'Pharmacist',   'Morning', 'Pharmacy',         'pharm1',    'pass123'),
  ('Sobia Rashid',     'Pharmacist',   'Evening', 'Pharmacy',         'pharm2',    'pass123'),
  ('Imran Javed',      'Security',     'Night',   'Main Gate',        'sec1',      'pass123');

-- ── PHARMACY / MEDICINES (50) ────────────────────────────────
INSERT INTO pharmacy(name,price_per_unit,stock) VALUES
  ('Panadol',               100,  500),
  ('Ponstan',                50,  400),
  ('Amoxicillin',            80,  600),
  ('Augmentin',             200,  300),
  ('Brufen',                 60,  450),
  ('Metformin',              40,  800),
  ('Atorvastatin',          120,  350),
  ('Omeprazole',             70,  500),
  ('Cetirizine',             30,  700),
  ('Azithromycin',          180,  250),
  ('Clopidogrel',           250,  200),
  ('Lisinopril',             90,  300),
  ('Aspirin',                20,  900),
  ('Dexamethasone',         150,  200),
  ('Metronidazole',          55,  600),
  ('Ciprofloxacin',         130,  400),
  ('Ranitidine',             45,  550),
  ('Salbutamol Inhaler',    350,  150),
  ('Insulin Glargine',      800,  100),
  ('Warfarin',              110,  180),
  ('Prednisolone',           90,  300),
  ('Tramadol',              160,  200),
  ('Diclofenac',             75,  450),
  ('Pantoprazole',           85,  500),
  ('Folic Acid',             25,  800),
  ('Amitriptyline',         140,  180),
  ('Clonazepam',            200,  150),
  ('Fluoxetine',            180,  200),
  ('Sertraline',            160,  220),
  ('Loratadine',             35,  600),
  ('Montelukast',           120,  300),
  ('Amlodipine',             75,  400),
  ('Bisoprolol',            110,  280),
  ('Carvedilol',            130,  220),
  ('Furosemide',             50,  500),
  ('Spironolactone',         80,  350),
  ('Levothyroxine',          60,  450),
  ('Glibenclamide',          35,  600),
  ('Sitagliptin',           350,  150),
  ('Esomeprazole',           90,  400),
  ('Clarithromycin',        220,  200),
  ('Doxycycline',           100,  350),
  ('Cefixime',              180,  300),
  ('Vancomycin',            800,   80),
  ('Morphine',              500,   60),
  ('Ondansetron',           120,  250),
  ('Domperidone',            40,  600),
  ('Vitamin D3',             30,  900),
  ('Calcium Carbonate',      25,  800),
  ('Zinc Sulphate',          20,  700);

-- ── LAB TESTS (30) ───────────────────────────────────────────
INSERT INTO lab_tests(name,price,available) VALUES
  ('Blood Complete Count',      1500, 1),
  ('Urine Analysis',             800, 1),
  ('X-Ray Chest',               2500, 1),
  ('MRI Brain',                15000, 1),
  ('ECG',                       2000, 1),
  ('Ultrasound Abdomen',        3500, 1),
  ('Blood Sugar Fasting',        500, 1),
  ('Liver Function Test',       3000, 1),
  ('Kidney Function Test',      2800, 1),
  ('Thyroid Profile',           4000, 1),
  ('CT Scan Head',             12000, 1),
  ('Hepatitis B Antigen',       1800, 1),
  ('Hepatitis C Antibody',      2000, 1),
  ('HbA1c',                     2500, 1),
  ('Lipid Profile',             2200, 1),
  ('Serum Creatinine',          1200, 1),
  ('Uric Acid',                 1100, 1),
  ('Stool Routine',              700, 1),
  ('Blood Culture',             4500, 1),
  ('Echocardiography',          8000, 1),
  ('Bone Marrow Biopsy',       20000, 1),
  ('Sputum Culture',            3000, 1),
  ('Prothrombin Time',          1800, 1),
  ('D-Dimer',                   3500, 1),
  ('Troponin I',                4000, 1),
  ('PSA Total',                 3500, 1),
  ('CA-125',                    5000, 1),
  ('HIV Screening',             2500, 1),
  ('Dengue NS1 Antigen',        2000, 1),
  ('Typhoid IgM',               1500, 1);

-- ── OPERATIONS (20) ──────────────────────────────────────────
INSERT INTO operations(name,price,available) VALUES
  ('Appendectomy',           80000, 1),
  ('Cholecystectomy',       120000, 1),
  ('Brain Surgery',         500000, 1),
  ('Knee Replacement',      350000, 1),
  ('Cataract Surgery',       60000, 1),
  ('Tonsillectomy',          40000, 1),
  ('Hernia Repair',          75000, 1),
  ('Coronary Bypass',       600000, 1),
  ('Hip Replacement',       400000, 1),
  ('Thyroidectomy',         150000, 1),
  ('C-Section',             180000, 1),
  ('Spinal Fusion',         450000, 1),
  ('Prostatectomy',         300000, 1),
  ('Hysterectomy',          250000, 1),
  ('Kidney Transplant',     900000, 1),
  ('Liver Transplant',     1200000, 1),
  ('Gastric Bypass',        400000, 1),
  ('Pacemaker Implant',     550000, 1),
  ('LASIK Eye Surgery',     120000, 1),
  ('Rhinoplasty',           200000, 1);

-- ── WARDS (8) ────────────────────────────────────────────────
INSERT INTO ward(name,capacity,occupied) VALUES
  ('General Ward',       30, 0),
  ('ICU',                10, 0),
  ('Surgical Ward',      20, 0),
  ('Pediatrics Ward',    15, 0),
  ('Cardiology Ward',    12, 0),
  ('Maternity Ward',     18, 0),
  ('Orthopedics Ward',   14, 0),
  ('Isolation Ward',      8, 0);

-- ── DIRECTOR ─────────────────────────────────────────────────
INSERT OR IGNORE INTO director(director_id,name,username,password,bank_balance)
  VALUES(1,'Noor Ul Huda Soomro','director','nhs2026',0);

-- ── VERIFY ───────────────────────────────────────────────────
SELECT 'doctors'    AS [Table], COUNT(*) AS Records FROM doctors    UNION ALL
SELECT 'staff',                 COUNT(*)             FROM staff      UNION ALL
SELECT 'pharmacy',              COUNT(*)             FROM pharmacy   UNION ALL
SELECT 'lab_tests',             COUNT(*)             FROM lab_tests  UNION ALL
SELECT 'operations',            COUNT(*)             FROM operations UNION ALL
SELECT 'ward',                  COUNT(*)             FROM ward;
