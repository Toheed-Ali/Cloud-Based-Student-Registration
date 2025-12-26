#include <iostream>
#include <string>
#include <iomanip>
#include "../database/DatabaseManager.h"
#include "../backend/HTTPServer.h"
#include "../backend/AuthService.h"
#include "../backend/AdminService.h"
#include "../backend/TimetableGenerator.h"
#include "APIClient.h"

using namespace std;

void displayMenu() {
    cout << "\n==================================" << endl;
    cout << "      ADMIN DASHBOARD" << endl;
    cout << "==================================" << endl;
    cout << "1. Add Student" << endl;
    cout << "2. Add Teacher" << endl;
    cout << "3. Add Course" << endl;
    cout << "4. Set Registration Window" << endl;
    cout << "5. View All Students" << endl;
    cout << "6. View All Teachers" << endl;
    cout << "7. Generate Timetable" << endl;
    cout << "8. Logout" << endl;
    cout << "==================================" << endl;
    cout << "Choice: ";
}

void addStudent(APIClient& client) {
    string id, email, name, contact, password;
    int semester;
    
    cout << "\n--- Add Student ---" << endl;
    cout << "Student ID: "; cin >> id;
    cout << "Email: "; cin >> email;
    cin.ignore();
    cout << "Full Name: "; getline(cin, name);
    cout << "Semester (1-8): "; cin >> semester;
    cout << "Contact: "; cin >> contact;
    cout << "Password: "; cin >> password;
    
    if (client.adminAddStudent(id, email, name, semester, contact, password)) {
        cout << "\n✓ Student added successfully!" << endl;
    } else {
        cout << "\n✗ Failed to add student." << endl;
    }
}

void addTeacher(APIClient& client) {
    string id, email, name, courseID, dept, contact, password;
    
    cout << "\n--- Add Teacher ---" << endl;
    cout << "Teacher ID: "; cin >> id;
    cout << "Email: "; cin >> email;
    cin.ignore();
    cout << "Full Name: "; getline(cin, name);
    cout << "Assigned Course ID: "; cin >> courseID;
    cin.ignore();
    cout << "Department: "; getline(cin, dept);
    cout << "Contact: "; cin >> contact;
    cout << "Password: "; cin >> password;
    
    if (client.adminAddTeacher(id, email, name, courseID, dept, contact, password)) {
        cout << "\n✓ Teacher added successfully!" << endl;
    } else {
        cout << "\n✗ Failed to add teacher." << endl;
    }
}

void addCourse(APIClient& client) {
    string id, name, teacherID;
    int semester;
    
    cout << "\n--- Add Course ---" << endl;
    cout << "Course ID: "; cin >> id;
    cin.ignore();
    cout << "Course Name: "; getline(cin, name);
    cout << "Semester (1-8): "; cin >> semester;
    cout << "Teacher ID: "; cin >> teacherID;
    
    if (client.adminAddCourse(id, name, semester, teacherID)) {
        cout << "\n✓ Course added successfully!" << endl;
    } else {
        cout << "\n✗ Failed to add course." << endl;
    }
}

void setRegistrationWindow(APIClient& client) {
    cout << "\n--- Set Course Add/Drop Period ---" << endl;
    
    string startDate, startTime, endDate, endTime;
    int isOpen;
    
    cout << "Start Date (YYYY-MM-DD): ";
    cin >> startDate;
    cout << "Start Time (HH:MM): ";
    cin >> startTime;
    
    cout << "End Date (YYYY-MM-DD): ";
    cin >> endDate;
    cout << "End Time (HH:MM): ";
    cin >> endTime;
    
    cout << "Enable Registration (1=Yes, 0=No): ";
    cin >> isOpen;
    
    // Parse dates and times to create timestamps
    struct tm tmStart = {}, tmEnd = {};
    
    // Parse start date/time (YYYY-MM-DD HH:MM)
    sscanf(startDate.c_str(), "%d-%d-%d", &tmStart.tm_year, &tmStart.tm_mon, &tmStart.tm_mday);
    sscanf(startTime.c_str(), "%d:%d", &tmStart.tm_hour, &tmStart.tm_min);
    tmStart.tm_year -= 1900;  // years since 1900
    tmStart.tm_mon -= 1;       // months since January (0-11)
    tmStart.tm_sec = 0;
    
    // Parse end date/time
    sscanf(endDate.c_str(), "%d-%d-%d", &tmEnd.tm_year, &tmEnd.tm_mon, &tmEnd.tm_mday);
    sscanf(endTime.c_str(), "%d:%d", &tmEnd.tm_hour, &tmEnd.tm_min);
    tmEnd.tm_year -= 1900;
    tmEnd.tm_mon -= 1;
    tmEnd.tm_sec = 59;  // End of minute
    
    time_t startTimestamp = mktime(&tmStart);
    time_t endTimestamp = mktime(&tmEnd);
    
    if (startTimestamp == -1 || endTimestamp == -1) {
        cout << "\n✗ Invalid date/time format!" << endl;
        return;
    }
    
    if (startTimestamp >= endTimestamp) {
        cout << "\n✗ Start time must be before end time!" << endl;
        return;
    }
    
    if (client.adminSetRegistrationWindow(startTimestamp, endTimestamp, isOpen == 1)) {
        cout << "\n✓ Registration window configured successfully!" << endl;
        cout << "Period: " << startDate << " " << startTime << " to " << endDate << " " << endTime << endl;
        cout << "Status: " << (isOpen ? "OPEN" : "CLOSED") << endl;
    } else {
        cout << "\n✗ Failed to set registration window" << endl;
    }
}

