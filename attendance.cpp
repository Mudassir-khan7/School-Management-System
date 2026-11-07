#include "attendance.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

// ==========================================
// AttendanceRecord Implementation
// ==========================================
AttendanceRecord::AttendanceRecord() : studentId(""), date(""), status("Absent") {}

AttendanceRecord::AttendanceRecord(const std::string& studentId, const std::string& date, const std::string& status)
    : studentId(studentId), date(date), status(status) {}

std::string AttendanceRecord::getStudentId() const { return studentId; }
std::string AttendanceRecord::getDate() const { return date; }
std::string AttendanceRecord::getStatus() const { return status; }
void AttendanceRecord::setStatus(const std::string& s) { status = s; }

std::string AttendanceRecord::toFileString() const {
    return studentId + "|" + date + "|" + status;
}

AttendanceRecord AttendanceRecord::fromFileString(const std::string& line) {
    std::stringstream ss(line);
    std::string id, date, status;
    if (std::getline(ss, id, '|') &&
        std::getline(ss, date, '|') &&
        std::getline(ss, status)) {
        return AttendanceRecord(id, date, status);
    }
    return AttendanceRecord();
}

// ==========================================
// AttendanceManager Implementation
// ==========================================
AttendanceManager::AttendanceManager(const std::string& filePath) : filePath(filePath) {}

bool AttendanceManager::loadFromFile() {
    records.clear();
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        return true; // File not created yet
    }

    std::string line;
    while (std::getline(inFile, line)) {
        line = UI::trim(line);
        if (line.empty()) continue;
        AttendanceRecord rec = AttendanceRecord::fromFileString(line);
        if (!rec.getStudentId().empty()) {
            records.push_back(rec);
        }
    }
    inFile.close();
    return true;
}

bool AttendanceManager::saveToFile() const {
    std::filesystem::path p(filePath);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }

    std::ofstream outFile(filePath, std::ios::trunc);
    if (!outFile.is_open()) {
        UI::printError("Failed to open file for writing: " + filePath);
        return false;
    }

    for (const auto& rec : records) {
        outFile << rec.toFileString() << "\n";
    }
    outFile.close();
    return true;
}

bool AttendanceManager::hasAttendance(const std::string& studentId, const std::string& date) const {
    for (const auto& rec : records) {
        if (rec.getStudentId() == studentId && rec.getDate() == date) {
            return true;
        }
    }
    return false;
}

bool AttendanceManager::markAttendance(const std::string& studentId, const std::string& date, const std::string& status) {
    if (hasAttendance(studentId, date)) {
        return false;
    }
    records.emplace_back(studentId, date, status);
    saveToFile();
    return true;
}

std::vector<AttendanceRecord> AttendanceManager::getRecordsByStudent(const std::string& studentId) const {
    std::vector<AttendanceRecord> result;
    for (const auto& rec : records) {
        if (rec.getStudentId() == studentId) {
            result.push_back(rec);
        }
    }
    return result;
}

const std::vector<AttendanceRecord>& AttendanceManager::getAllRecords() const {
    return records;
}

double AttendanceManager::calculateAttendancePercentage(const std::string& studentId, int& presentCount, int& totalDays) const {
    presentCount = 0;
    totalDays = 0;
    for (const auto& rec : records) {
        if (rec.getStudentId() == studentId) {
            totalDays++;
            if (rec.getStatus() == "Present") {
                presentCount++;
            }
        }
    }
    if (totalDays == 0) return 0.0;
    return (static_cast<double>(presentCount) / totalDays) * 100.0;
}

void AttendanceManager::deleteRecordsForStudent(const std::string& studentId) {
    auto it = std::remove_if(records.begin(), records.end(),
        [&studentId](const AttendanceRecord& rec) {
            return rec.getStudentId() == studentId;
        });

    if (it != records.end()) {
        records.erase(it, records.end());
        saveToFile();
    }
}

void AttendanceManager::handleMarkAttendance(const StudentManager& studentMgr) {
    UI::printSubHeader("Mark Student Attendance");

    if (studentMgr.getAllStudents().empty()) {
        UI::printError("No registered students found! Please add students first.");
        return;
    }

    std::string id = UI::getStringInput("Enter Student ID: ");
    const Student* s = studentMgr.findStudent(id);
    if (s == nullptr) {
        UI::printError("Student ID '" + id + "' not found! Attendance can only be marked for registered students.");
        return;
    }

    std::cout << UI::GREEN << "Student: " << s->getName() << " | Class: " << s->getStudentClass() << UI::RESET << "\n";

    std::string date = UI::getDateInput("Enter Date (YYYY-MM-DD): ");

    if (hasAttendance(id, date)) {
        UI::printError("Attendance already recorded for Student '" + id + "' on date '" + date + "'!");
        return;
    }

    std::string statusChoice;
    while (true) {
        statusChoice = UI::getStringInput("Mark Status (P = Present / A = Absent): ");
        std::transform(statusChoice.begin(), statusChoice.end(), statusChoice.begin(), ::toupper);
        if (statusChoice == "P" || statusChoice == "PRESENT") {
            statusChoice = "Present";
            break;
        } else if (statusChoice == "A" || statusChoice == "ABSENT") {
            statusChoice = "Absent";
            break;
        }
        UI::printError("Invalid choice! Enter 'P' for Present or 'A' for Absent.");
    }

    if (markAttendance(id, date, statusChoice)) {
        UI::printSuccess("Attendance recorded successfully for " + s->getName() + " on " + date + " as [" + statusChoice + "].");
    } else {
        UI::printError("Failed to record attendance.");
    }
}

