#ifndef ADMIN_SERVICE_H
#define ADMIN_SERVICE_H

#include "HTTPServer.h"
#include "utils/SHA256.h"
#include <sstream>

using namespace std;

class AdminService {
public:
    // POST /api/admin/addStudent
    static HTTPResponse addStudent(const HTTPRequest& req, DatabaseManager& db) {
        auto data = JSONParser::parse(req.body);
        
        Student student;
        student.studentID = JSONParser::getString(data, "studentID");
        student.email = JSONParser::getString(data, "email");
        student.name = JSONParser::getString(data, "name");
        student.currentSemester = JSONParser::getInt(data, "semester", 1);
        student.contactInfo = JSONParser::getString(data, "contact");
        student.dateOfAdmission = time(nullptr);
        
        if (student.studentID.empty() || student.email.empty() || student.name.empty()) {
            return HTTPServer::jsonError("Missing required fields");
        }
        
        // Add to database
        if (!db.addStudent(student)) {
            return HTTPServer::jsonError("Failed to add student");
        }
        
        // Also create user account
        User user;
        user.userID = student.studentID;
        user.email = student.email;
        user.name = student.name;
        user.role = UserRole::STUDENT;
        string defaultPassword = JSONParser::getString(data, "password", "student123");
        user.passwordHash = SHA256::hash(defaultPassword);
        
        db.createUser(user);
        
        map<string, string> response;
        response["success"] = "true";
        response["message"] = "Student added successfully";
        response["studentID"] = student.studentID;
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // POST /api/admin/removeStudent
    static HTTPResponse removeStudent(const HTTPRequest& req, DatabaseManager& db) {
        auto data = JSONParser::parse(req.body);
        string studentID = JSONParser::getString(data, "studentID");
        
        if (studentID.empty()) {
            return HTTPServer::jsonError("Student ID required");
        }
        
        Student student;
        if (!db.getStudent(studentID, student)) {
            return HTTPServer::jsonError("Student not found", 404);
        }
        
        string email = student.email;
        
        db.deleteStudent(studentID);
        db.deleteUser(email);
        
        map<string, string> response;
        response["success"] = "true";
        response["message"] = "Student removed successfully";
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // POST /api/admin/addTeacher
    static HTTPResponse addTeacher(const HTTPRequest& req, DatabaseManager& db) {
        auto data = JSONParser::parse(req.body);
        
        Teacher teacher;
        teacher.teacherID = JSONParser::getString(data, "teacherID");
        teacher.email = JSONParser::getString(data, "email");
        teacher.name = JSONParser::getString(data, "name");
        teacher.assignedCourseID = JSONParser::getString(data, "courseID");
        teacher.department = JSONParser::getString(data, "department");
        teacher.contactInfo = JSONParser::getString(data, "contact");
        
        if (teacher.teacherID.empty() || teacher.email.empty() || teacher.name.empty()) {
            return HTTPServer::jsonError("Missing required fields");
        }
        
        if (!db.addTeacher(teacher)) {
            return HTTPServer::jsonError("Failed to add teacher");
        }
        
        // Create user account
        User user;
        user.userID = teacher.teacherID;
        user.email = teacher.email;
        user.name = teacher.name;
        user.role = UserRole::TEACHER;
        string defaultPassword = JSONParser::getString(data, "password", "teacher123");
        user.passwordHash = SHA256::hash(defaultPassword);
        
        db.createUser(user);
        
        map<string, string> response;
        response["success"] = "true";
        response["message"] = "Teacher added successfully";
        response["teacherID"] = teacher.teacherID;
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // POST /api/admin/removeTeacher
    static HTTPResponse removeTeacher(const HTTPRequest& req, DatabaseManager& db) {
        auto data = JSONParser::parse(req.body);
        string teacherID = JSONParser::getString(data, "teacherID");
        
        if (teacherID.empty()) {
            return HTTPServer::jsonError("Teacher ID required");
        }
        
        Teacher teacher;
        if (!db.getTeacher(teacherID, teacher)) {
            return HTTPServer::jsonError("Teacher not found", 404);
        }
        
        string email = teacher.email;
        
        db.deleteTeacher(teacherID);
        db.deleteUser(email);
        
        map<string, string> response;
        response["success"] = "true";
        response["message"] = "Teacher removed successfully";
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // POST /api/admin/setRegistrationWindow
    static HTTPResponse setRegistrationWindow(const HTTPRequest& req, DatabaseManager& db) {
        auto data = JSONParser::parse(req.body);
        
        SystemConfig config = db.getConfig();
        config.registrationStartTime = JSONParser::getInt(data, "startTime", 0);
        config.registrationEndTime = JSONParser::getInt(data, "endTime", 0);
        config.isRegistrationOpen = JSONParser::getBool(data, "isOpen", true);
        
        // Auto-clear timetables when registration opens to prevent stale data
        if (config.isRegistrationOpen) {
            cout << "[AdminService] Registration Opened. Clearing old timetables." << endl;
            db.clearTimetables();
        }

        db.updateConfig(config);
        
        map<string, string> response;
        response["success"] = "true";
        response["message"] = "Registration window updated";
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // GET /api/admin/getRegistrationWindow
    static HTTPResponse getRegistrationWindow(const HTTPRequest& req, DatabaseManager& db) {
        SystemConfig config = db.getConfig();
        
        map<string, string> response;
        response["success"] = "true";
        response["startTime"] = to_string(config.registrationStartTime);
        response["endTime"] = to_string(config.registrationEndTime);
        response["isOpen"] = config.isRegistrationOpen ? "true" : "false";
        
        HTTPResponse httpRes = HTTPServer::jsonSuccess(response);
        
        // CRITICAL: Add cache-prevention headers to ensure all students see the same status
        httpRes.headers["Cache-Control"] = "no-store, no-cache, must-revalidate, max-age=0";
        httpRes.headers["Pragma"] = "no-cache";
        httpRes.headers["Expires"] = "0";
        
        return httpRes;
    }
    
    // GET /api/admin/viewAllStudents
    static HTTPResponse viewAllStudents(const HTTPRequest& req, DatabaseManager& db) {
        vector<Student> students = db.getAllStudents();
        
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < students.size(); i++) {
            if (i > 0) ss << ",";
            ss << "{"
               << "\"studentID\":\"" << students[i].studentID << "\","
               << "\"name\":\"" << students[i].name << "\","
               << "\"email\":\"" << students[i].email << "\","
               << "\"semester\":" << students[i].currentSemester << ","
               << "\"enrolledCourses\":" << JSONParser::stringifyArray(students[i].enrolledCourses)
               << "}";
        }
        ss << "]";
        
        map<string, string> response;
        response["success"] = "true";
        response["students"] = ss.str();
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // GET /api/admin/viewAllTeachers
    static HTTPResponse viewAllTeachers(const HTTPRequest& req, DatabaseManager& db) {
        vector<Teacher> teachers = db.getAllTeachers();
        
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < teachers.size(); i++) {
            if (i > 0) ss << ",";
            ss << "{"
               << "\"teacherID\":\"" << teachers[i].teacherID << "\","
               << "\"name\":\"" << teachers[i].name << "\","
               << "\"email\":\"" << teachers[i].email << "\","
               << "\"department\":\"" << teachers[i].department << "\","
               << "\"assignedCourse\":\"" << teachers[i].assignedCourseID << "\""
               << "}";
        }
        ss << "]";
        
        map<string, string> response;
        response["success"] = "true";
        response["teachers"] = ss.str();
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // POST /api/admin/addCourse
    static HTTPResponse addCourse(const HTTPRequest& req, DatabaseManager& db) {
        auto data = JSONParser::parse(req.body);
        
        Course course;
        course.courseID = JSONParser::getString(data, "courseID");
        course.courseName = JSONParser::getString(data, "courseName");
        course.semester = JSONParser::getInt(data, "semester");
        course.teacherID = JSONParser::getString(data, "teacherID");
        course.currentEnrollmentCount = 0;
        
        if (course.courseID.empty() || course.courseName.empty()) {
            return HTTPServer::jsonError("Missing required fields");
        }
        
        if (!db.addCourse(course)) {
            return HTTPServer::jsonError("Failed to add course");
        }
        
        map<string, string> response;
        response["success"] = "true";
        response["message"] = "Course added successfully";
        response["courseID"] = course.courseID;
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // GET /api/admin/viewTimetable?semester=X
    static HTTPResponse viewTimetable(const HTTPRequest& req, DatabaseManager& db) {
        // Extract semester from req.params
        string semesterStr;
        
        if (req.params.count("semester") > 0) {
            semesterStr = req.params.at("semester");
        }
        
        cout << "[AdminService] viewTimetable called" << endl;
        cout << "[AdminService] Semester param: '" << semesterStr << "'" << endl;
        cout << "[AdminService] All params:" << endl;
        for (const auto& p : req.params) {
            cout << "  " << p.first << " = " << p.second << endl;
        }
        
        if (semesterStr.empty()) {
            cout << "[AdminService] ERROR: semester parameter is empty" << endl;
            return HTTPServer::jsonError("Semester parameter required", 400);
        }
        
        int semester = 0;
        try {
            semester = stoi(semesterStr);
        } catch (const exception& e) {
            cout << "[AdminService] ERROR: Failed to parse semester: " << e.what() << endl;
            return HTTPServer::jsonError("Invalid semester parameter", 400);
        }
        
        cout << "[AdminService] Parsed semester: " << semester << endl;
        
        // Get timetable for this semester
        Timetable timetable;
        bool found = db.getTimetable(semester, timetable);
        
        cout << "[AdminService] getTimetable returned: " << (found ? "true" : "false") << endl;
        
        if (!found) {
            cout << "[AdminService] No timetable found for semester " << semester << endl;
            return HTTPServer::jsonError("Timetable not found for semester " + semesterStr, 404);
        }
        
        cout << "[AdminService] Found timetable with " << timetable.schedule.size() << " courses" << endl;
        
        // Return ALL courses for admin (no filtering)
        stringstream ss;
        ss << "[";
        
        for (size_t i = 0; i < timetable.schedule.size(); i++) {
            if (i > 0) ss << ",";
            
            const auto& sc = timetable.schedule[i];
            
            // CRITICAL FIX: Get real-time enrollment count from Course object
            // The sc.studentIDs might be stale if students enrolled after timetable generation
            Course course;
            int currentCount = 0;
            if (db.getCourse(sc.courseID, course)) {
                currentCount = course.enrolledStudents.size();
            } else {
                currentCount = sc.studentIDs.size(); // Fallback
            }
            
            cout << "[AdminService] Serializing course " << (i+1) << "/" << timetable.schedule.size() 
                 << ": " << sc.courseID << " with " << sc.slots.size() << " slots. Enrolled: " << currentCount << endl;
            
            ss << "{"
               << "\"courseID\":\"" << sc.courseID << "\","
               << "\"courseName\":\"" << sc.courseName << "\","
               << "\"teacherName\":\"" << sc.teacherName << "\","
               << "\"classroom\":" << sc.classroomID << ","
               << "\"studentCount\":" << currentCount << ","
               << "\"slots\":[";
            
            // Serialize all slots
            for (size_t j = 0; j < sc.slots.size(); j++) {
                if (j > 0) ss << ",";
                const auto& slot = sc.slots[j];
                ss << "{"
                   << "\"day\":" << slot.day << ","
                   << "\"hour\":" << slot.hour << ","
                   << "\"dayName\":\"" << slot.getDayName() << "\","
                   << "\"time\":\"" << slot.getTimeString() << "\""
                   << "}";
            }
            
            ss << "]}";
        }
        
        ss << "]";
        
        string timetableJson = ss.str();
        cout << "[AdminService] Generated JSON length: " << timetableJson.length() << " bytes" << endl;
        cout << "[AdminService] JSON preview: " << timetableJson.substr(0, 200) << "..." << endl;
        
        map<string, string> response;
        response["success"] = "true";
        response["semester"] = semesterStr;
        response["timetable"] = timetableJson;
        
        cout << "[AdminService] Returning success response" << endl;
        
        return HTTPServer::jsonSuccess(response);
    }
};

#endif // ADMIN_SERVICE_H