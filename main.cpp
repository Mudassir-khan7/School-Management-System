#include <iostream>
#include <iomanip>
#include <string>
#include "student.h"
#include "attendance.h"
#include "examination.h"

// ==========================================
// Extra Feature 1: Student Profile Card
// ==========================================
void displayStudentProfileCard(const StudentManager& studentMgr, 
                               const AttendanceManager& attendanceMgr, 
                               const ExaminationManager& examMgr) {
    UI::printSubHeader("Comprehensive Student Profile Card");

    std::string id = UI::getStringInput("Enter Student ID: ");
    const Student* s = studentMgr.findStudent(id);
    if (s == nullptr) {
        UI::printError("Student with ID '" + id + "' not found in the system.");
        return;
    }

    // Attendance data
    int present = 0, totalDays = 0;
    double attPct = attendanceMgr.calculateAttendancePercentage(id, present, totalDays);
    std::string attPctStr = (totalDays > 0) ? (std::to_string(attPct).substr(0, 5) + "%") : "N/A";
    std::string attStatus;
    if (totalDays == 0) {
        attStatus = "NO ATTENDANCE DATA";
    } else if (attPct >= 75.0) {
        attStatus = "REGULAR / ELIGIBLE (>= 75%)";
    } else {
        attStatus = "SHORTAGE / LOW ATTENDANCE (< 75%)";
    }

    // Exam data
    StudentAcademicSummary examSummary = examMgr.getStudentSummary(studentMgr, id);
    std::string examPctStr = (examSummary.subjectCount > 0) ? (std::to_string(examSummary.percentage).substr(0, 5) + "%") : "N/A";
    std::string academicStatus;
    if (examSummary.subjectCount == 0) {
        academicStatus = "NO EXAM RESULTS RECORDED";
    } else if (examSummary.hasFailedSubject || examSummary.percentage < 50.0) {
        academicStatus = "FAILED (NEEDS IMPROVEMENT)";
    } else if (examSummary.percentage >= 80.0) {
        academicStatus = "OUTSTANDING / PASSED WITH DISTINCTION";
    } else {
        academicStatus = "PASSED";
    }

    // Helper lambda to print formatted card row
    auto printCardRow = [](const std::string& text) {
        const size_t INNER_WIDTH = 68;
        std::cout << UI::CYAN << "║ " << UI::RESET;
        if (text.length() <= INNER_WIDTH) {
            std::cout << text << std::string(INNER_WIDTH - text.length(), ' ');
        } else {
            std::cout << text.substr(0, INNER_WIDTH);
        }
        std::cout << UI::CYAN << " ║\n" << UI::RESET;
    };

    // Formatted Card Layout
    std::cout << "\n\n";
    std::cout << UI::CYAN << UI::BOLD;
    std::cout << "╔══════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                     OFFICIAL STUDENT PROFILE CARD                    ║\n";
    std::cout << "║                      SCHOOL MANAGEMENT SYSTEM                        ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════╣\n" << UI::RESET;

    // Personal Details
    printCardRow("[1] PERSONAL DETAILS");
    printCardRow("  Student ID : " + s->getId() + std::string(20 - std::min<size_t>(20, s->getId().length()), ' ') + "Class : " + s->getStudentClass());
    printCardRow("  Full Name  : " + s->getName() + std::string(20 - std::min<size_t>(20, s->getName().length()), ' ') + "Age   : " + std::to_string(s->getAge()) + " years");
    std::cout << UI::CYAN << "╠══════════════════════════════════════════════════════════════════════╣\n" << UI::RESET;

    // Attendance Summary
    printCardRow("[2] ATTENDANCE SUMMARY");
    printCardRow("  Total Days : " + std::to_string(totalDays) + std::string(20 - std::min<size_t>(20, std::to_string(totalDays).length()), ' ') + "Present Days : " + std::to_string(present));
    printCardRow("  Absent Days: " + std::to_string(totalDays - present) + std::string(20 - std::min<size_t>(20, std::to_string(totalDays - present).length()), ' ') + "Attendance % : " + attPctStr);
    printCardRow("  Status     : " + attStatus);
    std::cout << UI::CYAN << "╠══════════════════════════════════════════════════════════════════════╣\n" << UI::RESET;

    // Examination Summary
    printCardRow("[3] EXAMINATION SUMMARY");
    std::string marksStr = std::to_string(static_cast<int>(examSummary.totalObtained)) + " / " + std::to_string(static_cast<int>(examSummary.totalMax));
    printCardRow("  Subjects   : " + std::to_string(examSummary.subjectCount) + std::string(20 - std::min<size_t>(20, std::to_string(examSummary.subjectCount).length()), ' ') + "Total Marks  : " + marksStr);
    printCardRow("  Percentage : " + examPctStr + std::string(20 - std::min<size_t>(20, examPctStr.length()), ' ') + "Overall Grade: " + examSummary.overallGrade);
    printCardRow("  Standing   : " + academicStatus);
    std::cout << UI::CYAN << UI::BOLD << "╚══════════════════════════════════════════════════════════════════════╝\n" << UI::RESET;
}

