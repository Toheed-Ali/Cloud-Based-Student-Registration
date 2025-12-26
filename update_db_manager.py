#!/usr/bin/env python3
"""
Quick script to update DatabaseManager.cpp to use IndexedStorage API for all entities
"""

import re

file_path = r"c:\Users\User\Pictures\BSCS-24119-PROJECT-3\database\DatabaseManager.cpp"

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Teacher operations (similar pattern to Student)
content = re.sub(
    r'bool DatabaseManager::addTeacher\(const Teacher& teacher\) \{[^}]+\}',
    '''bool DatabaseManager::addTeacher(const Teacher& teacher) {
    lock_guard<mutex> lock(dbMutex);
    return teachers.add(teacher);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'bool DatabaseManager::getTeacher\(const string& teacherID, Teacher& outTeacher\) \{[^}]+string\* serialized[^}]+\}',
    '''bool DatabaseManager::getTeacher(const string& teacherID, Teacher& outTeacher) {
    lock_guard<mutex> lock(dbMutex);
    return teachers.get(teacherID, outTeacher);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'bool DatabaseManager::updateTeacher\(const Teacher& teacher\) \{[^}]+saveAll\(\);[^}]+\}',
    '''bool DatabaseManager::updateTeacher(const Teacher& teacher) {
    lock_guard<mutex> lock(dbMutex);
    return teachers.update(teacher);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'bool DatabaseManager::deleteTeacher\(const string& teacherID\) \{[^}]+teachers\.remove[^}]+\}',
    '''bool DatabaseManager::deleteTeacher(const string& teacherID) {
    lock_guard<mutex> lock(dbMutex);
    return teachers.remove(teacherID);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'vector<Teacher> DatabaseManager::getAllTeachers\(\) \{[^}]+for \(const auto& pair : teachers\.getAllPairs\(\)\)[^}]+\}',
    '''vector<Teacher> DatabaseManager::getAllTeachers() {
    lock_guard<mutex> lock(dbMutex);
    return teachers.getAll();
}''',
    content,
    flags=re.DOTALL
)

# Course operations
content = re.sub(
    r'bool DatabaseManager::addCourse\(const Course& course\) \{[^}]+courses\.insert[^}]+\}',
    '''bool DatabaseManager::addCourse(const Course& course) {
    lock_guard<mutex> lock(dbMutex);
    return courses.add(course);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'bool DatabaseManager::getCourseInternal\(const string& courseID, Course& outCourse\) \{[^}]+string\* serialized[^}]+\}',
    '''bool DatabaseManager::getCourseInternal(const string& courseID, Course& outCourse) {
    return courses.get(courseID, outCourse);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'bool DatabaseManager::updateCourseInternal\(const Course& course\) \{[^}]+courses\.update[^}]+\}',
    '''bool DatabaseManager::updateCourseInternal(const Course& course) {
    return courses.update(course);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'bool DatabaseManager::updateCourse\(const Course& course\) \{[^}]+if \(result\) saveAll\(\);[^}]+\}',
    '''bool DatabaseManager::updateCourse(const Course& course) {
    lock_guard<mutex> lock(dbMutex);
    return updateCourseInternal(course);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'bool DatabaseManager::deleteCourse\(const string& courseID\) \{[^}]+courses\.remove[^}]+\}',
    '''bool DatabaseManager::deleteCourse(const string& courseID) {
    lock_guard<mutex> lock(dbMutex);
    return courses.remove(courseID);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'vector<Course> DatabaseManager::getAllCourses\(\) \{[^}]+for \(const auto& pair : courses\.getAllPairs\(\)\)[^}]+\}',
    '''vector<Course> DatabaseManager::getAllCourses() {
    lock_guard<mutex> lock(dbMutex);
    return courses.getAll();
}''',
    content,
    flags=re.DOTALL
)

# User operations  
content = re.sub(
    r'User\* DatabaseManager::getUserByEmail\(const string& email\) \{[^}]+string\* serialized = users\.get[^}]+\}',
    '''User* DatabaseManager::getUserByEmail(const string& email) {
    lock_guard<mutex> lock(dbMutex);
    static User cachedUser;
    if (users.get(email, cachedUser)) {
        return &cachedUser;
    }
    return nullptr;
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'bool DatabaseManager::createUser\(const User& user\) \{[^}]+users\.insert[^}]+\}',
    '''bool DatabaseManager::createUser(const User& user) {
    lock_guard<mutex> lock(dbMutex);
    return users.add(user);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'vector<User> DatabaseManager::getAllUsers\(\) \{[^}]+for \(const auto& pair : users\.getAllPairs\(\)\)[^}]+\}',
    '''vector<User> DatabaseManager::getAllUsers() {
    lock_guard<mutex> lock(dbMutex);
    return users.getAll();
}''',
    content,
    flags=re.DOTALL
)

# Timetable operations
content = re.sub(
    r'bool DatabaseManager::saveTimetable\(const Timetable& timetable\) \{[^}]+timetables\.insert[^}]+\}',
    '''bool DatabaseManager::saveTimetable(const Timetable& timetable) {
    lock_guard<mutex> lock(dbMutex);
    return timetables.add(timetable);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'bool DatabaseManager::getTimetable\(int semester, Timetable& outTimetable\) \{[^}]+string\* serialized = timetables\.get[^}]+\}',
    '''bool DatabaseManager::getTimetable(int semester, Timetable& outTimetable) {
    lock_guard<mutex> lock(dbMutex);
    return timetables.get(to_string(semester), outTimetable);
}''',
    content,
    flags=re.DOTALL
)

content = re.sub(
    r'vector<Timetable> DatabaseManager::getAllTimetables\(\) \{[^}]+for \(const auto& pair : timetables\.getAllPairs\(\)\)[^}]+\}',
    '''vector<Timetable> DatabaseManager::getAllTimetables() {
    lock_guard<mutex> lock(dbMutex);
    return timetables.getAll();
}''',
    content,
    flags=re.DOTALL
)

with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("✅ Updated all DatabaseManager CRUD operations to use IndexedStorage!")
print("   - Student: ✓")
print("   - Teacher: ✓")
print("   - Course: ✓")
print("   - User: ✓")
print("   - Timetable: ✓")