void viewAllStudents(APIClient& client) {
    cout << "\n--- All Students ---" << endl;
    vector<Student> students = client.adminViewAllStudents();
    
    if (students.empty()) {
        cout << "No students found." << endl;
        return;
    }
    
    cout << left << setw(12) << "ID" 
         << setw(25) << "Name" 
         << setw(25) << "Email" 
         << setw(10) << "Semester" 
         << "Courses" << endl;
    cout << string(85, '-') << endl;
    
    for (const auto& s : students) {
        cout << left << setw(12) << s.studentID
             << setw(25) << s.name
             << setw(25) << s.email
             << setw(10) << s.currentSemester
             << s.enrolledCourses.size() << endl;
    }
}

void viewAllTeachers(APIClient& client) {
    cout << "\n--- All Teachers ---" << endl;
    vector<Teacher> teachers = client.adminViewAllTeachers();
    
    if (teachers.empty()) {
        cout << "No teachers found." << endl;
        return;
    }
    
    cout << left << setw(12) << "ID" 
         << setw(25) << "Name" 
         << setw(25) << "Email" 
         << setw(15) << "Department" 
         << "Course" << endl;
    cout << string(90, '-') << endl;
    
    for (const auto& t : teachers) {
        cout << left << setw(12) << t.teacherID
             << setw(25) << t.name
             << setw(25) << t.email
             << setw(15) << t.department
             << t.assignedCourseID << endl;
    }
}

void generateTimetable(APIClient& client) {
    cout << "\n--- Generate Timetable ---" << endl;
    cout << "This will generate conflict-free timetables for all semesters." << endl;
    cout << "Continue? (y/n): ";
    
    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        cout << "\nGenerating timetables..." << endl;
        if (client.adminGenerateTimetable()) {
            cout << "✓ Timetables generated successfully!" << endl;
        } else {
            cout << "✗ Failed to generate timetables." << endl;
        }
    }
}

int main() {
    cout << "========================================" << endl;
    cout << "  University Management System" << endl;
    cout << "       ADMIN DASHBOARD" << endl;
    cout << "========================================\n" << endl;
    
    // Initialize backend
    DatabaseManager db("data");
    db.initialize();
    
    HTTPServer server(8080, db);
    server.post("/api/login", AuthService::login);
    server.post("/api/admin/addStudent", AdminService::addStudent);
    server.post("/api/admin/addTeacher", AdminService::addTeacher);
    server.post("/api/admin/addCourse", AdminService::addCourse);
    server.post("/api/admin/setRegistrationWindow", AdminService::setRegistrationWindow);
    server.post("/api/admin/generateTimetable", TimetableGenerator::generateTimetableAPI);
    server.get("/api/admin/viewAllStudents", AdminService::viewAllStudents);
    
    APIClient client(db, server);
    
    // Login
    string email, password;
    cout << "Email: "; cin >> email;
    cout << "Password: "; cin >> password;
    
    string role, userID, name;
    if (!client.login(email, password, role, userID, name)) {
        cout << "\n✗ Invalid credentials!" << endl;
        return 1;
    }
    
    if (role != "ADMIN") {
        cout << "\n✗ Access denied. Admin credentials required!" << endl;
        return 1;
    }
    
    cout << "\n✓ Logged in as: " << name << " (" << role << ")\n" << endl;
    
    // Main loop
    while (true) {
        displayMenu();
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: addStudent(client); break;
            case 2: addTeacher(client); break;
            case 3: addCourse(client); break;
            case 4: setRegistrationWindow(client); break;
            case 5: viewAllStudents(client); break;
            case 6: viewAllTeachers(client); break;
            case 7: generateTimetable(client); break;
            case 8:
                cout << "\nLogging out..." << endl;
                return 0;
            default:
                cout << "\nInvalid choice!" << endl;
        }
    }
    
    return 0;
}
