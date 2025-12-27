#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include <mutex>
#include <filesystem>
#include "IndexedStorage.h"
#include "DataModels.h"
#include "Serialization.h"

using namespace std;
namespace fs = std::filesystem;

class DatabaseManager {
private:
    // Data structures - using IndexedStorage for O(1) lookups + sorted iteration
    IndexedStorage<User> users;
    IndexedStorage<Student> students;
    IndexedStorage<Teacher> teachers;
    IndexedStorage<Course> courses;
    IndexedStorage<Timetable> timetables;
    SystemConfig config;
    
    // File paths
    string dataDir;
    string configFile;
    
    // Thread safety
    mutex dbMutex;
    
    // Helper methods
    void ensureDataDirectory();
    string generateID(const string& prefix);
    
public:
    DatabaseManager(const string& dataDirectory = "data");
    ~DatabaseManager();
    
    // Initialize database (create default admin if first run)
    bool initialize();
    
    // Load all data from disk
    bool loadAll();
    
    // Save all data to disk
    bool saveAll();
    
    // ========== User Operations ==========
    bool authenticateUser(const string& email, const string& password, User& outUser);
    bool createUser(const User& user);
    User* getUserByEmail(const string& email);
    bool updateUser(const User& user);
    bool deleteUser(const string& email);
    vector<User> getAllUsers();
    
    // ========== Student Operations ==========
    bool addStudent(const Student& student);
    bool getStudent(const string& studentID, Student& outStudent);  // Changed: returns bool, uses output param
    bool updateStudent(const Student& student);
    bool deleteStudent(const string& studentID);
    vector<Student> getAllStudents();
    vector<Student> getStudentsBySemester(int semester);
    
    // ========== Teacher Operations ==========
    bool addTeacher(const Teacher& teacher);
    bool getTeacher(const string& teacherID, Teacher& outTeacher);  // Changed: returns bool, uses output param
    bool updateTeacher(const Teacher& teacher);
    bool deleteTeacher(const string& teacherID);
    vector<Teacher> getAllTeachers();
    
    // ========== Course Operations ==========
    bool addCourse(const Course& course);
    bool getCourse(const string& courseID, Course& outCourse);  // Changed: returns bool, uses output param
    bool updateCourse(const Course& course);
    bool deleteCourse(const string& courseID);
    vector<Course> getAllCourses();
    vector<Course> getCoursesBySemester(int semester);
    
    // Enrollment operations
    bool enrollStudent(const string& studentID, const string& courseID);
    bool dropCourse(const string& studentID, const string& courseID);
    bool canEnroll(const string& studentID, const string& courseID, string& errorMsg);
    
    // ========== Timetable Operations ==========
    bool saveTimetable(const Timetable& timetable);
    bool getTimetable(int semester, Timetable& outTimetable);  // Changed: returns bool, uses output param
    vector<Timetable> getAllTimetables();
    void clearTimetables();
    
    // ========== System Configuration ==========
    SystemConfig getConfig();
    void updateConfig(const SystemConfig& config);
    bool isRegistrationOpen();
    
private:
    // Internal unlocked versions for use when lock is already held
    bool getStudentInternal(const string& studentID, Student& outStudent);
    bool getCourseInternal(const string& courseID, Course& outCourse);
    bool updateStudentInternal(const Student& student);
    bool updateCourseInternal(const Course& course);
};

#endif // DATABASE_MANAGER_H
