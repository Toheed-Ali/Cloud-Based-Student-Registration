#ifndef API_CLIENT_H
#define API_CLIENT_H

#include "../backend/HTTPServer.h"
#include "../backend/utils/JSONParser.h"
#include "../database/DatabaseManager.h"
#include <string>
#include <map>

using namespace std;

// Simple API client that communicates with the backend
// In production, this would use actual HTTP requests
// For demonstration, we directly call the backend handlers
class APIClient {
private:
    DatabaseManager& db;
    HTTPServer& server;
    string authToken;
    string userRole;
    
public:
    APIClient(DatabaseManager& database, HTTPServer& srv) 
        : db(database), server(srv) {}
    
    // Get database reference for error checking
    DatabaseManager& getDB() { return db; }
    
    // Authentication
    bool login(const string& email, const string& password, string& outRole, string& outUserID, string& outName) {
        HTTPRequest req;
        req.method = "POST";
        req.path = "/api/login";
        
        map<string, string> body;
        body["email"] = email;
        body["password"] = password;
        req.body = JSONParser::stringify(body);
        
        HTTPResponse resp = server.handleRequest(req);
        
        if (resp.statusCode == 200) {
            auto data = JSONParser::parse(resp.body);
            if (JSONParser::getString(data, "success") == "true") {
                authToken = JSONParser::getString(data, "token");
                userRole = JSONParser::getString(data, "role");
                outRole = userRole;
                outUserID = JSONParser::getString(data, "userID");
                outName = JSONParser::getString(data, "name");
                return true;
            }
        }
        return false;
    }
    
    string getRole() const { return userRole; }
    
    // Admin Operations
    bool adminAddStudent(const string& studentID, const string& email, const string& name, 
                        int semester, const string& contact, const string& password) {
        HTTPRequest req;
        req.method = "POST";
        req.path = "/api/admin/addStudent";
        
        map<string, string> body;
        body["studentID"] = studentID;
        body["email"] = email;
        body["name"] = name;
        body["semester"] = to_string(semester);
        body["contact"] = contact;
        body["password"] = password;
        req.body = JSONParser::stringify(body);
        
        HTTPResponse resp = server.handleRequest(req);
        return resp.statusCode == 200;
    }
    
    bool adminAddTeacher(const string& teacherID, const string& email, const string& name,
                        const string& courseID, const string& department, const string& contact,
                        const string& password) {
        HTTPRequest req;
        req.method = "POST";
        req.path = "/api/admin/addTeacher";
        
        map<string, string> body;
        body["teacherID"] = teacherID;
        body["email"] = email;
        body["name"] = name;
        body["courseID"] = courseID;
        body["department"] = department;
        body["contact"] = contact;
        body["password"] = password;
        req.body = JSONParser::stringify(body);
        
        HTTPResponse resp = server.handleRequest(req);
        return resp.statusCode == 200;
    }
    
    bool adminAddCourse(const string& courseID, const string& courseName, 
                       int semester, const string& teacherID) {
        HTTPRequest req;
        req.method = "POST";
        req.path = "/api/admin/addCourse";
        
        map<string, string> body;
        body["courseID"] = courseID;
        body["courseName"] = courseName;
        body["semester"] = to_string(semester);
        body["teacherID"] = teacherID;
        req.body = JSONParser::stringify(body);
        
        HTTPResponse resp = server.handleRequest(req);
        return resp.statusCode == 200;
    }
    
    bool adminSetRegistrationWindow(time_t start, time_t end, bool isOpen) {
        HTTPRequest req;
        req.method = "POST";
        req.path = "/api/admin/setRegistrationWindow";
        
        map<string, string> body;
        body["startTime"] = to_string(start);
        body["endTime"] = to_string(end);
        body["isOpen"] = isOpen ? "true" : "false";
        req.body = JSONParser::stringify(body);
        
        HTTPResponse resp = server.handleRequest(req);
        return resp.statusCode == 200;
    }
    
    vector<Student> adminViewAllStudents() {
        HTTPRequest req;
        req.method = "GET";
        req.path = "/api/admin/viewAllStudents";
        
        HTTPResponse resp = server.handleRequest(req);
        
        // In a real implementation, parse the JSON array
        // For now, directly get from database
        return db.getAllStudents();
    }
    
    vector<Teacher> adminViewAllTeachers() {
        return db.getAllTeachers();
    }
    
    bool adminGenerateTimetable() {
        HTTPRequest req;
        req.method = "POST";
        req.path = "/api/admin/generateTimetable";
        req.body = "{}";
        
        HTTPResponse resp = server.handleRequest(req);
        return resp.statusCode == 200;
    }
    
    // Student Operations
    bool studentEnrollCourse(const string& studentID, const string& courseID) {
        HTTPRequest req;
        req.method = "POST";
        req.path = "/api/student/enrollCourse";
        
        map<string, string> body;
        body["studentID"] = studentID;
        body["courseID"] = courseID;
        req.body = JSONParser::stringify(body);
        
        HTTPResponse resp = server.handleRequest(req);
        return resp.statusCode == 200;
    }
    
    bool studentDropCourse(const string& studentID, const string& courseID) {
        HTTPRequest req;
        req.method = "POST";
        req.path = "/api/student/dropCourse";
        
        map<string, string> body;
        body["studentID"] = studentID;
        body["courseID"] = courseID;
        req.body = JSONParser::stringify(body);
        
        HTTPResponse resp = server.handleRequest(req);
        return resp.statusCode == 200;
    }
    
    vector<Course> studentViewCourses(int semester) {
        return db.getCoursesBySemester(semester);
    }
    
    bool studentViewTimetable(int semester, Timetable& outTimetable) {
        return db.getTimetable(semester, outTimetable);
    }
    
    bool getStudent(const string& studentID, Student& outStudent) {
        return db.getStudent(studentID, outStudent);
    }
    
    // Teacher Operations
    bool getTeacher(const string& teacherID, Teacher& outTeacher) {
        return db.getTeacher(teacherID, outTeacher);
    }
    
    bool getCourse(const string& courseID, Course& outCourse) {
        return db.getCourse(courseID, outCourse);
    }
};

#endif // API_CLIENT_H
