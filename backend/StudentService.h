#ifndef STUDENT_SERVICE_H
#define STUDENT_SERVICE_H

#include "HTTPServer.h"
#include <sstream>

using namespace std;

class StudentService {
public:
    // POST /api/student/enrollCourse
    static HTTPResponse enrollCourse(const HTTPRequest& req, DatabaseManager& db) {
        auto data = JSONParser::parse(req.body);
        string studentID = JSONParser::getString(data, "studentID");
        string courseID = JSONParser::getString(data, "courseID");
        
        if (studentID.empty() || courseID.empty()) {
            return HTTPServer::jsonError("Student ID and Course ID required");
        }
        
        string errorMsg;
        if (!db.canEnroll(studentID, courseID, errorMsg)) {
            return HTTPServer::jsonError(errorMsg);
        }
        
        if (!db.enrollStudent(studentID, courseID)) {
            return HTTPServer::jsonError("Enrollment failed");
        }
        
        map<string, string> response;
        response["success"] = "true";
        response["message"] = "Enrolled successfully";
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // POST /api/student/dropCourse
    static HTTPResponse dropCourse(const HTTPRequest& req, DatabaseManager& db) {
        auto data = JSONParser::parse(req.body);
        string studentID = JSONParser::getString(data, "studentID");
        string courseID = JSONParser::getString(data, "courseID");
        
        if (studentID.empty() || courseID.empty()) {
            return HTTPServer::jsonError("Student ID and Course ID required");
        }
        
        if (!db.dropCourse(studentID, courseID)) {
            return HTTPServer::jsonError("Drop course failed");
        }
        
        map<string, string> response;
        response["success"] = "true";
        response["message"] = "Course dropped successfully";
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // GET /api/student/viewCourses?semester=X
    static HTTPResponse viewCourses(const HTTPRequest& req, DatabaseManager& db) {
        // Parse query string (simplified)
        int semester = 1;  // Default
        size_t pos = req.path.find("semester=");
        if (pos != string::npos) {
            semester = stoi(req.path.substr(pos + 9, 1));
        }
        
        vector<Course> courses = db.getCoursesBySemester(semester);
        
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < courses.size(); i++) {
            if (i > 0) ss << ",";
            ss << "{"
               << "\"courseID\":\"" << courses[i].courseID << "\","
               << "\"courseName\":\"" << courses[i].courseName << "\","
               << "\"semester\":" << courses[i].semester << ","
               << "\"teacherID\":\"" << courses[i].teacherID << "\","
               << "\"enrollmentCount\":" << courses[i].currentEnrollmentCount << ","
               << "\"available\":" << (courses[i].currentEnrollmentCount < 50 ? "true" : "false")
               << "}";
        }
        ss << "]";
        
        map<string, string> response;
        response["success"] = "true";
        response["courses"] = ss.str();
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // GET /api/student/viewTimetable?studentID=X
    static HTTPResponse viewTimetable(const HTTPRequest& req, DatabaseManager& db) {
        // Extract student ID from query
        string studentID;
        size_t pos = req.path.find("studentID=");
        if (pos != string::npos) {
            size_t endPos = req.path.find("&", pos);
            if (endPos == string::npos) endPos = req.path.length();
            studentID = req.path.substr(pos + 10, endPos - pos - 10);
        }
        
        if (studentID.empty()) {
            return HTTPServer::jsonError("Student ID required");
        }
        
        Student student;
        if (!db.getStudent(studentID, student)) {
            return HTTPServer::jsonError("Student not found", 404);
        }
        
        // Get student's timetable
        Timetable timetable;
        if (!db.getTimetable(student.currentSemester, timetable)) {
            return HTTPServer::jsonError("Timetable not generated yet");
        }
        
        // Filter for student's enrolled courses
        stringstream ss;
        ss << "[";
        bool first = true;
        for (const auto& sc : timetable.schedule) {
            // Check if student is enrolled in this course
            if (find(student.enrolledCourses.begin(), student.enrolledCourses.end(), 
                     sc.courseID) != student.enrolledCourses.end()) {
                if (!first) ss << ",";
                first = false;
                ss << "{"
                   << "\"courseID\":\"" << sc.courseID << "\","
                   << "\"courseName\":\"" << sc.courseName << "\","
                   << "\"teacherName\":\"" << sc.teacherName << "\","
                   << "\"classroom\":" << sc.classroomID << ","
                   << "\"slots\":[";
                
                // Serialize all slots for this course
                for (size_t i = 0; i < sc.slots.size(); i++) {
                    if (i > 0) ss << ",";
                    const auto& slot = sc.slots[i];
                    ss << "{\"day\":" << slot.day << ",\"hour\":" << slot.hour
                       << ",\"dayName\":\"" << slot.getDayName() << "\""
                       << ",\"time\":\"" << slot.getTimeString() << "\"}";
                }
                
                ss << "]}";
            }
        }
        ss << "]";
        
        map<string, string> response;
        response["success"] = "true";
        response["timetable"] = ss.str();
        
        return HTTPServer::jsonSuccess(response);
    }
};

#endif // STUDENT_SERVICE_H
