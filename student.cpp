#include "student.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

// ==========================================
// Student Implementation
// ==========================================
Student::Student() : id(""), name(""), age(0), studentClass("") {}

Student::Student(const std::string& id, const std::string& name, int age, const std::string& studentClass)
    : id(id), name(name), age(age), studentClass(studentClass) {}

std::string Student::getId() const { return id; }
std::string Student::getName() const { return name; }
int Student::getAge() const { return age; }
std::string Student::getStudentClass() const { return studentClass; }

void Student::setName(const std::string& n) { name = n; }
void Student::setAge(int a) { age = a; }
void Student::setStudentClass(const std::string& c) { studentClass = c; }

std::string Student::toFileString() const {
    return id + "|" + name + "|" + std::to_string(age) + "|" + studentClass;
}

Student Student::fromFileString(const std::string& line) {
    std::stringstream ss(line);
    std::string id, name, ageStr, studentClass;

    if (std::getline(ss, id, '|') &&
        std::getline(ss, name, '|') &&
        std::getline(ss, ageStr, '|') &&
        std::getline(ss, studentClass)) {
        try {
            int age = std::stoi(ageStr);
            return Student(id, name, age, studentClass);
        } catch (...) {
            // Failed conversion
        }
    }
    return Student();
}

// ==========================================
// StudentManager Implementation
// ==========================================
StudentManager::StudentManager(const std::string& filePath) : filePath(filePath) {}

bool StudentManager::loadFromFile() {
    students.clear();
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        // File doesn't exist yet, which is safe for first run
        return true;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        line = UI::trim(line);
        if (line.empty()) continue;
        Student s = Student::fromFileString(line);
        if (!s.getId().empty()) {
            students.push_back(s);
        }
    }
    inFile.close();
    return true;
}

bool StudentManager::saveToFile() const {
    // Ensure parent directory exists
    std::filesystem::path p(filePath);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }

    std::ofstream outFile(filePath, std::ios::trunc);
    if (!outFile.is_open()) {
        UI::printError("Failed to open file for writing: " + filePath);
        return false;
    }

    for (const auto& s : students) {
        outFile << s.toFileString() << "\n";
    }
    outFile.close();
    return true;
}

bool StudentManager::studentExists(const std::string& id) const {
    return findStudent(id) != nullptr;
}

const Student* StudentManager::findStudent(const std::string& id) const {
    for (const auto& s : students) {
        if (s.getId() == id) {
            return &s;
        }
    }
    return nullptr;
}

Student* StudentManager::findStudent(const std::string& id) {
    for (auto& s : students) {
        if (s.getId() == id) {
            return &s;
        }
    }
    return nullptr;
}

bool StudentManager::addStudent(const Student& student) {
    if (studentExists(student.getId())) {
        return false;
    }
    students.push_back(student);
    saveToFile();
    return true;
}

bool StudentManager::updateStudent(const std::string& id, const std::string& newName, int newAge, const std::string& newClass) {
    Student* s = findStudent(id);
    if (s == nullptr) return false;

    if (!newName.empty()) s->setName(newName);
    if (newAge > 0) s->setAge(newAge);
    if (!newClass.empty()) s->setStudentClass(newClass);

    saveToFile();
    return true;
}

bool StudentManager::deleteStudent(const std::string& id) {
    auto it = std::remove_if(students.begin(), students.end(),
        [&id](const Student& s) { return s.getId() == id; });

    if (it != students.end()) {
        students.erase(it, students.end());
        saveToFile();
        return true;
    }
    return false;
}

const std::vector<Student>& StudentManager::getAllStudents() const {
    return students;
}

std::vector<Student> StudentManager::getStudentsByClass(const std::string& className) const {
    std::vector<Student> result;
    for (const auto& s : students) {
        if (s.getStudentClass() == className) {
            result.push_back(s);
        }
    }
    return result;
}

std::vector<std::string> StudentManager::getAllClasses() const {
    std::vector<std::string> classes;
    for (const auto& s : students) {
        if (std::find(classes.begin(), classes.end(), s.getStudentClass()) == classes.end()) {
            classes.push_back(s.getStudentClass());
        }
    }
    std::sort(classes.begin(), classes.end());
    return classes;
}

void StudentManager::displayAllStudents() const {
    UI::printSubHeader("All Enrolled Students");
    if (students.empty()) {
        UI::printWarning("No student records found in the system.");
        return;
    }

    std::cout << UI::BOLD << UI::CYAN;
    std::cout << "+------------+--------------------------------+-----+----------------+\n";
    std::cout << "| " << std::left << std::setw(10) << "Student ID"
              << " | " << std::setw(30) << "Full Name"
              << " | " << std::setw(3)  << "Age"
              << " | " << std::setw(14) << "Class" << " |\n";
    std::cout << "+------------+--------------------------------+-----+----------------+\n" << UI::RESET;

    for (const auto& s : students) {
        std::cout << "| " << UI::YELLOW << std::left << std::setw(10) << s.getId() << UI::RESET
                  << " | " << std::left << std::setw(30) << s.getName()
                  << " | " << std::right << std::setw(3) << s.getAge() << std::left
                  << " | " << std::setw(14) << s.getStudentClass() << " |\n";
    }

    std::cout << UI::CYAN << "+------------+--------------------------------+-----+----------------+\n" << UI::RESET;
    std::cout << UI::GRAY << "Total Students: " << students.size() << UI::RESET << "\n";
}

