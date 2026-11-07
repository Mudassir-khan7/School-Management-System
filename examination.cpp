#include "examination.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <filesystem>

// ==========================================
// ExamRecord Implementation
// ==========================================
ExamRecord::ExamRecord() : studentId(""), subject(""), obtainedMarks(0.0), totalMarks(100.0) {}

ExamRecord::ExamRecord(const std::string& studentId, const std::string& subject, double obtainedMarks, double totalMarks)
    : studentId(studentId), subject(subject), obtainedMarks(obtainedMarks), totalMarks(totalMarks) {}

std::string ExamRecord::getStudentId() const { return studentId; }
std::string ExamRecord::getSubject() const { return subject; }
double ExamRecord::getObtainedMarks() const { return obtainedMarks; }
double ExamRecord::getTotalMarks() const { return totalMarks; }

void ExamRecord::setObtainedMarks(double marks) { obtainedMarks = marks; }
void ExamRecord::setTotalMarks(double total) { totalMarks = total; }

double ExamRecord::getPercentage() const {
    if (totalMarks <= 0.0) return 0.0;
    return (obtainedMarks / totalMarks) * 100.0;
}

std::string ExamRecord::getGrade() const {
    return ExaminationManager::calculateGrade(getPercentage());
}

std::string ExamRecord::toFileString() const {
    std::ostringstream ss;
    ss << studentId << "|" << subject << "|" << obtainedMarks << "|" << totalMarks;
    return ss.str();
}

ExamRecord ExamRecord::fromFileString(const std::string& line) {
    std::stringstream ss(line);
    std::string id, subject, obtStr, totStr;
    if (std::getline(ss, id, '|') &&
        std::getline(ss, subject, '|') &&
        std::getline(ss, obtStr, '|') &&
        std::getline(ss, totStr)) {
        try {
            double obt = std::stod(obtStr);
            double tot = std::stod(totStr);
            return ExamRecord(id, subject, obt, tot);
        } catch (...) {
            // Parse error
        }
    }
    return ExamRecord();
}

// ==========================================
// ExaminationManager Implementation
// ==========================================
ExaminationManager::ExaminationManager(const std::string& filePath) : filePath(filePath) {}

std::string ExaminationManager::calculateGrade(double percentage) {
    if (percentage >= 90.0) return "A+";
    if (percentage >= 80.0) return "A";
    if (percentage >= 70.0) return "B";
    if (percentage >= 60.0) return "C";
    if (percentage >= 50.0) return "D";
    return "F";
}

bool ExaminationManager::loadFromFile() {
    records.clear();
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        return true; // Safe if file does not exist yet
    }

    std::string line;
    while (std::getline(inFile, line)) {
        line = UI::trim(line);
        if (line.empty()) continue;
        ExamRecord rec = ExamRecord::fromFileString(line);
        if (!rec.getStudentId().empty()) {
            records.push_back(rec);
        }
    }
    inFile.close();
    return true;
}

