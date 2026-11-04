cmake_minimum_required(VERSION 3.16)

project(HospitalManagementSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network)

set(PROJECT_SOURCES
    main.cpp

    # ── Core OOP classes ──────────────────────────────────────────
    core/Patient.h
    core/Patient.cpp
    core/Doctor.h
    core/Doctor.cpp
    core/Ward.h
    core/Ward.cpp
    core/Wardstaff.h
    core/Wardstaff.cpp
    core/Reception.h
    core/Reception.cpp
    core/OPD.h
    core/OPD.cpp
    core/Operation.h
    core/Operation.cpp
    core/Test.h
    core/Test.cpp
    core/Department.h
    core/Department.cpp
    core/Departments.h
    core/Departments.cpp
    core/PrescriptionManager.h
    core/PrescriptionManager.cpp

    # ── Database layer ────────────────────────────────────────────
    db/DatabaseManager.h
    db/DatabaseManager_tables.cpp
    db/DatabaseManager_patients.cpp
    db/DatabaseManager_inventory.cpp
    db/DatabaseManager_misc.cpp

    # ── AI integration ────────────────────────────────────────────
    ai/GeminiClient.h
    ai/GeminiClient.cpp
    ai/DoctorAgent.h
    ai/DoctorAgent.cpp

    # ── GUI screens ───────────────────────────────────────────────
    gui/MainWindow.h
    gui/MainWindow.cpp
    gui/PatientPortal.h
    gui/PatientPortal.cpp
    gui/PatientRegister.h
    gui/PatientRegister.cpp
    gui/PatientLogin.h
    gui/PatientLogin.cpp
    gui/OPDScreen.h
    gui/OPDScreen.cpp
    gui/DoctorChatScreen.h
    gui/DoctorChatScreen.cpp
    gui/PatientActionsScreen.h
    gui/PatientActionsScreen.cpp
    gui/PharmacyScreen.h
    gui/PharmacyScreen.cpp
    gui/LabTestScreen.h
    gui/LabTestScreen.cpp
    gui/OperationScreen.h
    gui/OperationScreen.cpp
    gui/DischargeScreen.h
    gui/DischargeScreen.cpp
    gui/DirectorLogin.h
    gui/DirectorLogin.cpp
    gui/DirectorDashboard.h
    gui/DirectorDashboard.cpp
    gui/StaffLogin.h
    gui/StaffLogin.cpp
    gui/StaffPortal.h
    gui/StaffPortal.cpp

    # ── Qt Resources ─────────────────────────────────────────────
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HospitalManagementSystem
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(HospitalManagementSystem
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(HospitalManagementSystem PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
)

# Include directories so subdirectory headers resolve
target_include_directories(HospitalManagementSystem PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/db
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
)

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(HospitalManagementSystem)
endif()

# ── Post-build: copy runtime files to exe directory ──────────────────────────
add_custom_command(TARGET HospitalManagementSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/config.ini"
        "$<TARGET_FILE_DIR:HospitalManagementSystem>/config.ini"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:HospitalManagementSystem>/prescriptions"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:HospitalManagementSystem>/db"
    COMMENT "Copying runtime files to build directory..."
)
