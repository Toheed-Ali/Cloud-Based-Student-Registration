#include <iostream>
#include <cassert>
#include "../database/DatabaseManager.h"
#include "../backend/HTTPServer.h"
#include "../backend/AuthService.h"
#include "../backend/AdminService.h"
#include "../backend/StudentService.h"
#include "../backend/TeacherService.h"
#include "../backend/TimetableGenerator.h"

using namespace std;

void testAuth(DatabaseManager& db, HTTPServer& server) {
    cout << "\n=== Testing Authentication ===" << endl;
    
    // Test login with admin
    HTTPRequest req;
    req.method = "POST";
    req.path = "/api/login";
    req.body = "{\"email\":\"admin@university.com\",\"password\":\"admin123\"}";
    
    HTTPResponse resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    
    auto respData = JSONParser::parse(resp.body);
    assert(JSONParser::getString(respData, "success") == "true");
    assert(JSONParser::getString(respData, "role") == "ADMIN");
    
    cout << "[PASS] Admin login" << endl;
    
    // Test invalid login
    req.body = "{\"email\":\"invalid@test.com\",\"password\":\"wrong\"}";
    resp = server.handleRequest(req);
    assert(resp.statusCode == 401);
    cout << "[PASS] Invalid login rejection" << endl;
}

void testAdminOperations(DatabaseManager& db, HTTPServer& server) {
    cout << "\n=== Testing Admin Operations ===" << endl;
    
    // Add student
    HTTPRequest req;
    req.method = "POST";
    req.path = "/api/admin/addStudent";
    req.body = "{"
                "\"studentID\":\"S001\","
                "\"email\":\"student1@test.com\","
                "\"name\":\"John Doe\","
                "\"semester\":1,"
                "\"contact\":\"123-456-7890\","
                "\"password\":\"student123\""
                "}";
    
    HTTPResponse resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    cout << "[PASS] Add student" << endl;
    
    // Add teacher
    req.path = "/api/admin/addTeacher";
    req.body = "{"
                "\"teacherID\":\"T001\","
                "\"email\":\"teacher1@test.com\","
                "\"name\":\"Dr. Smith\","
                "\"courseID\":\"CS101\","
                "\"department\":\"Computer Science\","
                "\"contact\":\"987-654-3210\","
                "\"password\":\"teacher123\""
                "}";
    
    resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    cout << "[PASS] Add teacher" << endl;
    
    // Add course
    req.path = "/api/admin/addCourse";
    req.body = "{"
                "\"courseID\":\"CS101\","
                "\"courseName\":\"Introduction to Programming\","
                "\"semester\":1,"
                "\"teacherID\":\"T001\""
                "}";
    
    resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    cout << "[PASS] Add course" << endl;
    
    // Set registration window
    req.path = "/api/admin/setRegistrationWindow";
    time_t now = time(nullptr);
    string body = "{\"startTime\":" + to_string(now - 3600) + 
                  ",\"endTime\":" + to_string(now + 3600) + 
                  ",\"isOpen\":true}";
    req.body = body;
    
    resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    cout << "[PASS] Set registration window" << endl;
    
    // View all students
    req.method = "GET";
    req.path = "/api/admin/viewAllStudents";
    req.body = "";
    
    resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    assert(resp.body.find("S001") != string::npos);
    cout << "[PASS] View all students" << endl;
}

void testStudentOperations(DatabaseManager& db, HTTPServer& server) {
    cout << "\n=== Testing Student Operations ===" << endl;
    
    // Enroll in course
    HTTPRequest req;
    req.method = "POST";
    req.path = "/api/student/enrollCourse";
    req.body = "{\"studentID\":\"S001\",\"courseID\":\"CS101\"}";
    
    HTTPResponse resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    cout << "[PASS] Enroll in course" << endl;
    
    // View courses
    req.method = "GET";
    req.path = "/api/student/viewCourses?semester=1";
    req.body = "";
    
    resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    assert(resp.body.find("CS101") != string::npos);
    cout << "[PASS] View courses" << endl;
    
    // Drop course
    req.method = "POST";
    req.path = "/api/student/dropCourse";
    req.body = "{\"studentID\":\"S001\",\"courseID\":\"CS101\"}";
    
    resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    cout << "[PASS] Drop course" << endl;
}

