#include "DatabaseManager.h"
#include "Serialization.h"
#include "../backend/utils/SHA256.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <filesystem>

using namespace std;

DatabaseManager::DatabaseManager(const string& dataDirectory) 
    : dataDir(dataDirectory),
      configFile(dataDirectory + "/config.dat"),
      users(dataDirectory + "/users"),
      students(dataDirectory + "/students"),
      teachers(dataDirectory + "/teachers"),
      courses(dataDirectory + "/courses"),
      timetables(dataDirectory + "/timetables") {
    ensureDataDirectory();
}

DatabaseManager::~DatabaseManager() {
    saveAll();
}

void DatabaseManager::ensureDataDirectory() {
    if (!fs::exists(dataDir)) {
        fs::create_directories(dataDir);
    }
}

string DatabaseManager::generateID(const string& prefix) {
    static int counter = 1000;
    return prefix + to_string(counter++);
}

bool DatabaseManager::initialize() {
    lock_guard<mutex> lock(dbMutex);
    
    // Load existing data (IndexedStorage loads automatically in constructor)
    
    // Create default admin if no users exist
    if (users.getAll().empty()) {
        User admin;
        admin.userID = "ADMIN001";
        admin.email = "admin@university.com";
        admin.passwordHash = SHA256::hash("admin123");  // Hash the password
        admin.role = UserRole::ADMIN;
        admin.name = "System Administrator";
        
        users.add(admin);  // No serialization needed!
        
        cout << "[DatabaseManager] Created default admin account" << endl;
        cout << "  Email: admin@university.com" << endl;
        cout << "  Password: admin123" << endl;
        
        // IMPORTANT: Save immediately so indexes are persisted
        saveAll();
    }
    
    return true;
}

bool DatabaseManager::loadAll() {
    // IndexedStorage loads automatically in constructor from .btree, .hash, and .dat files
    // We only need to load the config file manually
    
    try {
        if (fs::exists(configFile)) {
            ifstream in(configFile);
            if (in.is_open()) {
                string line;
                getline(in, line);
                if (!line.empty()) {
                    config = Serializer::deserializeConfig(line);
                }
                in.close();
            }
        }
        
        return true;
    } catch (const exception& e) {
        cerr << "[DatabaseManager] Error loading config: " << e.what() << endl;
        return false;
    }
}

bool DatabaseManager::saveAll() {
    // IndexedStorage saves automatically in destructor and on modifications
    // We only need to save the config file manually
    
    try {
        ofstream configOut(configFile);
        configOut << Serializer::serializeConfig(config) << "\n";
        configOut.close();
        
        return true;
    } catch (const exception& e) {
        cerr << "[DatabaseManager] Error saving config: " << e.what() << endl;
        return false;
    }
}

// ========== User Operations ==========

bool DatabaseManager::authenticateUser(const string& email, const string& password, User& outUser) {
    lock_guard<mutex> lock(dbMutex);
    
    if (!users.get(email, outUser)) {
        return false;
    }
    
    string hashedPassword = SHA256::hash(password);
    return outUser.passwordHash == hashedPassword;
}

bool DatabaseManager::createUser(const User& user) {
    lock_guard<mutex> lock(dbMutex);
    
    cout << "[DB] createUser called for: " << user.email << " (role: " << static_cast<int>(user.role) << ")" << endl;
    
    if (users.exists(user.email)) {
        cout << "[DB] User already exists!" << endl;
        return false;  // User already exists
    }
    
    cout << "[DB] Adding user to IndexedStorage..." << endl;
    bool result = users.add(user);
    cout << "[DB] User added successfully!" << endl;
    return result;
}

User* DatabaseManager::getUserByEmail(const string& email) {
    lock_guard<mutex> lock(dbMutex);
    static User cachedUser;
    if (users.get(email, cachedUser)) {
        return &cachedUser;
    }
    return nullptr;
}

bool DatabaseManager::updateUser(const User& user) {
    lock_guard<mutex> lock(dbMutex);
    return users.update(user);
}

bool DatabaseManager::deleteUser(const string& email) {
    lock_guard<mutex> lock(dbMutex);
    return users.remove(email);
}

vector<User> DatabaseManager::getAllUsers() {
    lock_guard<mutex> lock(dbMutex);
    return users.getAll();
}

// ========== Student Operations ==========

bool DatabaseManager::addStudent(const Student& student) {
    lock_guard<mutex> lock(dbMutex);
    return students.add(student);  // O(1) + B-Tree insert
}

// Internal unlocked version for use when lock is already held
bool DatabaseManager::getStudentInternal(const string& studentID, Student& outStudent) {
    return students.get(studentID, outStudent);  // O(1) hash lookup!
}

bool DatabaseManager::getStudent(const string& studentID, Student& outStudent) {
    lock_guard<mutex> lock(dbMutex);
    return getStudentInternal(studentID, outStudent);
}

