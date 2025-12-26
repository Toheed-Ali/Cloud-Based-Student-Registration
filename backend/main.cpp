#include <iostream>
#include "../database/DatabaseManager.h"
#include "HTTPServer.h"
#include "AuthService.h"
#include "AdminService.h"
#include "StudentService.h"
#include "TeacherService.h"
#include "TimetableGenerator.h"

using namespace std;

int main() {
    cout << "========================================" << endl;
    cout << "  University Management System Server" << endl;
    cout << "========================================" << endl;
    cout << endl;
    
    // Initialize database
    DatabaseManager db("data");
    db.initialize();
    
    // Create HTTP server
    HTTPServer server(8080, db);
    
    // ========== Authentication Routes ==========
    server.post("/api/login", AuthService::login);
    
    // ========== Admin Routes ==========
    server.post("/api/admin/addStudent", AdminService::addStudent);
    server.post("/api/admin/removeStudent", AdminService::removeStudent);
    server.post("/api/admin/addTeacher", AdminService::addTeacher);
    server.post("/api/admin/removeTeacher", AdminService::removeTeacher);
    server.post("/api/admin/addCourse", AdminService::addCourse);
    server.post("/api/admin/setRegistrationWindow", AdminService::setRegistrationWindow);
    server.get("/api/admin/viewAllStudents", AdminService::viewAllStudents);
    server.get("/api/admin/viewAllTeachers", AdminService::viewAllTeachers);
    server.post("/api/admin/generateTimetable", TimetableGenerator::generateTimetableAPI);
    
    // ========== Student Routes ==========
    server.post("/api/student/enrollCourse", StudentService::enrollCourse);
    server.post("/api/student/dropCourse", StudentService::dropCourse);
    server.get("/api/student/viewCourses", StudentService::viewCourses);
    server.get("/api/student/viewTimetable", StudentService::viewTimetable);
    
    // ========== Teacher Routes ==========
    server.get("/api/teacher/viewStudents", TeacherService::viewStudents);
    server.get("/api/teacher/viewTimetable", TeacherService::viewTimetable);
    
    cout << "\n[Server] All routes registered successfully!" << endl;
    cout << "[Server] Total endpoints: 17" << endl;
    cout << "\n========================================" << endl;
    cout << "  Server Configuration" << endl;
    cout << "========================================" << endl;
    cout << "Port: 8080" << endl;
    cout << "Database: Custom B-Tree + Hash Table" << endl;
    cout << "Data Directory: ./data/" << endl;
    cout << "\nDefault Admin Credentials:" << endl;
    cout << "  Email: admin@university.com" << endl;
    cout << "  Password: admin123" << endl;
    cout << "========================================" << endl;
    
    // In production, this would call server.start() which uses cpp-httplib
    // For demonstration, we show the configuration
    server.start();
    
    cout << "\n[Server] Server ready!" << endl;
    cout << "[Server] Press Enter to exit..." << endl;
    cin.get();
    
    return 0;
}
