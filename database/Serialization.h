#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "DataModels.h"

using namespace std;

// Helper functions for string serialization (since binary doesn't work well with std::string)

class Serializer {
public:
    // String helpers
    static string vectorToString(const vector<string>& vec);
    static vector<string> stringToVector(const string& str);
    
    // User serialization
    static string serializeUser(const User& user);
    static User deserializeUser(const string& str);
    
    // Student serialization
    static string serializeStudent(const Student& student);
    static Student deserializeStudent(const string& str);
    
    // Teacher serialization
    static string serializeTeacher(const Teacher& teacher);
    static Teacher deserializeTeacher(const string& str);
    
    // Course serialization
    static string serializeCourse(const Course& course);
    static Course deserializeCourse(const string& str);
    
    // Timetable serialization
    static string serializeTimetable(const Timetable& timetable);
    static Timetable deserializeTimetable(const string& str);
    
    // SystemConfig serialization
    static string serializeConfig(const SystemConfig& config);
    static SystemConfig deserializeConfig(const string& str);
    
private:
    static string escape(const string& str);
    static string unescape(const string& str);
    static vector<string> split(const string& str, char delimiter);
};

// ==================== Implementation ====================

inline string Serializer::escape(const string& str) {
    string result;
    for (char c : str) {
        if (c == '|') result += "\\|";
        else if (c == '\\') result += "\\\\";
        else if (c == '\n') result += "\\n";
        else result += c;
    }
    return result;
}

inline string Serializer::unescape(const string& str) {
    string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            if (str[i + 1] == '|') { result += '|'; i++; }
            else if (str[i + 1] == '\\') { result += '\\'; i++; }
            else if (str[i + 1] == 'n') { result += '\n'; i++; }
            else result += str[i];
        } else {
            result += str[i];
        }
    }
    return result;
}

inline vector<string> Serializer::split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

inline string Serializer::vectorToString(const vector<string>& vec) {
    if (vec.empty()) return "";
    string result;
    for (size_t i = 0; i < vec.size(); i++) {
        result += escape(vec[i]);
        if (i < vec.size() - 1) result += ",";
    }
    return result;
}

inline vector<string> Serializer::stringToVector(const string& str) {
    if (str.empty()) return {};
    vector<string> tokens = split(str, ',');
    for (auto& token : tokens) {
        token = unescape(token);
    }
    return tokens;
}

inline string Serializer::serializeUser(const User& user) {
    stringstream ss;
    ss << escape(user.userID) << "|"
       << escape(user.email) << "|"
       << escape(user.passwordHash) << "|"
       << static_cast<int>(user.role) << "|"
       << escape(user.name);
    return ss.str();
}

inline User Serializer::deserializeUser(const string& str) {
    User user;
    vector<string> parts = split(str, '|');
    if (parts.size() >= 5) {
        user.userID = unescape(parts[0]);
        user.email = unescape(parts[1]);
        user.passwordHash = unescape(parts[2]);
        user.role = static_cast<UserRole>(stoi(parts[3]));
        user.name = unescape(parts[4]);
    }
    return user;
}

inline string Serializer::serializeStudent(const Student& student) {
    stringstream ss;
    ss << escape(student.studentID) << "|"
       << escape(student.email) << "|"
       << escape(student.name) << "|"
       << student.currentSemester << "|"
       << vectorToString(student.enrolledCourses) << "|"
       << escape(student.contactInfo) << "|"
       << student.dateOfAdmission;
    return ss.str();
}

inline Student Serializer::deserializeStudent(const string& str) {
    Student student;
    vector<string> parts = split(str, '|');
    if (parts.size() >= 7) {
        student.studentID = unescape(parts[0]);
        student.email = unescape(parts[1]);
        student.name = unescape(parts[2]);
        student.currentSemester = stoi(parts[3]);
        student.enrolledCourses = stringToVector(parts[4]);
        student.contactInfo = unescape(parts[5]);
        student.dateOfAdmission = stoll(parts[6]);
    }
    return student;
}

inline string Serializer::serializeTeacher(const Teacher& teacher) {
    stringstream ss;
    ss << escape(teacher.teacherID) << "|"
       << escape(teacher.email) << "|"
       << escape(teacher.name) << "|"
       << escape(teacher.assignedCourseID) << "|"
       << escape(teacher.department) << "|"
       << escape(teacher.contactInfo);
    return ss.str();
}