void testTimetableGeneration(DatabaseManager& db, HTTPServer& server) {
    cout << "\n=== Testing Timetable Generation ===" << endl;
    
    // Re-enroll student for timetable test
    db.enrollStudent("S001", "CS101");
    
    // Add more courses and students for realistic test
    Course c2;
    c2.courseID = "CS102";
    c2.courseName = "Data Structures";
    c2.semester = 1;
    c2.teacherID = "T001";
    db.addCourse(c2);
    
    // Generate timetable
    HTTPRequest req;
    req.method = "POST";
    req.path = "/api/admin/generateTimetable";
    req.body = "{}";
    
    HTTPResponse resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    
    auto respData = JSONParser::parse(resp.body);
    assert(JSONParser::getString(respData, "success") == "true");
    
    cout << "[PASS] Timetable generation" << endl;
    
    // Verify timetable was created
    Timetable* tt = db.getTimetable(1);
    assert(tt != nullptr);
    assert(!tt->schedule.empty());
    
    cout << "[PASS] Timetable verification (" << tt->schedule.size() << " courses scheduled)" << endl;
}

void testTeacherOperations(DatabaseManager& db, HTTPServer& server) {
    cout << "\n=== Testing Teacher Operations ===" << endl;
    
    // View students in course
    HTTPRequest req;
    req.method = "GET";
    req.path = "/api/teacher/viewStudents?teacherID=T001";
    req.body = "";
    
    HTTPResponse resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    assert(resp.body.find("CS101") != string::npos);
    cout << "[PASS] View enrolled students" << endl;
    
    // View timetable
    req.path = "/api/teacher/viewTimetable?teacherID=T001";
    
    resp = server.handleRequest(req);
    assert(resp.statusCode == 200);
    cout << "[PASS] View teacher timetable" << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "  Backend Integration Test Suite" << endl;
    cout << "========================================" << endl;
    
    try {
        // Initialize database
        DatabaseManager db("test_data");
        db.initialize();
        
        // Create HTTP server
        HTTPServer server(8080, db);
        
        // Register all routes
        server.post("/api/login", AuthService::login);
        server.post("/api/admin/addStudent", AdminService::addStudent);
        server.post("/api/admin/addTeacher", AdminService::addTeacher);
        server.post("/api/admin/addCourse", AdminService::addCourse);
        server.post("/api/admin/setRegistrationWindow", AdminService::setRegistrationWindow);
        server.get("/api/admin/viewAllStudents", AdminService::viewAllStudents);
        server.post("/api/student/enrollCourse", StudentService::enrollCourse);
        server.post("/api/student/dropCourse", StudentService::dropCourse);
        server.get("/api/student/viewCourses", StudentService::viewCourses);
        server.get("/api/teacher/viewStudents", TeacherService::viewStudents);
        server.get("/api/teacher/viewTimetable", TeacherService::viewTimetable);
        server.post("/api/admin/generateTimetable", TimetableGenerator::generateTimetableAPI);
        
        // Run tests
        testAuth(db, server);
        testAdminOperations(db, server);
        testStudentOperations(db, server);
        testTimetableGeneration(db, server);
        testTeacherOperations(db, server);
        
        cout << "\n========================================" << endl;
        cout << "  ALL BACKEND TESTS PASSED!" << endl;
        cout << "========================================" << endl;
        
        return 0;
    } catch (const exception& e) {
        cerr << "\n[ERROR] Test failed: " << e.what() << endl;
        return 1;
    }
}