bool ExaminationManager::saveToFile() const {
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

bool ExaminationManager::recordExists(const std::string& studentId, const std::string& subject) const {
    for (const auto& rec : records) {
        if (rec.getStudentId() == studentId && rec.getSubject() == subject) {
            return true;
        }
    }
    return false;
}

bool ExaminationManager::addOrUpdateRecord(const std::string& studentId, const std::string& subject, double obtained, double total) {
    for (auto& rec : records) {
        if (rec.getStudentId() == studentId && rec.getSubject() == subject) {
            rec.setObtainedMarks(obtained);
            rec.setTotalMarks(total);
            saveToFile();
            return true;
        }
    }
    records.emplace_back(studentId, subject, obtained, total);
    saveToFile();
    return true;
}

std::vector<ExamRecord> ExaminationManager::getRecordsByStudent(const std::string& studentId) const {
    std::vector<ExamRecord> result;
    for (const auto& rec : records) {
        if (rec.getStudentId() == studentId) {
            result.push_back(rec);
        }
    }
    return result;
}

const std::vector<ExamRecord>& ExaminationManager::getAllRecords() const {
    return records;
}

bool ExaminationManager::calculateStudentStats(const std::string& studentId, double& totalObtained, double& totalMax, double& percentage, std::string& grade) const {
    totalObtained = 0.0;
    totalMax = 0.0;
    int count = 0;

    for (const auto& rec : records) {
        if (rec.getStudentId() == studentId) {
            totalObtained += rec.getObtainedMarks();
            totalMax += rec.getTotalMarks();
            count++;
        }
    }

    if (count == 0 || totalMax <= 0.0) {
        percentage = 0.0;
        grade = "N/A";
        return false;
    }

    percentage = (totalObtained / totalMax) * 100.0;
    grade = calculateGrade(percentage);
    return true;
}

StudentAcademicSummary ExaminationManager::getStudentSummary(const StudentManager& studentMgr, const std::string& studentId) const {
    StudentAcademicSummary summary;
    summary.studentId = studentId;

    const Student* s = studentMgr.findStudent(studentId);
    if (s != nullptr) {
        summary.name = s->getName();
        summary.studentClass = s->getStudentClass();
    } else {
        summary.name = "Unknown";
        summary.studentClass = "Unknown";
    }

    auto studentRecs = getRecordsByStudent(studentId);
    summary.subjectCount = static_cast<int>(studentRecs.size());

    for (const auto& rec : studentRecs) {
        summary.totalObtained += rec.getObtainedMarks();
        summary.totalMax += rec.getTotalMarks();
        if (rec.getPercentage() < 50.0) {
            summary.hasFailedSubject = true;
        }
    }

    if (summary.totalMax > 0.0) {
        summary.percentage = (summary.totalObtained / summary.totalMax) * 100.0;
        summary.overallGrade = calculateGrade(summary.percentage);
    } else {
        summary.percentage = 0.0;
        summary.overallGrade = "N/A";
    }

    return summary;
}

void ExaminationManager::deleteRecordsForStudent(const std::string& studentId) {
    auto it = std::remove_if(records.begin(), records.end(),
        [&studentId](const ExamRecord& rec) {
            return rec.getStudentId() == studentId;
        });

    if (it != records.end()) {
        records.erase(it, records.end());
        saveToFile();
    }
}

void ExaminationManager::handleEnterMarks(const StudentManager& studentMgr) {
    UI::printSubHeader("Enter Examination Marks");

    if (studentMgr.getAllStudents().empty()) {
        UI::printError("No registered students found! Register students first.");
        return;
    }

    std::string id = UI::getStringInput("Enter Student ID: ");
    const Student* s = studentMgr.findStudent(id);
    if (s == nullptr) {
        UI::printError("Student ID '" + id + "' not found! Examination marks can only be added for registered students.");
        return;
    }

    std::cout << UI::GREEN << "Student: " << s->getName() << " | Class: " << s->getStudentClass() << UI::RESET << "\n";

    std::string subject = UI::getStringInput("Enter Subject Name (e.g., Mathematics, Physics): ");

    if (recordExists(id, subject)) {
        UI::printWarning("A record already exists for subject '" + subject + "' for this student.");
        std::string overwrite = UI::getStringInput("Do you want to update/overwrite the marks? (Y/N): ");
        if (overwrite != "Y" && overwrite != "y") {
            UI::printInfo("Operation cancelled.");
            return;
        }
    }

    double totalMarks = UI::getDoubleInput("Enter Total Marks (e.g., 100): ", 1.0, 1000.0);
    double obtainedMarks = UI::getDoubleInput("Enter Obtained Marks (0 - " + std::to_string(static_cast<int>(totalMarks)) + "): ", 0.0, totalMarks);

    if (obtainedMarks > totalMarks) {
        UI::printError("Obtained marks cannot exceed total marks!");
        return;
    }

    if (addOrUpdateRecord(id, subject, obtainedMarks, totalMarks)) {
        double pct = (obtainedMarks / totalMarks) * 100.0;
        std::string grade = calculateGrade(pct);
        UI::printSuccess("Examination marks recorded successfully!");
        std::cout << UI::CYAN << "  Result: " << obtainedMarks << " / " << totalMarks
                  << " (" << std::fixed << std::setprecision(2) << pct << "%) | Grade: " << grade << "\n" << UI::RESET;
    } else {
        UI::printError("Failed to save examination marks.");
    }
}

void ExaminationManager::handleViewResults(const StudentManager& studentMgr) const {
    UI::printSubHeader("View Examination Results");

    std::cout << UI::YELLOW << "Options:\n"
              << " [1] View Results by Student ID\n"
              << " [2] View All Examination Records\n"
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
            UI::printWarning("No exam records found for " + s->getName() + " (" + id + ").");
            return;
        }

        std::cout << "\n" << UI::GREEN << UI::BOLD << "Examination Marks for: " 
                  << s->getName() << " (" << s->getId() << ") | Class: " << s->getStudentClass() << "\n" << UI::RESET;

        std::cout << UI::CYAN << "+--------------------------------+----------+----------+------------+-------+\n";
        std::cout << "| " << std::left << std::setw(30) << "Subject"
                  << " | " << std::right << std::setw(8) << "Obtained"
                  << " | " << std::right << std::setw(8) << "Total"
                  << " | " << std::right << std::setw(10) << "Percentage"
                  << " | " << std::setw(5) << "Grade" << " |\n";
        std::cout << "+--------------------------------+----------+----------+------------+-------+\n" << UI::RESET;

        for (const auto& rec : studentRecs) {
            std::cout << "| " << std::left << std::setw(30) << rec.getSubject()
                      << " | " << std::right << std::setw(8) << std::fixed << std::setprecision(1) << rec.getObtainedMarks()
                      << " | " << std::right << std::setw(8) << std::fixed << std::setprecision(1) << rec.getTotalMarks()
                      << " | " << std::right << std::setw(9) << std::fixed << std::setprecision(2) << rec.getPercentage() << "%"
                      << " | " << std::setw(5) << rec.getGrade() << " |\n";
        }
        std::cout << UI::CYAN << "+--------------------------------+----------+----------+------------+-------+\n" << UI::RESET;

        double totObt = 0, totMax = 0, overallPct = 0;
        std::string overallGrade;
        calculateStudentStats(id, totObt, totMax, overallPct, overallGrade);

        std::cout << UI::BOLD << "Total Obtained: " << UI::GREEN << std::fixed << std::setprecision(1) << totObt << UI::RESET
                  << " / " << totMax
                  << " | Overall Percentage: " << UI::YELLOW << std::fixed << std::setprecision(2) << overallPct << "%" << UI::RESET
                  << " | Overall Grade: " << UI::MAGENTA << overallGrade << UI::RESET << "\n";
    } else {
        if (records.empty()) {
            UI::printWarning("No examination records found in the system.");
            return;
        }

        std::cout << "\n" << UI::CYAN << UI::BOLD;
        std::cout << "+------------+---------------------------+-----------------------+----------+-------+-------+\n";
        std::cout << "| " << std::left << std::setw(10) << "Student ID"
                  << " | " << std::setw(25) << "Student Name"
                  << " | " << std::setw(21) << "Subject"
                  << " | " << std::right << std::setw(8) << "Obtained"
                  << " | " << std::right << std::setw(5) << "Total"
                  << " | " << std::setw(5) << "Grade" << " |\n";
        std::cout << "+------------+---------------------------+-----------------------+----------+-------+-------+\n" << UI::RESET;

        for (const auto& rec : records) {
            const Student* s = studentMgr.findStudent(rec.getStudentId());
            std::string name = (s != nullptr) ? s->getName() : "Unknown";

            std::cout << "| " << std::left << std::setw(10) << rec.getStudentId()
                      << " | " << std::setw(25) << name
                      << " | " << std::setw(21) << rec.getSubject()
                      << " | " << std::right << std::setw(8) << std::fixed << std::setprecision(1) << rec.getObtainedMarks()
                      << " | " << std::right << std::setw(5) << static_cast<int>(rec.getTotalMarks())
                      << " | " << std::setw(5) << rec.getGrade() << " |\n";
        }
        std::cout << UI::CYAN << "+------------+---------------------------+-----------------------+----------+-------+-------+\n" << UI::RESET;
        std::cout << UI::GRAY << "Total Subject Marks Entries: " << records.size() << UI::RESET << "\n";
    }
}

