# Hospital Management System — Build & Run Guide

**Project Owner:** Noor Ul Huda Soomro  
**Language:** C++ 17 | **GUI:** Qt 6 | **Database:** SQLite3 via Qt | **AI:** Gemini 2.0 Flash

---

## 📋 Prerequisites

| Tool | Version | Download |
|------|---------|----------|
| Qt   | 6.x     | https://www.qt.io/download |
| CMake| ≥ 3.16  | Bundled with Qt Creator |
| MinGW or MSVC | Any 64-bit | Bundled with Qt installer |

---

## 🚀 Building in Qt Creator (Recommended)

1. **Open Qt Creator**
2. **File → Open File or Project** → select `CMakeLists.txt`
3. Qt Creator will auto-configure the project
4. Click **Configure Project** and choose your kit (MinGW 64-bit or MSVC)
5. Press **Build** (Ctrl+B)
6. Press **Run** (Ctrl+R)

> The database (`db/hospital.db`) is created automatically on first run with all seed data.

---

## ⚙️ Gemini AI Setup

1. Get a free API key from: https://aistudio.google.com/app/apikey
2. Open `config.ini` (in the build output directory, same folder as the `.exe`)
3. Replace `YOUR_GEMINI_API_KEY_HERE` with your key:
```ini
[Gemini]
api_key=AIzaSy...your_key_here
```

> Without the API key the AI doctor chat will show an error, but all other features work normally.

---

## 📁 Project Structure

```
HospitalManagementSystem/
├── CMakeLists.txt          ← Open this in Qt Creator
├── main.cpp
├── hospital.h
├── core/                   ← OOP base classes
├── db/                     ← Database layer
│   └── hospital.db         ← Auto-created on first run
├── ai/                     ← Gemini AI integration
├── gui/                    ← All Qt screens
├── assets/                 ← Background images + logo
├── prescriptions/          ← Generated at runtime
├── tests/                  ← Test files
└── config.ini              ← Gemini API key
```

---

## 🔑 Default Credentials

| Role     | Username   | Password  |
|----------|-----------|-----------|
| Director | `director` | `nhs2026` |
| Nurse    | `nurse1`   | `pass123` |
| Nurse    | `nurse2`   | `pass123` |
| Nurse    | `nurse3`   | `pass123` |
| Ward Boy | `wardboy1` | `pass123` |
| Tech     | `tech1`    | `pass123` |

---

## 🧪 Running Tests

The tests are companion `.cpp` files in `tests/`. To run them:

1. Call `runDatabaseTests()` from `main.cpp` before `app.exec()`
2. Call `runGeminiTests()` after setting the API key
3. Call `runGuiTests()` with a running `QApplication`

> Comment out test calls before submitting the final build.

---

## 📌 Known Notes

- **Paths with spaces:** The project path may contain spaces (Windows). CMake handles this automatically.
- **First run:** All 13 database tables are created and seeded automatically.
- **Prescriptions:** Files are saved to `<exe_dir>/prescriptions/P####D####.txt`
- **config.ini:** Must be in the same directory as the compiled `.exe`

---

*Hospital Management System v1.0 — Noor Ul Huda Soomro Memorial Hospital*
