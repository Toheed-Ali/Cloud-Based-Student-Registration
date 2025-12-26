#include <iostream>
#include <string>
#include <iomanip>
#include "../database/DatabaseManager.h"
#include "../backend/HTTPServer.h"
#include "../backend/AuthService.h"
#include "../backend/StudentService.h"
#include "../backend/TimetableGenerator.h"
#include "APIClient.h"

using namespace std;

void displayMenu() {
    cout << "\n==================================" << endl;
    cout << "      STUDENT PORTAL" << endl;
    cout << "==================================" << endl;
    cout << "1. View Available Courses" << endl;
    cout << "2. Enroll in Course" << endl;
    cout << "3. Drop Course" << endl;
    cout << "4. View My Enrolled Courses" << endl;
    cout << "5. View My Timetable" << endl;
    cout << "6. Logout" << endl;
    cout << "==================================" << endl;
    cout << "Choice: ";
}

void viewCourses(APIClient& client, int semester) {
    cout << "\n--- Available Courses (Semester " << semester << ") ---" << endl;
    vector<Course> courses = client.studentViewCourses(semester);
    
    if (courses.empty()) {
        cout << "No courses available." << endl;
        return;
    }
    
    // Check if registration is currently open
    SystemConfig config = client.getDB().getConfig();
    time_t now = time(nullptr);
    bool registrationOpen = config.isRegistrationOpen && 
                           (now >= config.registrationStartTime && now <= config.registrationEndTime);
    
    cout << left << setw(10) << "ID" 
         << setw(35) << "Name" 
         << setw(15) << "Teacher" 
         << setw(12) << "Enrolled" 
         << "Status" << endl;
    cout << string(75, '-') << endl;
    
    for (const auto& c : courses) {
        string status;
        if (!registrationOpen) {
            status = "CLOSED";  // Registration window closed
        } else if (c.currentEnrollmentCount >= 50) {
            status = "FULL";    // Course at capacity
        } else {
            status = "OPEN";    // Available for enrollment
        }
        
        cout << left << setw(10) << c.courseID
             << setw(35) << c.courseName
             << setw(15) << c.teacherID
             << setw(12) << (to_string(c.currentEnrollmentCount) + "/50")
             << status << endl;
    }
}

void viewEnrolledCourses(APIClient& client, const string& studentID) {
    Student student;
    if (!client.getStudent(studentID, student)) {
        cout << "\n✗ Student not found!" << endl;
        return;
    }
    
    cout << "\n--- My Enrolled Courses ---" << endl;
    
    if (student.enrolledCourses.empty()) {
        cout << "You are not enrolled in any courses yet." << endl;
        return;
    }
    
    cout << left << setw(10) << "Course ID" 
         << setw(35) << "Course Name" 
         << "Status" << endl;
    cout << string(60, '-') << endl;
    
    for (const auto& courseID : student.enrolledCourses) {
        Course course;
        if (client.getCourse(courseID, course)) {
            cout << left << setw(10) << courseID
                 << setw(35) << course.courseName
                 << "ENROLLED" << endl;
        }
    }
    
    cout << "\nTotal: " << student.enrolledCourses.size() << "/5 courses" << endl;
}

void enrollCourse(APIClient& client, const string& studentID) {
    cout << "\n--- Enroll in Course ---" << endl;
    
    // First, check current enrollment status
    Student student;
    if (!client.getStudent(studentID, student)) {
        cout << "✗ Failed to retrieve student information!" << endl;
        return;
    }
    
    // Check if already at max courses
    if (student.enrolledCourses.size() >= 5) {
        cout << "✗ You are already enrolled in the maximum 5 courses!" << endl;
        return;
    }
    
    string courseID;
    cout << "Course ID: ";
    cin >> courseID;
    
    // Check if already enrolled
    if (find(student.enrolledCourses.begin(), student.enrolledCourses.end(), courseID) 
        != student.enrolledCourses.end()) {
        cout << "\n⚠ You are already enrolled in course " << courseID << "!" << endl;
        return;
    }
    
    if (client.studentEnrollCourse(studentID, courseID)) {
        cout << "\n✓ Successfully enrolled in course!" << endl;
    } else {
        cout << "\n✗ Enrollment failed!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "- Course not found or not for your semester" << endl;
        cout << "- Course is full" << endl;
        cout << "- Registration window is closed (contact admin)" << endl;
    }
}

