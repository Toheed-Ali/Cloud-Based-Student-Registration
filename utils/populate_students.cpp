#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "../database/DatabaseManager.h"
#include "../database/DataModels.h"
#include "../backend/utils/SHA256.h"

using namespace std;

// Split string by delimiter
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
    cout << "========================================" << endl;
    cout << "   Student Population Utility" << endl;
    cout << "========================================\n" << endl;
    
    // Initialize database
    DatabaseManager db("data");
    db.initialize();
    
    // Open data file
    ifstream dataFile("../utils/students_data.txt");
    if (!dataFile.is_open()) {
        cerr << "Error: Could not open students_data.txt" << endl;
        cerr << "Make sure the file exists in utils/ directory" << endl;
        return 1;
    }
    
    int added = 0;
    int skipped = 0;
    int enrolled = 0;
    string line;
    
    cout << "Reading student data from file...\n" << endl;
    
    while (getline(dataFile, line)) {
        if (line.empty()) continue;
        
        // Parse line: StudentID|Email|Name|Semester|Courses|Contact|Timestamp
        vector<string> fields = split(line, '|');
        if (fields.size() < 6) {
            cerr << "Skipping invalid line: " << line << endl;
            continue;
        }
        
        string studentID = fields[0];
        string email = fields[1];        string name = fields[2];
        int semester = stoi(fields[3]);
        string coursesStr = fields[4];
        string contact = fields[5];
        
        // Parse enrolled courses
        vector<string> courses = split(coursesStr, ',');
        
        // Create User for authentication (password = last 4 digits of student ID)
        User user;
        user.userID = studentID;
        user.email = email;
        user.name = name;
        user.role = UserRole::STUDENT;
        // Default password: last 4 digits of ID
        string defaultPassword = studentID.substr(studentID.length() - 4);
        user.passwordHash = SHA256::hash(defaultPassword);
        
        if (!db.createUser(user)) {
            cout << "✗ User exists: " << studentID << endl;
            skipped++;
            continue;
        }
        
        // Create Student
        Student student;
        student.studentID = studentID;
        student.email = email;
        student.name = name;
        student.currentSemester = semester;
        student.contactInfo = contact;
        student.enrolledCourses = courses;
        student.dateOfAdmission = time(nullptr);
        
        if (db.addStudent(student)) {
            cout << "✓ Added: " << studentID << " - " << name << " (Sem " << semester << ")";
            cout << " → Pre-enrolled in " << courses.size() << " courses" << endl;
            added++;
            enrolled += courses.size();
        } else {
            cout << "✗ Failed to add student: " << studentID << endl;
            skipped++;
        }
    }
    
    dataFile.close();
    
    cout << "\n--- Processing Course Enrollments ---" << endl;
    cout << "Updating course enrollment lists...\n" << endl;
    
    // Now update all courses to include enrolled students
    vector<Student> allStudents = db.getAllStudents();
    map<string, vector<string>> courseEnrollments; // courseID -> list of studentIDs
    
    // Collect all enrollments
    for (const auto& student : allStudents) {
        for (const auto& courseID : student.enrolledCourses) {
            courseEnrollments[courseID].push_back(student.studentID);
        }
    }
    
    // Update each course
    int coursesUpdated = 0;
    for (const auto& pair : courseEnrollments) {
        string courseID = pair.first;
        const vector<string>& studentIDs = pair.second;
        
        Course course;
        if (db.getCourse(courseID, course)) {
            course.enrolledStudents = studentIDs;
            course.currentEnrollmentCount = studentIDs.size();
            
            if (db.updateCourse(course)) {
                cout << "✓ Updated " << courseID << ": " << studentIDs.size() << " students enrolled" << endl;
                coursesUpdated++;
            }
        }
    }
    
    cout << "\n========================================" << endl;
    cout << "Summary:" << endl;
    cout << "  Students added: " << added << endl;
    cout << "  Students skipped: " << skipped << endl;
    cout << "  Total enrollments: " << enrolled << endl;
    cout << "  Courses updated: " << coursesUpdated << endl;
    cout << "========================================" << endl;
    cout << "\nDefault password for all students: last 4 digits of Student ID" << endl;
    cout << "Example: BSCS24119 → password: 4119" << endl;
    
    return 0;
}
