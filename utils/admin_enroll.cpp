#include "../database/DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

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
    cout << "=== Admin Enrollment Utility (Bypasses Registration Window) ===" << endl;
    cout << "Loading student data..." << endl;
    
    ifstream infile("students_data.txt");
   if (!infile.is_open()) {
        cerr << "ERROR: Could not open students_data.txt" << endl;
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
    
    cout << "Initializing database..." << endl;
    DatabaseManager db("./data");
    db.initialize();
    
    int successCount = 0;
    int failCount = 0;
    int totalEnrollments = 0;
    
    cout << "\nDirectly enrolling students (admin mode)..." << endl;
    cout << "----------------------------------------" << endl;
    
    for (const auto& line : studentData) {
        vector<string> parts = split(line, '|');
        
        if (parts.size() < 5) continue;
        
        string studentID = parts[0];
        string coursesStr = parts[4];
        
        if (coursesStr.empty()) continue;
        
        vector<string> courses;
        stringstream ss(coursesStr);
        string course;
        while (getline(ss, course, ',')) {
            courses.push_back(course);
        }
        
        // Load student
        Student student;
        if (!db.getStudent(studentID, student)) {
            cerr << "[SKIP] " << studentID << " - Student not found" << endl;
            failCount++;
            continue;
        }
        
        // Directly add courses to student record
        student.enrolledCourses = courses;
        
        // Update student
        if (!db.updateStudent(student)) {
            cerr << "[FAIL] " << studentID << " - Failed to update student" << endl;
            failCount++;
            continue;
        }
        
        // Update course enrollment counts
        int enrolled = 0;
        for (const auto& courseID : courses) {
            Course course;
            if (db.getCourse(courseID, course)) {
                // Add student to course if not already there
                auto it = find(course.enrolledStudents.begin(), course.enrolledStudents.end(), studentID);
                if (it == course.enrolledStudents.end()) {
                    course.enrolledStudents.push_back(studentID);
                    course.currentEnrollmentCount = course.enrolledStudents.size();
                    if (db.updateCourse(course)) {
                        enrolled++;
                    }
                } else {
                    enrolled++;  // Already enrolled
                }
            }
        }
        
        if (enrolled == courses.size()) {
            cout << "[OK] " << studentID << " - Enrolled in " << enrolled << " courses" << endl;
            successCount++;
            totalEnrollments += enrolled;
        } else {
            cout << "[PARTIAL] " << studentID << " - Enrolled in " << enrolled << "/" << courses.size() << " courses" << endl;
            failCount++;
            totalEnrollments += enrolled;
        }
    }
    
    cout << "----------------------------------------" << endl;
    cout << "Summary:" << endl;
    cout << "  Students fully enrolled: " << successCount << endl;
    cout << "  Students with errors: " << failCount << endl;
    cout << "  Total enrollments: " << totalEnrollments << endl;
    cout << "\nDone!" << endl;
    
    return 0;
}
