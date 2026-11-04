#pragma once
// ── Master include header for HospitalManagementSystem ──────────────────────

// Core OOP classes
#include "core/Patient.h"
#include "core/Doctor.h"
#include "core/Ward.h"
#include "core/Wardstaff.h"
#include "core/Reception.h"
#include "core/OPD.h"
#include "core/Operation.h"
#include "core/Test.h"
#include "core/Department.h"
#include "core/Departments.h"
#include "core/PrescriptionManager.h"

// Database layer
#include "db/DatabaseManager.h"

// AI integration
#include "ai/GeminiClient.h"
#include "ai/DoctorAgent.h"