void AttendanceManager::handleViewAttendance(const StudentManager& studentMgr) const {
    UI::printSubHeader("View Attendance Records");

    std::cout << UI::YELLOW << "Options:\n"
              << " [1] View Records by Student ID\n"
              << " [2] View All Attendance Records\n"
              << "Enter choice: " << UI::RESET;
    int choice = UI::getIntInput("", 1, 2);

    if (choice == 1) {
        std::string id = UI::getStringInput("Enter Student ID: ");
        const Student* s = studentMgr.findStudent(id);
        if (s == nullptr) {
            UI::printError("Student ID '" + id + "' does not exist.");
            return;
        }

        auto studentRecs = getRecordsByStudent(id);
        if (studentRecs.empty()) {
            UI::printWarning("No attendance records found for " + s->getName() + " (" + id + ").");
            return;
        }

        std::cout << "\n" << UI::GREEN << UI::BOLD << "Attendance Records for: " 
                  << s->getName() << " (" << s->getId() << ") | Class: " << s->getStudentClass() << "\n" << UI::RESET;

        std::cout << UI::CYAN << "+------------+--------------+--------------+\n";
        std::cout << "| " << std::left << std::setw(10) << "Date"
                  << " | " << std::setw(12) << "Status"
                  << " | " << std::setw(12) << "Indicator" << " |\n";
        std::cout << "+------------+--------------+--------------+\n" << UI::RESET;

        for (const auto& rec : studentRecs) {
            std::string indicator = (rec.getStatus() == "Present") ? (UI::GREEN + "✔ Present" + UI::RESET) : (UI::RED + "✘ Absent " + UI::RESET);
            std::cout << "| " << std::left << std::setw(10) << rec.getDate()
                      << " | " << std::setw(12) << rec.getStatus()
                      << " | " << std::setw(21) << indicator << " |\n";
        }
        std::cout << UI::CYAN << "+------------+--------------+--------------+\n" << UI::RESET;

        int present = 0, total = 0;
        double pct = calculateAttendancePercentage(id, present, total);
        std::cout << UI::BOLD << "Summary: " << UI::RESET 
                  << "Present: " << UI::GREEN << present << UI::RESET
                  << " | Absent: " << UI::RED << (total - present) << UI::RESET
                  << " | Total Days: " << total
                  << " | Attendance: " << UI::YELLOW << std::fixed << std::setprecision(2) << pct << "%\n" << UI::RESET;
    } else {
        if (records.empty()) {
            UI::printWarning("No attendance records logged in the system.");
            return;
        }

        std::cout << "\n" << UI::CYAN << UI::BOLD;
        std::cout << "+------------+---------------------------+------------+------------+\n";
        std::cout << "| " << std::left << std::setw(10) << "Student ID"
                  << " | " << std::setw(25) << "Student Name"
                  << " | " << std::setw(10) << "Date"
                  << " | " << std::setw(10) << "Status" << " |\n";
        std::cout << "+------------+---------------------------+------------+------------+\n" << UI::RESET;

        for (const auto& rec : records) {
            const Student* s = studentMgr.findStudent(rec.getStudentId());
            std::string name = (s != nullptr) ? s->getName() : "Unknown";
            std::string statusColored = (rec.getStatus() == "Present") ? (UI::GREEN + "Present" + UI::RESET) : (UI::RED + "Absent " + UI::RESET);

            std::cout << "| " << std::left << std::setw(10) << rec.getStudentId()
                      << " | " << std::setw(25) << name
                      << " | " << std::setw(10) << rec.getDate()
                      << " | " << std::setw(19) << statusColored << " |\n";
        }
        std::cout << UI::CYAN << "+------------+---------------------------+------------+------------+\n" << UI::RESET;
        std::cout << UI::GRAY << "Total Attendance Logs: " << records.size() << UI::RESET << "\n";
    }
}

void AttendanceManager::handleCalculatePercentage(const StudentManager& studentMgr) const {
    UI::printSubHeader("Calculate Attendance Percentage");
    std::string id = UI::getStringInput("Enter Student ID: ");

    const Student* s = studentMgr.findStudent(id);
    if (s == nullptr) {
        UI::printError("Student ID '" + id + "' not found.");
        return;
    }

    int present = 0, total = 0;
    double percentage = calculateAttendancePercentage(id, present, total);

    std::cout << "\n" << UI::CYAN << UI::BOLD << "============ Student Attendance Report ============\n" << UI::RESET;
    std::cout << " Student ID    : " << UI::YELLOW << s->getId() << UI::RESET << "\n";
    std::cout << " Student Name  : " << s->getName() << "\n";
    std::cout << " Class         : " << s->getStudentClass() << "\n";
    std::cout << " Total Classes : " << total << "\n";
    std::cout << " Days Present  : " << UI::GREEN << present << UI::RESET << "\n";
    std::cout << " Days Absent   : " << UI::RED << (total - present) << UI::RESET << "\n";
    std::cout << " Percentage    : ";
    if (total == 0) {
        std::cout << UI::GRAY << "N/A (No records)\n" << UI::RESET;
    } else {
        std::string color = (percentage >= 75.0) ? UI::GREEN : ((percentage >= 60.0) ? UI::YELLOW : UI::RED);
        std::cout << color << UI::BOLD << std::fixed << std::setprecision(2) << percentage << "%\n" << UI::RESET;
        if (percentage < 75.0) {
            UI::printWarning("Attendance is below 75% requirement!");
        }
    }
    std::cout << UI::CYAN << UI::BOLD << "===================================================\n" << UI::RESET;
}