// ==========================================
// Extra Feature 2: Student Report Card
// ==========================================
void ExaminationManager::handleGenerateReportCard(const StudentManager& studentMgr) const {
    UI::printSubHeader("Generate Student Academic Report Card");

    std::string id = UI::getStringInput("Enter Student ID: ");
    const Student* s = studentMgr.findStudent(id);
    if (s == nullptr) {
        UI::printError("Student with ID '" + id + "' not found.");
        return;
    }

    auto studentRecs = getRecordsByStudent(id);
    if (studentRecs.empty()) {
        UI::printWarning("No examination marks recorded for student '" + s->getName() + "'. Report card cannot be generated.");
        return;
    }

    double totalObtained = 0.0;
    double totalMax = 0.0;
    bool hasFailedSubject = false;

    for (const auto& rec : studentRecs) {
        totalObtained += rec.getObtainedMarks();
        totalMax += rec.getTotalMarks();
        if (rec.getPercentage() < 50.0) {
            hasFailedSubject = true;
        }
    }

    double overallPct = (totalMax > 0.0) ? (totalObtained / totalMax) * 100.0 : 0.0;
    std::string overallGrade = calculateGrade(overallPct);
    std::string statusStr = (!hasFailedSubject && overallPct >= 50.0) ? (UI::GREEN + UI::BOLD + "PASS" + UI::RESET) : (UI::RED + UI::BOLD + "FAIL" + UI::RESET);

    // Formatted University-Level Report Card
    auto printReportRow = [](const std::string& text) {
        const size_t INNER_WIDTH = 68;
        std::cout << UI::CYAN << "║ " << UI::RESET;
        if (text.length() <= INNER_WIDTH) {
            std::cout << text << std::string(INNER_WIDTH - text.length(), ' ');
        } else {
            std::cout << text.substr(0, INNER_WIDTH);
        }
        std::cout << UI::CYAN << " ║\n" << UI::RESET;
    };

    std::cout << "\n\n";
    std::cout << UI::CYAN << UI::BOLD << "╔══════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                     ACADEMIC REPORT CARD                             ║\n";
    std::cout << "║                   SCHOOL MANAGEMENT SYSTEM                           ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════╣\n" << UI::RESET;

    printReportRow("Student ID   : " + s->getId() + std::string(18 - std::min<size_t>(18, s->getId().length()), ' ') + "Class : " + s->getStudentClass());
    printReportRow("Student Name : " + s->getName() + std::string(18 - std::min<size_t>(18, s->getName().length()), ' ') + "Age   : " + std::to_string(s->getAge()) + " years");

    std::cout << UI::CYAN << "╠══════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ " << std::left << std::setw(26) << "SUBJECT NAME"
              << " | " << std::right << std::setw(9) << "OBTAINED"
              << " | " << std::right << std::setw(7) << "TOTAL"
              << " | " << std::right << std::setw(9) << "PERCENT"
              << " | " << std::setw(5) << "GRADE" << " ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════╣\n" << UI::RESET;

    for (const auto& rec : studentRecs) {
        std::ostringstream lineStream;
        lineStream << std::left << std::setw(26) << (rec.getSubject().length() > 25 ? rec.getSubject().substr(0, 25) : rec.getSubject())
                   << " | " << std::right << std::setw(9) << std::fixed << std::setprecision(1) << rec.getObtainedMarks()
                   << " | " << std::right << std::setw(7) << static_cast<int>(rec.getTotalMarks())
                   << " | " << std::right << std::setw(8) << std::fixed << std::setprecision(1) << rec.getPercentage() << "%"
                   << " | " << std::setw(5) << rec.getGrade();
        printReportRow(lineStream.str());
    }

    std::cout << UI::CYAN << "╠══════════════════════════════════════════════════════════════════════╣\n" << UI::RESET;
    std::string passFailText = (!hasFailedSubject && overallPct >= 50.0) ? "PASS" : "FAIL";
    printReportRow("Total Marks        : " + std::to_string(static_cast<int>(totalObtained)) + " / " + std::to_string(static_cast<int>(totalMax)));
    printReportRow("Overall Percentage : " + (std::to_string(overallPct).substr(0, 5) + "%"));
    printReportRow("Overall Grade      : " + overallGrade);
    printReportRow("Academic Status    : " + passFailText);
    std::cout << UI::CYAN << UI::BOLD << "╚══════════════════════════════════════════════════════════════════════╝\n" << UI::RESET;
}

