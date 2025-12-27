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
        // Get semester from query parameters
        int semester = 1;  // Default
        
        if (req.params.count("semester") > 0) {
            semester = stoi(req.params.at("semester"));
        }
        
        cout << "[StudentService] Querying courses for semester: " << semester << endl;
        vector<Course> courses = db.getCoursesBySemester(semester);
        cout << "[StudentService] Found " << courses.size() << " courses" << endl;
        if (!courses.empty()) {
            cout << "[StudentService] First course: " << courses[0].courseID << " semester=" << courses[0].semester << endl;
        }
        
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < courses.size(); i++) {
            Teacher teacher;
            string teacherName = "TBA";
            if (!courses[i].teacherID.empty() && db.getTeacher(courses[i].teacherID, teacher)) {
                teacherName = teacher.name;
            }

            if (i > 0) ss << ",";
            ss << "{"
               << "\"courseID\":\"" << courses[i].courseID << "\","
               << "\"courseName\":\"" << courses[i].courseName << "\","
               << "\"semester\":" << courses[i].semester << ","
               << "\"teacherID\":\"" << courses[i].teacherID << "\","
               << "\"teacherName\":\"" << teacherName << "\","
               << "\"enrollmentCount\":" << courses[i].currentEnrollmentCount << ","
               << "\"credits\":3,"
               << "\"available\":" << (courses[i].currentEnrollmentCount < 50 ? "true" : "false")
               << "}";
        }
        ss << "]";
        
        map<string, string> response;
        response["success"] = "true";
        response["courses"] = ss.str();
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // GET /api/student/getMyData?studentID=X or /api/student/mydata?studentID=X
    static HTTPResponse getMyData(const HTTPRequest& req, DatabaseManager& db) {
        string studentID;
        
        // CRITICAL FIX: Check both param formats
        if (req.params.count("studentID") > 0) {
            studentID = req.params.at("studentID");
        }
        
        cout << "[StudentService] getMyData called for studentID: " << studentID << endl;
        
        if (studentID.empty()) {
            cout << "[StudentService] ERROR: No studentID provided" << endl;
            return HTTPServer::jsonError("Student ID required");
        }
        
        Student student;
        if (!db.getStudent(studentID, student)) {
            cout << "[StudentService] ERROR: Student not found: " << studentID << endl;
            return HTTPServer::jsonError("Student not found", 404);
        }
        
        cout << "[StudentService] Found student: " << student.name << " (semester " << student.currentSemester << ")" << endl;
        
        // Get enrolled courses with details
        stringstream coursesJson;
        coursesJson << "[";
        for (size_t i = 0; i < student.enrolledCourses.size(); i++) {
            if (i > 0) coursesJson << ",";
            
            Course course;
            if (db.getCourse(student.enrolledCourses[i], course)) {
                Teacher teacher;
                string teacherName = "TBA";
                if (!course.teacherID.empty() && db.getTeacher(course.teacherID, teacher)) {
                    teacherName = teacher.name;
                }

                coursesJson << "{"
                           << "\"courseID\":\"" << course.courseID << "\","
                           << "\"courseName\":\"" << course.courseName << "\","
                           << "\"teacherID\":\"" << course.teacherID << "\","
                           << "\"teacherName\":\"" << teacherName << "\","
                           << "\"semester\":" << course.semester << ","
                           << "\"enrollmentCount\":" << course.currentEnrollmentCount << ","
                           << "\"credits\":3"
                           << "}";
            }
        }
        coursesJson << "]";
        
        map<string, string> response;
        response["success"] = "true";
        response["studentID"] = student.studentID;
        response["name"] = student.name;
        response["email"] = student.email;
        response["currentSemester"] = to_string(student.currentSemester);
        response["enrolledCourses"] = coursesJson.str();
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // GET /api/student/viewTimetable?studentID=X
    static HTTPResponse viewTimetable(const HTTPRequest& req, DatabaseManager& db) {
        // Restriction: Student sees timetable only after registration is closed
        if (db.isRegistrationOpen()) {
             return HTTPServer::jsonError("Timetable will be available after registration closes.", 403);
        }

        // CRITICAL FIX: Extract student ID from query params
        string studentID;
        
        if (req.params.count("studentID") > 0) {
            studentID = req.params.at("studentID");
        }
        
        cout << "[StudentService] viewTimetable called for student: " << studentID << endl;
        
        if (studentID.empty()) {
            cout << "[StudentService] viewTimetable: No studentID provided" << endl;
            return HTTPServer::jsonError("Student ID required");
        }
        
        Student student;
        if (!db.getStudent(studentID, student)) {
            cout << "[StudentService] Student not found: " << studentID << endl;
            return HTTPServer::jsonError("Student not found", 404);
        }
        
        cout << "[StudentService] Student found. Semester: " << student.currentSemester << endl;
        cout << "[StudentService] Enrolled courses: " << student.enrolledCourses.size() << endl;
        
        // Get student's timetable
        Timetable timetable;
        if (!db.getTimetable(student.currentSemester, timetable)) {
            cout << "[StudentService] No timetable for semester " << student.currentSemester << endl;
            return HTTPServer::jsonError("Timetable not generated yet", 400);
        }
        
        cout << "[StudentService] Timetable found with " << timetable.schedule.size() << " total courses" << endl;
        
        // CRITICAL FIX: Filter for student's enrolled courses
        stringstream ss;
        ss << "[";
        bool first = true;
        int matchedCourses = 0;
        
        for (const auto& sc : timetable.schedule) {
            // Check if student is enrolled in this course
            bool isEnrolled = false;
            for (const auto& enrolledCourseID : student.enrolledCourses) {
                if (enrolledCourseID == sc.courseID) {
                    isEnrolled = true;
                    break;
                }
            }
            
            if (isEnrolled) {
                matchedCourses++;
                cout << "[StudentService] Including course: " << sc.courseID << " with " << sc.slots.size() << " slots" << endl;
                
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
                    
                    cout << "[StudentService]   Slot: day=" << slot.day << " hour=" << slot.hour << endl;
                    
                    ss << "{\"day\":" << slot.day << ",\"hour\":" << slot.hour
                       << ",\"dayName\":\"" << slot.getDayName() << "\""
                       << ",\"time\":\"" << slot.getTimeString() << "\"}";
                }
                
                ss << "]}";
            }
        }
        ss << "]";
        
        cout << "[StudentService] Matched " << matchedCourses << " enrolled courses in timetable" << endl;
        
        string timetableJson = ss.str();
        cout << "[StudentService] Timetable JSON length: " << timetableJson.length() << endl;
        
        map<string, string> response;
        response["success"] = "true";
        response["timetable"] = timetableJson;
        
        return HTTPServer::jsonSuccess(response);
    }
};

#endif // STUDENT_SERVICE_H