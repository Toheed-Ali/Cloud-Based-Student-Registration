#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
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
    cout << "   Teacher Population Utility" << endl;
    cout << "========================================\n" << endl;
    
    // Initialize database
    DatabaseManager db("data");
    db.initialize();
    
    // Open data file
    ifstream dataFile("../utils/teachers_data.txt");
    if (!dataFile.is_open()) {
        cerr << "Error: Could not open teachers_data.txt" << endl;
        cerr << "Make sure the file exists in utils/ directory" << endl;
        return 1;
    }
    
    int added = 0;
    int skipped = 0;
    string line;
    
    cout << "Reading teacher data from file...\n" << endl;
    
    while (getline(dataFile, line)) {
        if (line.empty()) continue;
        
        // Parse line: TeacherID|Email|Name|AssignedCourse|Department|Contact
        vector<string> fields = split(line, '|');
        if (fields.size() < 6) {
            cerr << "Skipping invalid line: " << line << endl;
            continue;
        }
        
        string teacherID = fields[0];
        string email = fields[1];
        string name = fields[2];
        string assignedCourse = fields[3];
        string department = fields[4];
        string contact = fields[5];
        
        // Create User for authentication (password = last 4 digits of teacher ID)
        User user;
        user.userID = teacherID;
        user.email = email;
        user.name = name;
        user.role = UserRole::TEACHER;
        // Default password: last 4 digits of ID
        string defaultPassword = teacherID.substr(teacherID.length() - 4);
        user.passwordHash = SHA256::hash(defaultPassword);
        
        if (!db.createUser(user)) {
            cout << "✗ User exists: " << teacherID << endl;
            skipped++;
            continue;
        }
        
        // Create Teacher
        Teacher teacher;
        teacher.teacherID = teacherID;
        teacher.email = email;
        teacher.name = name;
        teacher.assignedCourses.push_back(assignedCourse);
        teacher.department = department;
        teacher.contactInfo = contact;
        
        if (db.addTeacher(teacher)) {
            cout << "✓ Added: " << teacherID << " - " << name << " (" << department << ")" << endl;
            added++;
        } else {
            cout << "✗ Failed to add teacher: " << teacherID << endl;
            skipped++;
        }
    }
    
    dataFile.close();
    
    cout << "\n========================================" << endl;
    cout << "Summary:" << endl;
    cout << "  Teachers added: " << added << endl;
    cout << "  Teachers skipped: " << skipped << endl;
    cout << "========================================" << endl;
    cout << "\nDefault password for all teachers: last 4 digits of Teacher ID" << endl;
    cout << "Example: T1100 → password: 1100" << endl;
    
    return 0;
}