void StudentManager::handleAddStudent() {
    UI::printSubHeader("Register New Student");

    std::string id = UI::getStringInput("Enter Student ID (e.g., S101): ");
    if (studentExists(id)) {
        UI::printError("Student ID '" + id + "' already exists! Duplicate IDs are not allowed.");
        return;
    }

    std::string name = UI::getStringInput("Enter Student Full Name: ");
    int age = UI::getIntInput("Enter Age (4 - 100): ", 4, 100);
    std::string studentClass = UI::getStringInput("Enter Class / Grade (e.g., CS-101, Grade-10): ");

    Student newStudent(id, name, age, studentClass);
    if (addStudent(newStudent)) {
        UI::printSuccess("Student registered successfully!");
    } else {
        UI::printError("Failed to add student.");
    }
}

void StudentManager::handleSearchStudent() const {
    UI::printSubHeader("Search Student by ID");
    std::string id = UI::getStringInput("Enter Student ID to search: ");

    const Student* s = findStudent(id);
    if (s == nullptr) {
        UI::printError("No student found with ID '" + id + "'.");
        return;
    }

    std::cout << "\n" << UI::GREEN << UI::BOLD << "================ Student Details ================\n" << UI::RESET;
    std::cout << UI::CYAN << "  Student ID  : " << UI::RESET << s->getId() << "\n";
    std::cout << UI::CYAN << "  Full Name   : " << UI::RESET << s->getName() << "\n";
    std::cout << UI::CYAN << "  Age         : " << UI::RESET << s->getAge() << " years\n";
    std::cout << UI::CYAN << "  Class       : " << UI::RESET << s->getStudentClass() << "\n";
    std::cout << UI::GREEN << UI::BOLD << "=================================================\n" << UI::RESET;
}

void StudentManager::handleUpdateStudent() {
    UI::printSubHeader("Update Student Information");
    std::string id = UI::getStringInput("Enter Student ID to update: ");

    Student* s = findStudent(id);
    if (s == nullptr) {
        UI::printError("Student with ID '" + id + "' does not exist.");
        return;
    }

    std::cout << UI::GRAY << "Current details: " << s->getName() << " | Age: " 
              << s->getAge() << " | Class: " << s->getStudentClass() << UI::RESET << "\n";
    std::cout << UI::YELLOW << "Leave field empty or press Enter to keep current value.\n" << UI::RESET;

    std::string newName = UI::getStringInput("Enter New Name [" + s->getName() + "]: ", true);
    if (newName.empty()) newName = s->getName();

    std::cout << UI::YELLOW << "Enter New Age [" << s->getAge() << "] (or 0 to keep): " << UI::RESET;
    std::string ageStr;
    std::getline(std::cin, ageStr);
    ageStr = UI::trim(ageStr);
    int newAge = s->getAge();
    if (!ageStr.empty()) {
        try {
            int parsed = std::stoi(ageStr);
            if (parsed >= 4 && parsed <= 100) {
                newAge = parsed;
            } else {
                UI::printWarning("Age out of range (4-100). Keeping previous age.");
            }
        } catch (...) {
            UI::printWarning("Invalid number. Keeping previous age.");
        }
    }

    std::string newClass = UI::getStringInput("Enter New Class [" + s->getStudentClass() + "]: ", true);
    if (newClass.empty()) newClass = s->getStudentClass();

    if (updateStudent(id, newName, newAge, newClass)) {
        UI::printSuccess("Student record updated successfully!");
    } else {
        UI::printError("Failed to update student record.");
    }
}

bool StudentManager::handleDeleteStudent(std::string& deletedId) {
    UI::printSubHeader("Delete Student Record");
    std::string id = UI::getStringInput("Enter Student ID to delete: ");

    const Student* s = findStudent(id);
    if (s == nullptr) {
        UI::printError("Student with ID '" + id + "' does not exist.");
        return false;
    }

    std::cout << UI::RED << "Are you sure you want to delete student '" << s->getName() 
              << "' (" << s->getId() << ")?\nThis will also remove all associated attendance and exam records! (Y/N): " 
              << UI::RESET;
    std::string confirm;
    std::getline(std::cin, confirm);
    confirm = UI::trim(confirm);

    if (confirm == "Y" || confirm == "y") {
        if (deleteStudent(id)) {
            deletedId = id;
            UI::printSuccess("Student record deleted successfully from system.");
            return true;
        } else {
            UI::printError("Failed to delete student record.");
        }
    } else {
        UI::printInfo("Deletion cancelled by user.");
    }
    return false;
}
