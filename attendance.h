#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>
#include "student.h"

// ==========================================
// Attendance Record Entity
// ==========================================
class AttendanceRecord {
private:
    std::string studentId;
    std::string date;     // YYYY-MM-DD
    std::string status;   // "Present" or "Absent"

public:
    AttendanceRecord();
    AttendanceRecord(const std::string& studentId, const std::string& date, const std::string& status);

    // Getters
    std::string getStudentId() const;
    std::string getDate() const;
    std::string getStatus() const;

    // Setters
    void setStatus(const std::string& status);

    // Serialization
    std::string toFileString() const;
    static AttendanceRecord fromFileString(const std::string& line);
};

// ==========================================
// Attendance Manager Class
// ==========================================
class AttendanceManager {
private:
    std::vector<AttendanceRecord> records;
    std::string filePath;

public:
    explicit AttendanceManager(const std::string& filePath = "data/attendance.txt");

    bool loadFromFile();
    bool saveToFile() const;

    bool markAttendance(const std::string& studentId, const std::string& date, const std::string& status);
    bool hasAttendance(const std::string& studentId, const std::string& date) const;

    std::vector<AttendanceRecord> getRecordsByStudent(const std::string& studentId) const;
    const std::vector<AttendanceRecord>& getAllRecords() const;

    double calculateAttendancePercentage(const std::string& studentId, int& presentCount, int& totalDays) const;
    void deleteRecordsForStudent(const std::string& studentId);

    // Sub-menu UI Handlers
    void handleMarkAttendance(const StudentManager& studentMgr);
    void handleViewAttendance(const StudentManager& studentMgr) const;
    void handleCalculatePercentage(const StudentManager& studentMgr) const;
};

#endif // ATTENDANCE_H
