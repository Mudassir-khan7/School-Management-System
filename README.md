# School Management System in C++ (C++17)

[![C++17](https://img.shields.io/badge/Standard-C%2B%2B17-blue.svg)](https://isocpp.org/)
[![Compiler](https://img.shields.io/badge/Compiler-GCC%20%2F%20MinGW--w64-green.svg)](https://www.mingw-w64.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)]()
[![Zero Dependencies](https://img.shields.io/badge/Dependencies-Zero%20(Pure%20STL)-orange.svg)]()

A high-performance, modular **School Management System** built using **Standard C++ (C++17)**, object-oriented design principles, persistent flat-file storage, and an attractive ANSI-colored console interface.

Designed for an undergraduate university software engineering course demonstration and oral viva defense.

---

## 📌 Project Overview

The system manages the three core operational functions of an educational institution:
1. **Student Information Management** (Registration, profiles, search, update, cascading deletion).
2. **Student Attendance Management** (Daily attendance tracking, ISO date enforcement, same-day duplicate prevention).
3. **Examination Results Management** (Score ingestion, range validation, automatic letter grading, transcript generation).

### 🌟 Key Highlights & University Special Features
- **Student Profile Card:** An all-in-one console dashboard integrating student identity, attendance eligibility, and academic standing in a character-aligned box-border card.
- **Academic Report Card:** Official student transcript with subject-wise marks, individual grades, cumulative totals, aggregate percentages, and strict `PASS` / `FAIL` evaluation.
- **Class Merit List:** Automated ranked leaderboard by cohort or institution-wide, implementing standard competition ranking for students with equal percentages.
- **Cascading Referential Integrity:** Deleting a student automatically removes all associated attendance and examination records across all files, preventing orphaned data.
- **Crash-Proof Defensive Input:** Uses stream tokenization and `try-catch` numeric parsing to completely eliminate console stream crashes from invalid user inputs.

---

## 📂 Project Structure

```text
School-Management-System/
│
├── main.cpp                 # Main entry point, menu loops, and Profile Card view
├── student.h                # Declarations: Student, StudentManager, UI utilities
├── student.cpp              # Implementations: Student and StudentManager operations
├── attendance.h             # Declarations: AttendanceRecord, AttendanceManager
├── attendance.cpp           # Implementations: AttendanceRecord and AttendanceManager operations
├── examination.h            # Declarations: ExamRecord, StudentAcademicSummary, ExaminationManager
├── examination.cpp          # Implementations: ExamRecord, ExaminationManager, Report Card, Merit List
├── compile.bat              # Windows batch script to compile via GCC/MinGW
├── run.bat                  # Windows batch script to launch the executable
│
├── School_Management_System_Complete_Documentation.pdf # 38-page technical documentation & viva guide
├── School_Management_System_Presentation.pdf          # 18-slide university presentation deck (16:9)
│
└── data/                    # Persistent storage directory
    ├── students.txt         # Serialized student records (ID|Name|Age|Class)
    ├── attendance.txt       # Serialized attendance logs (ID|Date|Status)
    └── results.txt          # Serialized examination marks (ID|Subject|Obtained|Total)
```

---

## ⚙️ Compilation and Execution

### Prerequisites
- Compiler: GCC / MinGW-w64 supporting C++17 (e.g. `g++ 11+` or `16+`).
- OS: Windows 10/11 (with UTF-8 and ANSI Virtual Terminal Processing enabled automatically) or Linux/macOS.

### Option A: Using the Provided Batch Scripts (Windows)
- Double-click `compile.bat` to compile with `-std=c++17 -Wall -Wextra`.
- Double-click `run.bat` to start the program.

### Option B: Manual Command Line
```powershell
# Navigate to the project directory
cd School-Management-System

# Compile all translation units
g++ -std=c++17 -Wall -Wextra main.cpp student.cpp attendance.cpp examination.cpp -o SchoolManagementSystem.exe

# Run the executable
.\SchoolManagementSystem.exe
```

---

## 📊 Sample Console Interface

### 1. Student Profile Card
```text
╔══════════════════════════════════════════════════════════════════════╗
║                     OFFICIAL STUDENT PROFILE CARD                    ║
║                      SCHOOL MANAGEMENT SYSTEM                        ║
╠══════════════════════════════════════════════════════════════════════╣
║ [1] PERSONAL DETAILS                                                 ║
║   Student ID : S101                Class : CS-101                    ║
║   Full Name  : Alice Johnson       Age   : 19 years                  ║
╠══════════════════════════════════════════════════════════════════════╣
║ [2] ATTENDANCE SUMMARY                                               ║
║   Total Days : 2                   Present Days : 2                  ║
║   Absent Days: 0                   Attendance % : 100.0%             ║
║   Status     : REGULAR / ELIGIBLE (>= 75%)                           ║
╠══════════════════════════════════════════════════════════════════════╣
║ [3] EXAMINATION SUMMARY                                              ║
║   Subjects   : 2                   Total Marks  : 186 / 200          ║
║   Percentage : 93.00%              Overall Grade: A+                 ║
║   Standing   : OUTSTANDING / PASSED WITH DISTINCTION                 ║
╚══════════════════════════════════════════════════════════════════════╝
```

### 2. Academic Report Card
```text
╔══════════════════════════════════════════════════════════════════════╗
║                     ACADEMIC REPORT CARD                             ║
║                   SCHOOL MANAGEMENT SYSTEM                           ║
╠══════════════════════════════════════════════════════════════════════╣
║ Student ID   : S101              Class : CS-101                      ║
║ Student Name : Alice Johnson     Age   : 19 years                    ║
╠══════════════════════════════════════════════════════════════════════╣
║ SUBJECT NAME               |  OBTAINED |   TOTAL |   PERCENT | GRADE ║
╠══════════════════════════════════════════════════════════════════════╣
║ Programming                |      95.0 |     100 |     95.0% |    A+ ║
║ Data Structures            |      91.0 |     100 |     91.0% |    A+ ║
╠══════════════════════════════════════════════════════════════════════╣
║ Total Marks        : 186 / 200                                       ║
║ Overall Percentage : 93.00%                                          ║
║ Overall Grade      : A+                                              ║
║ Academic Status    : PASS                                            ║
╚══════════════════════════════════════════════════════════════════════╝
```

### 3. Class Merit List (Competition Tie-Handling)
```text
+------+------------+---------------------------+------------+------------+-------+----+
| Rank | Student ID | Student Name              | Class      | Percentage | Grade | St |
+------+------------+---------------------------+------------+------------+-------+----+
| #1   | S101       | Alice Johnson             | CS-101     |     93.00% |    A+ |  P |
| #2   | S102       | Bob Smith                 | CS-101     |     85.00% |     A |  P |
| #2   | S103       | Charlie Brown             | CS-101     |     85.00% |     A |  P |
+------+------------+---------------------------+------------+------------+-------+----+
Note: Students with equal percentage share the same merit position rank.
```

---

## 📑 Included PDF Documents

- `School_Management_System_Complete_Documentation.pdf`: Complete 38-page university documentation report, line-by-line function explanations, architecture diagrams, and 70 viva questions with answers.
- `School_Management_System_Presentation.pdf`: 18-slide widescreen (16:9) presentation deck for project defense.

---

## 👨‍💻 Author
- **GitHub:** [@Mudassir-khan7](https://github.com/Mudassir-khan7)
- **Course:** Object-Oriented Programming (OOP) & Data Structures
