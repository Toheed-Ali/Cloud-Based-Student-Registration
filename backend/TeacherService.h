#ifndef TEACHER_SERVICE_H
#define TEACHER_SERVICE_H

#include "HTTPServer.h"
#include <sstream>

using namespace std;

class TeacherService {
public:
    // GET /api/teacher/viewStudents?teacherID=X
    static HTTPResponse viewStudents(const HTTPRequest& req, DatabaseManager& db) {
        // Extract teacher ID from query
        string teacherID;
        size_t pos = req.path.find("teacherID=");
        if (pos != string::npos) {
            size_t endPos = req.path.find("&", pos);
            if (endPos == string::npos) endPos = req.path.length();
            teacherID = req.path.substr(pos + 10, endPos - pos - 10);
        }
        
        if (teacherID.empty()) {
            return HTTPServer::jsonError("Teacher ID required");
        }
        
        Teacher teacher;
        if (!db.getTeacher(teacherID, teacher)) {
            return HTTPServer::jsonError("Teacher not found", 404);
        }
        
        // Get teacher's course
        Course course;
        if (!db.getCourse(teacher.assignedCourseID, course)) {
            return HTTPServer::jsonError("No course assigned");
        }
        
        // Get enrolled students
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < course.enrolledStudents.size(); i++) {
            if (i > 0) ss << ",";
            
            Student student;
            if (db.getStudent(course.enrolledStudents[i], student)) {
                ss << "{"
                   << "\"studentID\":\"" << student.studentID << "\","
                   << "\"name\":\"" << student.name << "\","
                   << "\"email\":\"" << student.email << "\","
                   << "\"semester\":" << student.currentSemester
                   << "}";
            }
        }
        ss << "]";
        
        map<string, string> response;
        response["success"] = "true";
        response["courseID"] = course.courseID;
        response["courseName"] = course.courseName;
        response["students"] = ss.str();
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // GET /api/teacher/viewTimetable?teacherID=X
    static HTTPResponse viewTimetable(const HTTPRequest& req, DatabaseManager& db) {
        // Extract teacher ID from query
        string teacherID;
        size_t pos = req.path.find("teacherID=");
        if (pos != string::npos) {
            size_t endPos = req.path.find("&", pos);
            if (endPos == string::npos) endPos = req.path.length();
            teacherID = req.path.substr(pos + 10, endPos - pos - 10);
        }
        
        if (teacherID.empty()) {
            return HTTPServer::jsonError("Teacher ID required");
        }
        
        Teacher teacher;
        if (!db.getTeacher(teacherID, teacher)) {
            return HTTPServer::jsonError("Teacher not found", 404);
        }
        
        // Get course
        Course course;
        if (!db.getCourse(teacher.assignedCourseID, course)) {
            return HTTPServer::jsonError("No course assigned");
        }
        
        // Get timetable
        Timetable timetable;
        if (!db.getTimetable(course.semester, timetable)) {
            return HTTPServer::jsonError("Timetable not generated yet");
        }
        
        // Find course in timetable
        stringstream ss;
        ss << "{";
        bool found = false;
        for (const auto& sc : timetable.schedule) {
            if (sc.courseID == teacher.assignedCourseID) {
                found = true;
                ss << "\"courseID\":\"" << sc.courseID << "\","
                   << "\"courseName\":\"" << sc.courseName << "\","
                   << "\"classroom\":" << sc.classroomID << ","
                   << "\"slots\":[";
                
                // Serialize all session slots
                for (size_t i = 0; i < sc.slots.size(); i++) {
                    if (i > 0) ss << ",";
                    const auto& slot = sc.slots[i];
                    ss << "{\"day\":" << slot.day
                       << ",\"hour\":" << slot.hour
                       << ",\"dayName\":\"" << slot.getDayName() << "\""
                       << ",\"time\":\"" << slot.getTimeString() << "\"}";
                }
                
                ss << "]";
                break;
            }
        }
        ss << "}";
        
        if (!found) {
            return HTTPServer::jsonError("Course not scheduled yet");
        }
        
        map<string, string> response;
        response["success"] = "true";
        response["timetable"] = ss.str();
        
        return HTTPServer::jsonSuccess(response);
    }
};

#endif // TEACHER_SERVICE_H