void dropCourse(APIClient& client, const string& studentID) {
    cout << "\n--- Drop Course ---" << endl;
    
    // Show enrolled courses first
    Student student;
    if (!client.getStudent(studentID, student)) {
        cout << "✗ Failed to retrieve student information!" << endl;
        return;
    }
    
    if (student.enrolledCourses.empty()) {
        cout << "You are not enrolled in any courses." << endl;
        return;
    }
    
    cout << "\nYour Enrolled Courses:" << endl;
    cout << string(40, '-') << endl;
    for (const auto& courseID : student.enrolledCourses) {
        Course course;
        if (client.getCourse(courseID, course)) {
            cout << "  " << courseID << " - " << course.courseName << endl;
        }
    }
    cout << string(40, '-') << endl;
    
    string courseID;
    cout << "\nCourse ID to drop: ";
    cin >> courseID;
    
    if (client.studentDropCourse(studentID, courseID)) {
        cout << "\n✓ Successfully dropped course!" << endl;
    } else {
        cout << "\n✗ Failed to drop course!" << endl;
    }
}

void viewTimetable(APIClient& client, const string& studentID) {
    Student student;
    if (!client.getStudent(studentID, student)) {
        cout << "\n✗ Student not found!" << endl;
        return;
    }
    
    Timetable timetable;
    if (!client.studentViewTimetable(student.currentSemester, timetable) || timetable.schedule.empty()) {
        cout << "\nTimetable not generated yet. Contact admin." << endl;
        return;
    }
    
    cout << "\n========================================" << endl;
    cout << "   MY TIMETABLE - SEMESTER " << student.currentSemester << endl;
    cout << "========================================" << endl;
    
    // Build a 2D grid: [day][hour] -> course info with room
    string grid[5][5];  // 5 days × 5 time slots
    for (int d = 0; d < 5; d++) {
        for (int h = 0; h < 5; h++) {
            grid[d][h] = "-";
        }
    }
    
    // Fill grid with enrolled courses
    for (const auto& sc : timetable.schedule) {
        // Check if student is enrolled
        bool enrolled = false;
        for (const auto& courseID : student.enrolledCourses) {
            if (courseID == sc.courseID) {
                enrolled = true;
                break;
            }
        }
        
        if (enrolled) {
            // Place course with room number in all its time slots
            string courseInfo = sc.courseID + "(R" + to_string(sc.classroomID) + ")";
            for (const auto& slot : sc.slots) {
                grid[slot.day][slot.hour] = courseInfo;
            }
        }
    }
    
    // Display grid
    const string days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    const string times[] = {"9:00-10:30", "10:30-12:00", "LUNCH", "13:00-14:30", "14:30-16:00", "16:00-17:30"};
    
    // Header
    cout << string(89, '=') << endl;
    cout << left << setw(14) << "Time";
    for (int d = 0; d < 5; d++) {
        cout << setw(15) << days[d];
    }
    cout << endl;
    cout << string(89, '=') << endl;
    
    // Rows for each time slot
    for (int h = 0; h < 5; h++) {
        // Time column
        if (h == 2) {
            // Insert lunch break row before slot 2
            cout << left << setw(14) << "12:00-13:00";
            for (int d = 0; d < 5; d++) {
                cout << setw(15) << "LUNCH";
            }
            cout << endl;
        }
        
        cout << left << setw(14) << times[h < 2 ? h : h + 1];  // Skip lunch in times array
        
        // Course columns
        for (int d = 0; d < 5; d++) {
            cout << setw(15) << grid[d][h];
        }
        cout << endl;
    }
    
    cout << string(89, '=') << endl;
    cout << "Total Courses Enrolled: " << student.enrolledCourses.size() << "/5" << endl;
    cout << string(89, '=') << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "  University Management System" << endl;
    cout << "       STUDENT PORTAL" << endl;
    cout << "========================================\n" << endl;
    
    // Initialize backend
    DatabaseManager db("data");
    db.initialize();
    
    HTTPServer server(8080, db);
    server.post("/api/login", AuthService::login);
    server.post("/api/student/enrollCourse", StudentService::enrollCourse);
    server.post("/api/student/dropCourse", StudentService::dropCourse);
    
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
    
    if (role != "STUDENT") {
        cout << "\n✗ Access denied. Student credentials required!" << endl;
        return 1;
    }
    
    cout << "\n✓ Logged in as: " << name << " (" << userID << ")\n" << endl;
    
    // Get student info
    Student student;
    if (!client.getStudent(userID, student)) {
        cout << "✗ Student record not found!" << endl;
        return 1;
    }
    
    cout << "Semester: " << student.currentSemester << endl;
    
    // Main loop
    while (true) {
        displayMenu();
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: viewCourses(client, student.currentSemester); break;
            case 2: enrollCourse(client, userID); break;
            case 3: dropCourse(client, userID); break;
            case 4: viewEnrolledCourses(client, userID); break;
            case 5: viewTimetable(client, userID); break;
            case 6:
                cout << "\nLogging out..." << endl;
                return 0;
            default:
                cout << "\nInvalid choice!" << endl;
        }
    }
    
    return 0;
}
