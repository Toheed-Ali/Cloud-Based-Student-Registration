#include "../database/DatabaseManager.h"
#include "../backend/utils/SHA256.h"
#include <iostream>
<parameter name="sstream">
#include <vector>
#include <string>
#include <fstream>

using namespace std;

// Helper function to split string by delimiter
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    cout << "=== Student Population Utility ===" << endl;
    cout << "Loading student data from file..." << endl;
    
    // Read student data from file
    ifstream infile("students_data.txt");
    if (!infile.is_open()) {
        cerr << "ERROR: Could not open students_data.txt" << endl;
        cerr << "Please create this file with student data in format:" << endl;
        cerr << "studentID|email|name|semester|courses|phone|timestamp" << endl;
        return 1;
    }
    
    vector<string> studentData;
    string line;
    while (getline(infile, line)) {
        if (!line.empty()) {
            studentData.push_back(line);
        }
    }
    infile.close();
    
    cout << "Found " << studentData.size() << " students in file" << endl;
    cout << "Initializing database..." << endl;
    
    DatabaseManager db("./data");
    db.initialize();
    
    int successCount = 0;
    int failCount = 0;
    
    cout << "\nAdding " << studentData.size() << " students..." << endl;
    cout << "----------------------------------------" << endl;
    
    for (const auto& line : studentData) {
        vector<string> parts = split(line, '|');
        
        if (parts.size() < 7) {
            cerr << "ERROR: Invalid student data (expected 7 fields): " << line << endl;
            failCount++;
            continue;
        }
        
        string studentID = parts[0];
        string email = parts[1];
        string name = parts[2];
        int semester = stoi(parts[3]);
        string coursesStr = parts[4];  // Comma-separated course IDs
        string phone = parts[5];
        time_t admission = stol(parts[6]);
        
        // Parse enrolled courses
        vector<string> courses;
        if (!coursesStr.empty()) {
            stringstream ss(coursesStr);
            string course;
            while (getline(ss, course, ',')) {
                courses.push_back(course);
            }
        }
        
        // Check if student already exists
        Student existingStudent;
        if (db.getStudent(studentID, existingStudent)) {
            cout << "[SKIP] " << studentID << " - Already exists" << endl;
            continue;
        }
        
        // Create User account for student
        User user;
        user.userID = studentID;
        user.email = email;
        // Default password is last 4 digits of student ID
        string password = studentID.substr(studentID.length() - 4);
        user.passwordHash = SHA256::hash(password);
        user.role = UserRole::STUDENT;
        user.name = name;
        
        // Add user account
        if (!db.createUser(user)) {
            cerr << "[FAIL] " << studentID << " - Failed to create user account" << endl;
            failCount++;
            continue;
        }
        
        // Create Student record
        Student student;
        student.studentID = studentID;
        student.email = email;
        student.name = name;
        student.currentSemester = semester;
        student.enrolledCourses = courses;
        student.contactInfo = phone;
        student.dateOfAdmission = admission;
        
        // Add student
        if (db.addStudent(student)) {
            cout << "[OK] " << studentID << " - " << name 
                 << " (Semester " << semester << ", " << courses.size() << " courses)" << endl;
            successCount++;
        } else {
            cerr << "[FAIL] " << studentID << " - Failed to add student record" << endl;
            failCount++;
        }
    }
    
    cout << "----------------------------------------" << endl;
    cout << "Summary:" << endl;
    cout << "  Successfully added: " << successCount << endl;
    cout << "  Failed: " << failCount << endl;
    cout << "  Total students: " << studentData.size() << endl;
    cout << "\nDefault password for all students: Last 4 digits of student ID" << endl;
    cout << "  Example: BSCS24119 -> password: 4119" << endl;
    cout << "\nDone!" << endl;
    
    return 0;
}
