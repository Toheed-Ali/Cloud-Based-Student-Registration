#include <iostream>
#include <string>
#include <iomanip>
#include "../database/DatabaseManager.h"
#include "../backend/HTTPServer.h"
#include "../backend/AuthService.h"
#include "../backend/TeacherService.h"
#include "APIClient.h"

using namespace std;

void displayMenu() {
    cout << "\n==================================" << endl;
    cout << "      TEACHER PORTAL" << endl;
    cout << "==================================" << endl;
    cout << "1. View Enrolled Students" << endl;
    cout << "2. View My Course Timetable" << endl;
    cout << "3. Logout" << endl;
    cout << "==================================" << endl;
    cout << "Choice: ";
}

void viewEnrolledStudents(APIClient& client, const string& teacherID) {
    Teacher teacher;
    if (!client.getTeacher(teacherID, teacher)) {
        cout << "\n✗ Teacher record not found!" << endl;
        return;
    }
    
    Course course;
    if (!client.getCourse(teacher.assignedCourseID, course)) {
        cout << "\n⚠ No course assigned yet." << endl;
        return;
    }
    
    cout << "\n--- Enrolled Students ---" << endl;
    cout << "Course: " << course.courseName << " (" << course.courseID << ")" << endl;
    cout << "Total Students: " << course.currentEnrollmentCount << "/50\n" << endl;
    
    if (course.enrolledStudents.empty()) {
        cout << "No students enrolled yet." << endl;
        return;
    }
    
    cout << left << setw(12) << "ID" 
         << setw(25) << "Name" 
         << setw(25) << "Email" 
         << "Contact" << endl;
    cout << string(75, '-') << endl;
    
    for (const auto& studentID : course.enrolledStudents) {
        Student student;
        if (client.getStudent(studentID, student)) {
            cout << left << setw(12) << student.studentID
                 << setw(25) << student.name
                 << setw(25) << student.email
                 << student.contactInfo << endl;
        }
    }
}

void viewCourseTimetable(APIClient& client, const string& teacherID) {
    Teacher teacher;
    if (!client.getTeacher(teacherID, teacher)) {
        cout << "\n✗ Teacher record not found!" << endl;
        return;
    }
    
    Course course;
    if (!client.getCourse(teacher.assignedCourseID, course)) {
        cout << "\n⚠ No course assigned yet." << endl;
        return;
    }
    
    Timetable timetable;
    if (!client.studentViewTimetable(course.semester, timetable) || timetable.schedule.empty()) {
        cout << "\n⚠ Timetable not generated yet. Contact admin." << endl;
        return;
    }
    
    cout << "\n--- My Course Timetable ---" << endl;
    cout << "Course: " << course.courseName << " (" << course.courseID << ")" << endl;
    
    string days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    bool found = false;
    
    for (const auto& sc : timetable.schedule) {
        if (sc.courseID == course.courseID) {
            found = true;
            
            cout << "\nSchedule:" << endl;
            cout << "Classroom: " << sc.classroomID << endl;
            cout << "Students: " << sc.studentIDs.size() << endl;
            
            cout << "\nSession Times:" << endl;
            for (const auto& slot : sc.slots) {
                cout << "  " << slot.getDayName() << " " << slot.getTimeString() << endl;
            }
            break;
        }
    }
    
    if (!found) {
        cout << "\n⚠ Course not scheduled in timetable yet." << endl;
    }
}

int main() {
    cout << "========================================" << endl;
    cout << "  University Management System" << endl;
    cout << "       TEACHER PORTAL" << endl;
    cout << "========================================\n" << endl;
    
    // Initialize backend
    DatabaseManager db("data");
    db.initialize();
    
    HTTPServer server(8080, db);
    server.post("/api/login", AuthService::login);
    
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
    
    if (role != "TEACHER") {
        cout << "\n✗ Access denied. Teacher credentials required!" << endl;
        return 1;
    }
    
    cout << "\n✓ Logged in as: " << name << " (" << userID << ")\n" << endl;
    
    // Main loop
    while (true) {
        displayMenu();
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: viewEnrolledStudents(client, userID); break;
            case 2: viewCourseTimetable(client, userID); break;
            case 3:
                cout << "\nLogging out..." << endl;
                return 0;
            default:
                cout << "\nInvalid choice!" << endl;
        }
    }
    
    return 0;
}