// Internal unlocked version
bool DatabaseManager::updateStudentInternal(const Student& student) {
    return students.update(student);  // O(1) update
}

bool DatabaseManager::updateStudent(const Student& student) {
    lock_guard<mutex> lock(dbMutex);
    return updateStudentInternal(student);
}

bool DatabaseManager::deleteStudent(const string& studentID) {
    lock_guard<mutex> lock(dbMutex);
    
    // First get the student to access their data
    Student student;
    if (!getStudentInternal(studentID, student)) {
        return false;  // Student not found
    }
    
    // Remove student from all enrolled courses and update counts
    for (const auto& courseID : student.enrolledCourses) {
        Course course;
        if (getCourseInternal(courseID, course)) {
            // Remove student from course's enrolledStudents list
            auto it = find(course.enrolledStudents.begin(), course.enrolledStudents.end(), studentID);
            if (it != course.enrolledStudents.end()) {
                course.enrolledStudents.erase(it);
                course.currentEnrollmentCount--;
                updateCourseInternal(course);
            }
        }
    }
    
    // Delete the user account associated with this student
    if (!student.email.empty()) {
        users.remove(student.email);
    }
    
    // Finally delete the student record
    return students.remove(studentID);
}

vector<Student> DatabaseManager::getAllStudents() {
    lock_guard<mutex> lock(dbMutex);
    return students.getAll();  // Sorted by B-Tree!
}

vector<Student> DatabaseManager::getStudentsBySemester(int semester) {
    vector<Student> all = getAllStudents();
    vector<Student> result;
    
    for (const auto& student : all) {
        if (student.currentSemester == semester) {
            result.push_back(student);
        }
    }
    
    return result;
}

// ========== Teacher Operations ==========

bool DatabaseManager::addTeacher(const Teacher& teacher) {
    lock_guard<mutex> lock(dbMutex);
    return teachers.add(teacher);
}

bool DatabaseManager::getTeacher(const string& teacherID, Teacher& outTeacher) {
    lock_guard<mutex> lock(dbMutex);
    return teachers.get(teacherID, outTeacher);
}

bool DatabaseManager::updateTeacher(const Teacher& teacher) {
    lock_guard<mutex> lock(dbMutex);
    return teachers.update(teacher);
}

bool DatabaseManager::deleteTeacher(const string& teacherID) {
    lock_guard<mutex> lock(dbMutex);
    
    // First get the teacher to access their data
    Teacher teacher;
    if (!teachers.get(teacherID, teacher)) {
        return false;  // Teacher not found
    }
    
    // Delete the user account associated with this teacher
    if (!teacher.email.empty()) {
        users.remove(teacher.email);
    }
    
    // Delete the teacher record
    return teachers.remove(teacherID);
}

vector<Teacher> DatabaseManager::getAllTeachers() {
    lock_guard<mutex> lock(dbMutex);
    return teachers.getAll();
}

// ========== Course Operations ==========

bool DatabaseManager::addCourse(const Course& course) {
    lock_guard<mutex> lock(dbMutex);
    return courses.add(course);
}

// Internal unlocked version for use when lock is already held
bool DatabaseManager::getCourseInternal(const string& courseID, Course& outCourse) {
    return courses.get(courseID, outCourse);
}

bool DatabaseManager::getCourse(const string& courseID, Course& outCourse) {
    lock_guard<mutex> lock(dbMutex);
    return getCourseInternal(courseID, outCourse);
}

// Internal unlocked version  
bool DatabaseManager::updateCourseInternal(const Course& course) {
    return courses.update(course);
}

bool DatabaseManager::updateCourse(const Course& course) {
    lock_guard<mutex> lock(dbMutex);
    return updateCourseInternal(course);
}

bool DatabaseManager::deleteCourse(const string& courseID) {
    lock_guard<mutex> lock(dbMutex);
    return courses.remove(courseID);
}

vector<Course> DatabaseManager::getAllCourses() {
    lock_guard<mutex> lock(dbMutex);
    return courses.getAll();
}

vector<Course> DatabaseManager::getCoursesBySemester(int semester) {
    vector<Course> all = getAllCourses();
    vector<Course> result;
    
    for (const auto& course : all) {
        if (course.semester == semester) {
            result.push_back(course);
        }
    }
    
    return result;
}

// ========== Enrollment Operations ==========

