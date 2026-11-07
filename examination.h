#ifndef EXAMINATION_H
#define EXAMINATION_H

#include <string>
#include <vector>
#include "student.h"

// ==========================================
// Examination Record Entity
// ==========================================
class ExamRecord {
private:
    std::string studentId;
    std::string subject;
    double obtainedMarks;
    double totalMarks;

public:
    ExamRecord();
    ExamRecord(const std::string& studentId, const std::string& subject, double obtainedMarks, double totalMarks);

    // Getters
    std::string getStudentId() const;
    std::string getSubject() const;
    double getObtainedMarks() const;
    double getTotalMarks() const;

    // Setters
    void setObtainedMarks(double marks);
    void setTotalMarks(double total);

    // Calculations
    double getPercentage() const;
    std::string getGrade() const;

    // Serialization
    std::string toFileString() const;
    static ExamRecord fromFileString(const std::string& line);
};

// ==========================================
// Student Exam Summary (For Rankings & Reports)
// ==========================================
struct StudentAcademicSummary {
    std::string studentId;
    std::string name;
    std::string studentClass;
    double totalObtained = 0.0;
    double totalMax = 0.0;
    double percentage = 0.0;
    std::string overallGrade = "N/A";
    bool hasFailedSubject = false;
    int subjectCount = 0;
    int rank = 0;
};

// ==========================================
// Examination Manager Class
// ==========================================
class ExaminationManager {
private:
    std::vector<ExamRecord> records;
    std::string filePath;

public:
    explicit ExaminationManager(const std::string& filePath = "data/results.txt");

    static std::string calculateGrade(double percentage);

    bool loadFromFile();
    bool saveToFile() const;

    bool addOrUpdateRecord(const std::string& studentId, const std::string& subject, double obtained, double total);
    bool recordExists(const std::string& studentId, const std::string& subject) const;

    std::vector<ExamRecord> getRecordsByStudent(const std::string& studentId) const;
    const std::vector<ExamRecord>& getAllRecords() const;

    bool calculateStudentStats(const std::string& studentId, double& totalObtained, double& totalMax, double& percentage, std::string& grade) const;
    StudentAcademicSummary getStudentSummary(const StudentManager& studentMgr, const std::string& studentId) const;

    void deleteRecordsForStudent(const std::string& studentId);

    // UI Handlers & Extra Features
    void handleEnterMarks(const StudentManager& studentMgr);
    void handleViewResults(const StudentManager& studentMgr) const;
    void handleGenerateReportCard(const StudentManager& studentMgr) const;
    void handleClassMeritList(const StudentManager& studentMgr) const;
};

#endif // EXAMINATION_H