// ==========================================
// Extra Feature 3: Class Merit List
// ==========================================
void ExaminationManager::handleClassMeritList(const StudentManager& studentMgr) const {
    UI::printSubHeader("Class Merit List & Rankings");

    auto classes = studentMgr.getAllClasses();
    if (classes.empty()) {
        UI::printWarning("No classes found. Please enroll students first.");
        return;
    }

    std::cout << UI::YELLOW << "Available Classes:\n" << UI::RESET;
    for (size_t i = 0; i < classes.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << classes[i] << "\n";
    }
    std::cout << "  [0] All Classes (Combined Leaderboard)\n";

    int choice = UI::getIntInput("Select Class option: ", 0, static_cast<int>(classes.size()));

    std::vector<StudentAcademicSummary> summaries;
    std::string selectedClassName;

    if (choice == 0) {
        selectedClassName = "All Classes";
        for (const auto& s : studentMgr.getAllStudents()) {
            StudentAcademicSummary sum = getStudentSummary(studentMgr, s.getId());
            if (sum.subjectCount > 0) {
                summaries.push_back(sum);
            }
        }
    } else {
        selectedClassName = classes[choice - 1];
        auto studentsInClass = studentMgr.getStudentsByClass(selectedClassName);
        for (const auto& s : studentsInClass) {
            StudentAcademicSummary sum = getStudentSummary(studentMgr, s.getId());
            if (sum.subjectCount > 0) {
                summaries.push_back(sum);
            }
        }
    }

    if (summaries.empty()) {
        UI::printWarning("No examination results recorded for students in '" + selectedClassName + "'.");
        return;
    }

    // Sort descending by percentage, then by total obtained
    std::sort(summaries.begin(), summaries.end(), [](const StudentAcademicSummary& a, const StudentAcademicSummary& b) {
        if (std::abs(a.percentage - b.percentage) > 0.0001) {
            return a.percentage > b.percentage;
        }
        return a.totalObtained > b.totalObtained;
    });

    // Calculate ranks (handling equal percentages with the same rank)
    int currentRank = 1;
    for (size_t i = 0; i < summaries.size(); ++i) {
        if (i > 0) {
            if (std::abs(summaries[i].percentage - summaries[i - 1].percentage) > 0.0001) {
                currentRank = static_cast<int>(i + 1); // Standard competition ranking
            }
        }
        summaries[i].rank = currentRank;
    }

    // Display Merit List
    std::cout << "\n\n" << UI::CYAN << UI::BOLD;
    std::cout << "========================================================================================\n";
    std::cout << "                            OFFICIAL CLASS MERIT LIST                                   \n";
    std::cout << "                            Class: " << selectedClassName << "\n";
    std::cout << "========================================================================================\n";
    std::cout << "+------+------------+---------------------------+------------+------------+-------+----+\n";
    std::cout << "| " << std::left << std::setw(4) << "Rank"
              << " | " << std::setw(10) << "Student ID"
              << " | " << std::setw(25) << "Student Name"
              << " | " << std::setw(10) << "Class"
              << " | " << std::right << std::setw(10) << "Percentage"
              << " | " << std::setw(5) << "Grade"
              << " | " << std::setw(2) << "St" << " |\n";
    std::cout << "+------+------------+---------------------------+------------+------------+-------+----+\n" << UI::RESET;

    for (const auto& sm : summaries) {
        std::string rankStr = "#" + std::to_string(sm.rank);
        std::string rankColored;
        if (sm.rank == 1) {
            rankColored = UI::YELLOW + UI::BOLD + rankStr + UI::RESET;
        } else if (sm.rank == 2) {
            rankColored = UI::WHITE + UI::BOLD + rankStr + UI::RESET;
        } else if (sm.rank == 3) {
            rankColored = UI::MAGENTA + UI::BOLD + rankStr + UI::RESET;
        } else {
            rankColored = rankStr;
        }

        std::string statusBadge = (sm.hasFailedSubject || sm.percentage < 50.0) ? (UI::RED + "F" + UI::RESET) : (UI::GREEN + "P" + UI::RESET);

        std::cout << "| " << std::left << std::setw(13) << rankColored
                  << " | " << UI::YELLOW << std::setw(10) << sm.studentId << UI::RESET
                  << " | " << std::setw(25) << sm.name
                  << " | " << std::setw(10) << sm.studentClass
                  << " | " << std::right << std::setw(9) << std::fixed << std::setprecision(2) << sm.percentage << "%"
                  << " | " << std::setw(5) << sm.overallGrade
                  << " | " << std::setw(11) << statusBadge << " |\n";
    }

    std::cout << UI::CYAN << "+------+------------+---------------------------+------------+------------+-------+----+\n" << UI::RESET;
    std::cout << UI::GRAY << "Note: Students with equal percentage share the same merit position rank.\n"
              << "Status Indicator: P = All Passed, F = Needs Improvement/Failed in Subject\n" << UI::RESET;
}
