#include "httplib.h"
#include "json.hpp"
#include "../database/database.h"
#include "../timetable/timetable.h"
#include <iostream>

using namespace httplib;
using json = nlohmann::json;
using namespace std;

int main() {
    Server svr;
    Database db;
    
    // Load existing data
    // db.load(); 
    // For now, generate sample data if empty or just generate for testing
    db.generateSampleData();

    // CORS (Optional, good for frontend)
    svr.set_pre_routing_handler([](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if (req.method == "OPTIONS") {
            res.status = 200;
            return Server::HandlerResponse::Handled;
        }
        return Server::HandlerResponse::Unhandled;
    });

    // Login
    svr.Post("/login", [&](const Request& req, Response& res) {
        try {
            auto j = json::parse(req.body);
            string username = j["username"];
            string password = j["password"];
            
            UserAuth* auth = db.authenticate(username, password);
            if (auth) {
                json response;
                response["status"] = "success";
                response["role"] = (auth->role == ADMIN ? "admin" : (auth->role == TEACHER ? "teacher" : "student"));
                response["id"] = auth->referenceID;
                res.set_content(response.dump(), "application/json");
            } else {
                res.status = 401;
                json response;
                response["status"] = "error";
                response["message"] = "Invalid credentials";
                res.set_content(response.dump(), "application/json");
            }
        } catch (...) {
            res.status = 400;
            res.set_content("{\"status\":\"error\", \"message\":\"Invalid JSON\"}", "application/json");
        }
    });

    // Get Student Profile
    svr.Get(R"(/student/(\d+-\w+-\d+))", [&](const Request& req, Response& res) {
        string rollNo = req.matches[1];
        Student* s = db.getStudent(rollNo);
        if (s) {
            json response;
            response["rollNumber"] = s->rollNumber;
            response["name"] = s->name;
            response["email"] = s->email;
            response["semester"] = s->currentSemester;
            response["cgpa"] = s->cgpa;
            res.set_content(response.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"status\":\"error\", \"message\":\"Student not found\"}", "application/json");
        }
    });

    // Get Teacher Profile
    svr.Get(R"(/teacher/(\w+))", [&](const Request& req, Response& res) {
        string username = req.matches[1];
        Teacher* t = db.getTeacher(username);
        if (t) {
            json response;
            response["username"] = t->username;
            response["name"] = t->name;
            response["email"] = t->email;
            res.set_content(response.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"status\":\"error\", \"message\":\"Teacher not found\"}", "application/json");
        }
    });

    // Add Course (Admin/Teacher)
    svr.Post("/course", [&](const Request& req, Response& res) {
        try {
            auto j = json::parse(req.body);
            Course c;
            c.courseCode = j["courseCode"];
            c.courseName = j["courseName"];
            c.semester = j["semester"];
            c.teacherUsername = j["teacherUsername"];
            c.creditHours = j["creditHours"];
            c.maxStudents = j["maxStudents"];
            c.enrolledCount = 0;

            if (db.addCourse(c)) {
                json response;
                response["status"] = "success";
                response["message"] = "Course added";
                res.set_content(response.dump(), "application/json");
            } else {
                res.status = 409; // Conflict
                json response;
                response["status"] = "error";
                response["message"] = "Course already exists";
                res.set_content(response.dump(), "application/json");
            }
        } catch (...) {
            res.status = 400;
            res.set_content("{\"status\":\"error\", \"message\":\"Invalid JSON\"}", "application/json");
        }
    });
    
    // Enroll Student
    svr.Post("/enroll", [&](const Request& req, Response& res) {
        try {
            auto j = json::parse(req.body);
            string rollNo = j["rollNo"];
            string courseCode = j["courseCode"];
            int semester = j["semester"];
            
            if (db.enrollStudent(rollNo, courseCode, semester)) {
                json response;
                response["status"] = "success";
                response["message"] = "Enrolled successfully";
                res.set_content(response.dump(), "application/json");
            } else {
                res.status = 400;
                json response;
                response["status"] = "error";
                response["message"] = "Enrollment failed (Student/Course not found or already enrolled)";
                res.set_content(response.dump(), "application/json");
            }
        } catch (...) {
            res.status = 400;
            res.set_content("{\"status\":\"error\", \"message\":\"Invalid JSON\"}", "application/json");
        }
    });

    // Generate Timetable
    svr.Post("/timetable", [&](const Request& req, Response& res) {
        try {
            TimetableManager tm(&db);
            
            // Define Time Slots (Hardcoded for now, or could be in request)
            tm.addTimeSlot(1, "Monday", "09:00 - 12:00");
            tm.addTimeSlot(2, "Monday", "13:00 - 16:00");
            tm.addTimeSlot(3, "Tuesday", "09:00 - 12:00");
            tm.addTimeSlot(4, "Tuesday", "13:00 - 16:00");
            tm.addTimeSlot(5, "Wednesday", "09:00 - 12:00");
            
            // Define Rooms
            tm.addRoom("R-101", 50);
            tm.addRoom("R-102", 50);
            
            tm.generateTimetable();
            vector<Exam> schedule = tm.getSchedule();
            
            json response;
            response["status"] = "success";
            json exams = json::array();
            for (const auto& exam : schedule) {
                json e;
                e["courseCode"] = exam.courseCode;
                e["timeSlotID"] = exam.timeSlotID;
                e["roomID"] = exam.roomID;
                exams.push_back(e);
            }
            response["schedule"] = exams;
            res.set_content(response.dump(), "application/json");
            
        } catch (...) {
            res.status = 500;
            res.set_content("{\"status\":\"error\", \"message\":\"Internal Server Error\"}", "application/json");
        }
    });

    cout << "Server starting at http://localhost:8080" << endl;
    svr.listen("0.0.0.0", 8080);
    
    return 0;
}