bool DatabaseManager::canEnroll(const string& studentID, const string& courseID, string& errorMsg) {
    // NOTE: This function should be called with dbMutex already locked
    // Check if registration is open - use internal check without lock
    if (!config.isRegistrationOpen) {
        errorMsg = "Registration window is closed";
        return false;
    }
    
    time_t now = time(nullptr);
    if (!(now >= config.registrationStartTime && now <= config.registrationEndTime)) {
        errorMsg = "Registration window is closed";
        return false;
    }
    
    // Get student using new API (INTERNAL - no lock)
    Student student;
    if (!getStudentInternal(studentID, student)) {
        errorMsg = "Student not found";
        return false;
    }
    
    // Check if student already enrolled in 5 courses
    if (student.enrolledCourses.size() >= 5) {
        errorMsg = "Student already enrolled in maximum 5 courses";
        return false;
    }
    
    // Get course using new API (INTERNAL - no lock)
    Course course;
    if (!getCourseInternal(courseID, course)) {
        errorMsg = "Course not found";
        return false;
    }
    
    // Check if course is in student's semester
    if (course.semester != student.currentSemester) {
        errorMsg = "Course is not in student's current semester";
        return false;
    }
    
    // Check if course is full
    if (course.currentEnrollmentCount >= 50) {
        errorMsg = "Course is full (50 students maximum)";
        return false;
    }
    
    // Check if student already enrolled in this course
    if (find(student.enrolledCourses.begin(), student.enrolledCourses.end(), courseID) != student.enrolledCourses.end()) {
        errorMsg = "Student already enrolled in this course";
        return false;
    }
    
    return true;
}

bool DatabaseManager::enrollStudent(const string& studentID, const string& courseID) {
    lock_guard<mutex> lock(dbMutex);
    
    string errorMsg;
    if (!canEnroll(studentID, courseID, errorMsg)) {
        cerr << "[DatabaseManager] Enrollment failed: " << errorMsg << endl;
        return false;
    }
    
    // Get student and course using INTERNAL API (no re-locking)
    Student student;
    Course course;
    if (!getStudentInternal(studentID, student)) {
        return false;
    }
    
    if (!getCourseInternal(courseID, course)) {
        return false;
    }
    
    // Update student
    student.enrolledCourses.push_back(courseID);
    updateStudentInternal(student);
    
    // Update course
    course.enrolledStudents.push_back(studentID);
    course.currentEnrollmentCount++;
    updateCourseInternal(course);
    
    saveAll();
    return true;
}

bool DatabaseManager::dropCourse(const string& studentID, const string& courseID) {
    lock_guard<mutex> lock(dbMutex);
    
    // Check if registration window is open (same check as enrollment)
    if (!config.isRegistrationOpen) {
        cerr << "[DatabaseManager] Cannot drop course - registration window is closed" << endl;
        return false;
    }
    
    time_t now = time(nullptr);
    if (!(now >= config.registrationStartTime && now <= config.registrationEndTime)) {
        cerr << "[DatabaseManager] Cannot drop course - outside registration window" << endl;
        return false;
    }
    
    // Get student and course using INTERNAL API (no re-locking)
    Student student;
    if (!getStudentInternal(studentID, student)) {
        return false;
    }
    
    Course course;
    if (!getCourseInternal(courseID, course)) {
        return false;
    }
    
    // Remove from student's enrolledCourses
    auto it = find(student.enrolledCourses.begin(), student.enrolledCourses.end(), courseID);
    if (it == student.enrolledCourses.end()) {
        return false;  // Student not enrolled in this course
    }
    student.enrolledCourses.erase(it);
    updateStudentInternal(student);
    
    // Remove from course's enrolledStudents
    auto it2 = find(course.enrolledStudents.begin(), course.enrolledStudents.end(), studentID);
    if (it2 != course.enrolledStudents.end()) {
        course.enrolledStudents.erase(it2);
        course.currentEnrollmentCount--;
        updateCourseInternal(course);
    }
    
    saveAll();
    return true;
}

// ========== Timetable Operations ==========

bool DatabaseManager::saveTimetable(const Timetable& timetable) {
    lock_guard<mutex> lock(dbMutex);
    return timetables.add(timetable);
}

bool DatabaseManager::getTimetable(int semester, Timetable& outTimetable) {
    lock_guard<mutex> lock(dbMutex);
    return timetables.get(to_string(semester), outTimetable);
}

vector<Timetable> DatabaseManager::getAllTimetables() {
    lock_guard<mutex> lock(dbMutex);
    return timetables.getAll();
}

// ========== System Config Operations ==========

SystemConfig DatabaseManager::getConfig() {
    lock_guard<mutex> lock(dbMutex);
    return config;
}

void DatabaseManager::updateConfig(const SystemConfig& newConfig) {
    lock_guard<mutex> lock(dbMutex);
    
    config = newConfig;
    saveAll();
}

bool DatabaseManager::isRegistrationOpen() {
    lock_guard<mutex> lock(dbMutex);
    
    if (!config.isRegistrationOpen) {
        return false;
    }
    
    time_t now = time(nullptr);
    return (now >= config.registrationStartTime && now <= config.registrationEndTime);
}