// ==========================================
// Sub-menu Handlers
// ==========================================
void runStudentMenu(StudentManager& studentMgr, AttendanceManager& attendanceMgr, ExaminationManager& examMgr) {
    while (true) {
        UI::printHeader("MODULE 1: STUDENT INFORMATION MANAGEMENT");
        std::cout << UI::WHITE
                  << "  [1] Add New Student\n"
                  << "  [2] Display All Students\n"
                  << "  [3] Search Student by ID\n"
                  << "  [4] Update Student Information\n"
                  << "  [5] Delete Student Record\n"
                  << "  [6] View Student Profile Card (All-in-One Summary)\n"
                  << "  [0] Back to Main Menu\n"
                  << UI::CYAN << "----------------------------------------------------------------------\n" << UI::RESET;

        int choice = UI::getIntInput("Enter your choice (0-6): ", 0, 6);
        if (choice == 0) break;

        switch (choice) {
            case 1:
                studentMgr.handleAddStudent();
                break;
            case 2:
                studentMgr.displayAllStudents();
                break;
            case 3:
                studentMgr.handleSearchStudent();
                break;
            case 4:
                studentMgr.handleUpdateStudent();
                break;
            case 5: {
                std::string deletedId;
                if (studentMgr.handleDeleteStudent(deletedId)) {
                    // Synchronize deletion across attendance and examination records
                    attendanceMgr.deleteRecordsForStudent(deletedId);
                    examMgr.deleteRecordsForStudent(deletedId);
                    UI::printInfo("Synchronized: All associated attendance and exam records for ID '" + deletedId + "' have been removed.");
                }
                break;
            }
            case 6:
                displayStudentProfileCard(studentMgr, attendanceMgr, examMgr);
                break;
            default:
                UI::printError("Invalid choice.");
                break;
        }
        UI::pausePrompt();
    }
}

void runAttendanceMenu(const StudentManager& studentMgr, AttendanceManager& attendanceMgr) {
    while (true) {
        UI::printHeader("MODULE 2: ATTENDANCE MANAGEMENT");
        std::cout << UI::WHITE
                  << "  [1] Mark Student Attendance\n"
                  << "  [2] View Attendance Records (All / By Student)\n"
                  << "  [3] Calculate Student Attendance Percentage\n"
                  << "  [0] Back to Main Menu\n"
                  << UI::CYAN << "----------------------------------------------------------------------\n" << UI::RESET;

        int choice = UI::getIntInput("Enter your choice (0-3): ", 0, 3);
        if (choice == 0) break;

        switch (choice) {
            case 1:
                attendanceMgr.handleMarkAttendance(studentMgr);
                break;
            case 2:
                attendanceMgr.handleViewAttendance(studentMgr);
                break;
            case 3:
                attendanceMgr.handleCalculatePercentage(studentMgr);
                break;
            default:
                UI::printError("Invalid choice.");
                break;
        }
        UI::pausePrompt();
    }
}

void runExaminationMenu(const StudentManager& studentMgr, ExaminationManager& examMgr) {
    while (true) {
        UI::printHeader("MODULE 3: EXAMINATION RESULTS MANAGEMENT");
        std::cout << UI::WHITE
                  << "  [1] Enter Examination Marks\n"
                  << "  [2] View Examination Results\n"
                  << "  [3] Generate Student Report Card\n"
                  << "  [4] Generate Class Merit List\n"
                  << "  [0] Back to Main Menu\n"
                  << UI::CYAN << "----------------------------------------------------------------------\n" << UI::RESET;

        int choice = UI::getIntInput("Enter your choice (0-4): ", 0, 4);
        if (choice == 0) break;

        switch (choice) {
            case 1:
                examMgr.handleEnterMarks(studentMgr);
                break;
            case 2:
                examMgr.handleViewResults(studentMgr);
                break;
            case 3:
                examMgr.handleGenerateReportCard(studentMgr);
                break;
            case 4:
                examMgr.handleClassMeritList(studentMgr);
                break;
            default:
                UI::printError("Invalid choice.");
                break;
        }
        UI::pausePrompt();
    }
}

// ==========================================
// Main Application Entry Point
// ==========================================
int main() {
    // Initialize ANSI and UTF-8 console output for Windows and cross-platform
    UI::initConsole();

    // Instantiate managers with their relative file storage paths
    StudentManager studentMgr("data/students.txt");
    AttendanceManager attendanceMgr("data/attendance.txt");
    ExaminationManager examMgr("data/results.txt");

    // Automatically load existing records at application startup
    studentMgr.loadFromFile();
    attendanceMgr.loadFromFile();
    examMgr.loadFromFile();

    while (true) {
        UI::printHeader("SCHOOL MANAGEMENT SYSTEM");
        std::cout << UI::WHITE
                  << "  [1] Student Information Management\n"
                  << "  [2] Attendance Management\n"
                  << "  [3] Examination Results Management\n"
                  << "  [0] Exit\n"
                  << UI::CYAN << "======================================================================\n" << UI::RESET;

        int choice = UI::getIntInput("Enter your selection (0-3): ", 0, 3);

        if (choice == 0) {
            std::cout << "\n" << UI::GREEN << UI::BOLD 
                      << "Thank you for using the School Management System. Goodbye!\n" 
                      << UI::RESET;
            break;
        }

        switch (choice) {
            case 1:
                runStudentMenu(studentMgr, attendanceMgr, examMgr);
                break;
            case 2:
                runAttendanceMenu(studentMgr, attendanceMgr);
                break;
            case 3:
                runExaminationMenu(studentMgr, examMgr);
                break;
            default:
                UI::printError("Invalid selection.");
                UI::pausePrompt();
                break;
        }
    }

    return 0;
}
