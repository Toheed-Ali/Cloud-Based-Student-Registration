#ifndef DATA_MODELS_H
#define DATA_MODELS_H

#include <string>
#include <vector>
#include <ctime>

using namespace std;

// User roles enum
enum class UserRole {
    ADMIN,
    STUDENT,
    TEACHER
};

// User structure for authentication
struct User {
    string userID;
    string email;
    string passwordHash;
    UserRole role;
    string name;
    
    User() : role(UserRole::STUDENT) {}
};

// Student structure
struct Student {
    string studentID;
    string email;
    string name;
    int currentSemester;  // 1-8
    vector<string> enrolledCourses;  // Max 5
    string contactInfo;
    time_t dateOfAdmission;
    
    Student() : currentSemester(1), dateOfAdmission(0) {}
};

// Teacher structure
struct Teacher {
    string teacherID;
    string email;
    string name;
    string assignedCourseID;  // Only one course
    string department;
    string contactInfo;
    
    Teacher() {}
};

// Course structure
struct Course {
    string courseID;
    string courseName;
    int semester;  // 1-8
    string teacherID;
    vector<string> enrolledStudents;  // Max 50
    int currentEnrollmentCount;
    
    Course() : semester(1), currentEnrollmentCount(0) {}
    
    // Determine sessions per week: CS courses get 3, others get 2
    int getRequiredSessions() const {
        if (courseID.length() >= 2 && courseID.substr(0, 2) == "CS") {
            return 3;  // CS courses: 3 sessions per week
        }
        return 2;  // MATH, MGT, ENG, PHY, ELEC: 2 sessions per week
    }
};

// Time slot structure (1.5-hour slots with lunch break)
struct TimeSlot {
    int day;   // 0-4 (Monday-Friday)
    int hour;  // 0-4 (5 slots: 9-10:30, 10:30-12, 13-14:30, 14:30-16, 16-17:30)
    
    TimeSlot() : day(0), hour(0) {}
    TimeSlot(int d, int h) : day(d), hour(h) {}
    
    // Get day name
    string getDayName() const {
        const string days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
        return (day >= 0 && day < 5) ? days[day] : "Unknown";
    }
    
    // Get time range string for 1.5-hour slots
    string getTimeString() const {
        const string times[] = {
            "9:00-10:30",    // Slot 0
            "10:30-12:00",   // Slot 1
            "13:00-14:30",   // Slot 2 (after lunch 12-1)
            "14:30-16:00",   // Slot 3
            "16:00-17:30"    // Slot 4
        };
        return (hour >= 0 && hour < 5) ? times[hour] : "Unknown";
    }
    
    bool operator==(const TimeSlot& other) const {
        return day == other.day && hour == other.hour;
    }
    
    // Required for std::set
    bool operator<(const TimeSlot& other) const {
        if (day != other.day) return day < other.day;
        return hour < other.hour;
    }
};

// Scheduled course in timetable (can have multiple sessions per week)
struct ScheduledCourse {
    string courseID;
    string courseName;
    string teacherID;
    string teacherName;
    int classroomID;  // 1-5
    vector<TimeSlot> slots;  // Multiple sessions (3 for CS, 2 for others)
    vector<string> studentIDs;
    
    ScheduledCourse() : classroomID(0) {}
};

// Timetable for one semester
struct Timetable {
    int semesterNumber;  // 1-8
    vector<ScheduledCourse> schedule;  // All scheduled courses for this semester
    
    Timetable() : semesterNumber(1) {}
};

// System configuration
struct SystemConfig {
    time_t registrationStartTime;
    time_t registrationEndTime;
    bool isRegistrationOpen;
    bool isTimetableGenerated;
    
    SystemConfig() : 
        registrationStartTime(0), 
        registrationEndTime(0), 
        isRegistrationOpen(false),
        isTimetableGenerated(false) {}
};

// Helper function to convert role to string
inline string roleToString(UserRole role) {
    switch(role) {
        case UserRole::ADMIN: return "ADMIN";
        case UserRole::STUDENT: return "STUDENT";
        case UserRole::TEACHER: return "TEACHER";
        default: return "UNKNOWN";
    }
}

// Helper function to convert string to role
inline UserRole stringToRole(const string& roleStr) {
    if (roleStr == "ADMIN") return UserRole::ADMIN;
    if (roleStr == "STUDENT") return UserRole::STUDENT;
    if (roleStr == "TEACHER") return UserRole::TEACHER;
    return UserRole::STUDENT;  // Default
}

#endif // DATA_MODELS_H