inline Teacher Serializer::deserializeTeacher(const string& str) {
    Teacher teacher;
    vector<string> parts = split(str, '|');
    if (parts.size() >= 6) {
        teacher.teacherID = unescape(parts[0]);
        teacher.email = unescape(parts[1]);
        teacher.name = unescape(parts[2]);
        teacher.assignedCourseID = unescape(parts[3]);
        teacher.department = unescape(parts[4]);
        teacher.contactInfo = unescape(parts[5]);
    }
    return teacher;
}

inline string Serializer::serializeCourse(const Course& course) {
    stringstream ss;
    ss << escape(course.courseID) << "|"
       << escape(course.courseName) << "|"
       << course.semester << "|"
       << escape(course.teacherID) << "|"
       << vectorToString(course.enrolledStudents) << "|"
       << course.currentEnrollmentCount;
    return ss.str();
}

inline Course Serializer::deserializeCourse(const string& str) {
    Course course;
    vector<string> parts = split(str, '|');
    if (parts.size() >= 6) {
        course.courseID = unescape(parts[0]);
        course.courseName = unescape(parts[1]);
        course.semester = stoi(parts[2]);
        course.teacherID = unescape(parts[3]);
        course.enrolledStudents = stringToVector(parts[4]);
        course.currentEnrollmentCount = stoi(parts[5]);
    }
    return course;
}

inline string Serializer::serializeTimetable(const Timetable& timetable) {
    stringstream ss;
    ss << timetable.semesterNumber << "|" << timetable.schedule.size();
    for (const auto& sc : timetable.schedule) {
        ss << "|" << escape(sc.courseID)
           << "," << escape(sc.courseName)
           << "," << escape(sc.teacherID)
           << "," << escape(sc.teacherName)
           << "," << sc.classroomID
           << "," << sc.slots.size();  // Number of slots
        // Serialize all time slots
        for (const auto& slot : sc.slots) {
            ss << "," << slot.day << "," << slot.hour;
        }
        ss << "," << vectorToString(sc.studentIDs);
    }
    return ss.str();
}

inline Timetable Serializer::deserializeTimetable(const string& str) {
    Timetable timetable;
    vector<string> parts = split(str, '|');
    if (parts.size() >= 2) {
        timetable.semesterNumber = stoi(parts[0]);
        int count = stoi(parts[1]);
        for (int i = 0; i < count && i + 2 < parts.size(); i++) {
            vector<string> fields = split(parts[i + 2], ',');
            if (fields.size() >= 6) {
                ScheduledCourse sc;
                sc.courseID = unescape(fields[0]);
                sc.courseName = unescape(fields[1]);
                sc.teacherID = unescape(fields[2]);
                sc.teacherName = unescape(fields[3]);
                sc.classroomID = stoi(fields[4]);
                
                // Deserialize slots
                int numSlots = stoi(fields[5]);
                int fieldIdx = 6;
                for (int s = 0; s < numSlots && fieldIdx + 1 < fields.size(); s++) {
                    TimeSlot slot;
                    slot.day = stoi(fields[fieldIdx]);
                    slot.hour = stoi(fields[fieldIdx + 1]);
                    sc.slots.push_back(slot);
                    fieldIdx += 2;
                }
                
                // Deserialize student IDs (at fieldIdx)
                if (fieldIdx < fields.size()) {
                    sc.studentIDs = stringToVector(fields[fieldIdx]);
                }
                
                timetable.schedule.push_back(sc);
            }
        }
    }
    return timetable;
}

inline string Serializer::serializeConfig(const SystemConfig& config) {
    stringstream ss;
    ss << config.registrationStartTime << "|"
       << config.registrationEndTime << "|"
       << config.isRegistrationOpen << "|"
       << config.isTimetableGenerated;
    return ss.str();
}

inline SystemConfig Serializer::deserializeConfig(const string& str) {
    SystemConfig config;
    vector<string> parts = split(str, '|');
    if (parts.size() >= 4) {
        config.registrationStartTime = stoll(parts[0]);
        config.registrationEndTime = stoll(parts[1]);
        config.isRegistrationOpen = stoi(parts[2]);
        config.isTimetableGenerated = stoi(parts[3]);
    }
    return config;
}

#endif // SERIALIZATION_H
