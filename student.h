#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

// ==========================================
// ANSI Color Palette & UI Utilities
// ==========================================
namespace UI {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";
    const std::string RED     = "\033[91m";
    const std::string GREEN   = "\033[92m";
    const std::string YELLOW  = "\033[93m";
    const std::string BLUE    = "\033[94m";
    const std::string MAGENTA = "\033[95m";
    const std::string CYAN    = "\033[96m";
    const std::string WHITE   = "\033[97m";
    const std::string GRAY    = "\033[90m";

    // Setup Windows console for ANSI escape sequences and UTF-8 output
    inline void initConsole() {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    inline void clearScreen() {
#ifdef _WIN32
        std::cout << "\033[2J\033[H";
#else
        std::cout << "\033[2J\033[H";
#endif
    }

    inline void printHeader(const std::string& title) {
        std::cout << "\n" << CYAN << BOLD;
        std::cout << "======================================================================\n";
        int totalWidth = 70;
        int padding = (totalWidth - static_cast<int>(title.length())) / 2;
        if (padding < 0) padding = 0;
        std::cout << std::string(padding, ' ') << title << "\n";
        std::cout << "======================================================================\n" << RESET;
    }

    inline void printSubHeader(const std::string& subtitle) {
        std::cout << "\n" << BLUE << BOLD << "--- " << subtitle << " ---\n" << RESET;
    }

    inline void printSuccess(const std::string& msg) {
        std::cout << GREEN << BOLD << "[SUCCESS] " << RESET << GREEN << msg << RESET << "\n";
    }

    inline void printError(const std::string& msg) {
        std::cout << RED << BOLD << "[ERROR] " << RESET << RED << msg << RESET << "\n";
    }

    inline void printWarning(const std::string& msg) {
        std::cout << YELLOW << BOLD << "[WARNING] " << RESET << YELLOW << msg << RESET << "\n";
    }

    inline void printInfo(const std::string& msg) {
        std::cout << CYAN << "[INFO] " << RESET << msg << "\n";
    }

    inline void pausePrompt() {
        std::cout << "\n" << GRAY << "Press Enter to continue..." << RESET;
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    // Input Helpers
    inline std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    inline std::string getStringInput(const std::string& prompt, bool allowEmpty = false) {
        std::string input;
        while (true) {
            std::cout << YELLOW << prompt << RESET;
            std::getline(std::cin, input);
            input = trim(input);
            if (!allowEmpty && input.empty()) {
                printError("Input cannot be empty. Please try again.");
                continue;
            }
            // Check for delimiter '|' which would corrupt text file format
            if (input.find('|') != std::string::npos) {
                printError("Character '|' is reserved. Please avoid using it.");
                continue;
            }
            return input;
        }
    }

    inline int getIntInput(const std::string& prompt, int minVal = 0, int maxVal = 1000) {
        while (true) {
            std::cout << YELLOW << prompt << RESET;
            std::string line;
            std::getline(std::cin, line);
            line = trim(line);
            if (line.empty()) {
                printError("Input cannot be empty.");
                continue;
            }
            try {
                size_t idx = 0;
                int val = std::stoi(line, &idx);
                if (idx != line.length()) {
                    printError("Please enter a valid whole number.");
                    continue;
                }
                if (val < minVal || val > maxVal) {
                    printError("Value must be between " + std::to_string(minVal) + " and " + std::to_string(maxVal) + ".");
                    continue;
                }
                return val;
            } catch (...) {
                printError("Invalid numeric input. Please try again.");
            }
        }
    }

    inline double getDoubleInput(const std::string& prompt, double minVal = 0.0, double maxVal = 1000.0) {
        while (true) {
            std::cout << YELLOW << prompt << RESET;
            std::string line;
            std::getline(std::cin, line);
            line = trim(line);
            if (line.empty()) {
                printError("Input cannot be empty.");
                continue;
            }
            try {
                size_t idx = 0;
                double val = std::stod(line, &idx);
                if (idx != line.length()) {
                    printError("Please enter a valid numeric value.");
                    continue;
                }
                if (val < minVal || val > maxVal) {
                    std::ostringstream ss;
                    ss << "Value must be between " << minVal << " and " << maxVal << ".";
                    printError(ss.str());
                    continue;
                }
                return val;
            } catch (...) {
                printError("Invalid numeric input. Please try again.");
            }
        }
    }

    inline std::string getDateInput(const std::string& prompt) {
        while (true) {
            std::string date = getStringInput(prompt);
            // Format check: YYYY-MM-DD
            if (date.length() == 10 && date[4] == '-' && date[7] == '-') {
                bool validDigits = true;
                for (int i = 0; i < 10; ++i) {
                    if (i == 4 || i == 7) continue;
                    if (!std::isdigit(static_cast<unsigned char>(date[i]))) {
                        validDigits = false;
                        break;
                    }
                }
                if (validDigits) {
                    int year = std::stoi(date.substr(0, 4));
                    int month = std::stoi(date.substr(5, 2));
                    int day = std::stoi(date.substr(8, 2));
                    if (year >= 2000 && year <= 2099 && month >= 1 && month <= 12 && day >= 1 && day <= 31) {
                        return date;
                    }
                }
            }
            printError("Invalid date format! Please enter in YYYY-MM-DD format (e.g., 2026-11-01).");
        }
    }
}

// ==========================================
// Student Entity Class
// ==========================================
class Student {
private:
    std::string id;
    std::string name;
    int age;
    std::string studentClass;

public:
    Student();
    Student(const std::string& id, const std::string& name, int age, const std::string& studentClass);

    // Getters
    std::string getId() const;
    std::string getName() const;
    int getAge() const;
    std::string getStudentClass() const;

    // Setters
    void setName(const std::string& name);
    void setAge(int age);
    void setStudentClass(const std::string& studentClass);

    // Serialization
    std::string toFileString() const;
    static Student fromFileString(const std::string& line);
};

// ==========================================
// Student Manager Class
// ==========================================
class StudentManager {
private:
    std::vector<Student> students;
    std::string filePath;

public:
    explicit StudentManager(const std::string& filePath = "data/students.txt");

    bool loadFromFile();
    bool saveToFile() const;

    bool addStudent(const Student& student);
    bool studentExists(const std::string& id) const;
    const Student* findStudent(const std::string& id) const;
    Student* findStudent(const std::string& id);
    bool updateStudent(const std::string& id, const std::string& newName, int newAge, const std::string& newClass);
    bool deleteStudent(const std::string& id);

    const std::vector<Student>& getAllStudents() const;
    std::vector<Student> getStudentsByClass(const std::string& className) const;
    std::vector<std::string> getAllClasses() const;

    // UI Handlers
    void displayAllStudents() const;
    void handleAddStudent();
    void handleSearchStudent() const;
    void handleUpdateStudent();
    bool handleDeleteStudent(std::string& deletedId);
};

#endif // STUDENT_H
