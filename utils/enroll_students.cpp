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
    cout << "=== Student Enrollment Utility ===" << endl;
    cout << "Loading student data from file..." << endl;
    
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
    
    cout << "\nEnrolling students in courses..." << endl;
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
        
        bool allSuccess = true;
        for (const auto& courseID : courses) {
            if (db.enrollStudent(studentID, courseID)) {
                totalEnrollments++;
            } else {
                allSuccess = false;
            }
        }
        
        if (allSuccess) {
            cout << "[OK] " << studentID << " - Enrolled in " << courses.size() << " courses" << endl;
            successCount++;
        } else {
            cout << "[PARTIAL] " << studentID << " - Some enrollments failed" << endl;
            failCount++;
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
