#ifndef TIMETABLE_GENERATOR_H
#define TIMETABLE_GENERATOR_H

#include "../database/DatabaseManager.h"
#include "../database/DataModels.h"
#include <vector>
#include <set>
#include <map>
#include <iostream>

using namespace std;

class TimetableGenerator {
private:
    DatabaseManager& db;
    
    // Track resource usage during generation
    map<int, set<TimeSlot>> teacherSchedule;     // teacherID -> occupied slots
    map<int, set<TimeSlot>> classroomSchedule;   // classroomID -> occupied slots
    map<string, set<TimeSlot>> studentSchedule;   // studentID -> occupied slots
    
    // Check if a time slot is valid for a course
    bool isSlotValid(const Course& course, const TimeSlot& slot, int classroomID) {
        // Check if teacher is busy
        int teacherHash = hash<string>{}(course.teacherID) % 1000;
        if (teacherSchedule[teacherHash].count(slot) > 0) {
            return false;
        }
        
        // Check if classroom is busy
        if (classroomSchedule[classroomID].count(slot) > 0) {
            return false;
        }
        
        // Check if any enrolled student is busy
        for (const string& studentID : course.enrolledStudents) {
            if (studentSchedule[studentID].count(slot) > 0) {
                return false;
            }
        }
        
        return true;
    }
    
    // Schedule a course at a specific slot
    void scheduleCourse(const Course& course, const TimeSlot& slot, int classroomID) {
        int teacherHash = hash<string>{}(course.teacherID) % 1000;
        teacherSchedule[teacherHash].insert(slot);
        classroomSchedule[classroomID].insert(slot);
        
        for (const string& studentID : course.enrolledStudents) {
            studentSchedule[studentID].insert(slot);
        }
    }
    
    // Unschedule a course (for backtracking)
    void unscheduleCourse(const Course& course, const TimeSlot& slot, int classroomID) {
        int teacherHash = hash<string>{}(course.teacherID) % 1000;
        teacherSchedule[teacherHash].erase(slot);
        classroomSchedule[classroomID].erase(slot);
        
        for (const string& studentID : course.enrolledStudents) {
            studentSchedule[studentID].erase(slot);
        }
    }
    
    // Helper: Try to find N valid slots for a course
    bool findSlotsForCourse(const Course& course, int numSlots, int classroom,
                           vector<TimeSlot>& selectedSlots) {
        vector<TimeSlot> candidates;
        
        // Find all valid slots for this course (5 days × 5 slots = 25)
        for (int day = 0; day < 5; day++) {
            for (int hour = 0; hour < 5; hour++) {  // 0-4 (5 slots per day)
                TimeSlot slot(day, hour);
                if (isSlotValid(course, slot, classroom)) {
                    candidates.push_back(slot);
                }
            }
        }
        
        if (candidates.size() < numSlots) {
            return false;  // Not enough valid slots
        }
        
        // IMPROVED: Group slots by day to enable intelligent distribution
        map<int, vector<TimeSlot>> slotsByDay;
        for (const auto& slot : candidates) {
            slotsByDay[slot.dayOfWeek].push_back(slot);
        }
        
        // Strategy: Spread sessions across different days when possible
        selectedSlots.clear();
        
        // First, try to pick one slot from each different day
        for (const auto& pair : slotsByDay) {
            if (selectedSlots.size() >= numSlots) break;
            selectedSlots.push_back(pair.second[0]); // Pick first available slot from this day
        }
        
        // If we still need more slots (e.g., need 3 slots but only 2 days available),
        // fill remaining slots from any available day
        if (selectedSlots.size() < numSlots) {
            for (const auto& slot : candidates) {
                // Check if this slot is not already selected
                bool alreadySelected = false;
                for (const auto& selected : selectedSlots) {
                    if (selected.dayOfWeek == slot.dayOfWeek && selected.startHour == slot.startHour) {
                        alreadySelected = true;
                        break;
                    }
                }
                
                if (!alreadySelected) {
                    selectedSlots.push_back(slot);
                    if (selectedSlots.size() >= numSlots) break;
                }
            }
        }
        
        return selectedSlots.size() == numSlots;
    }
    
    // Backtracking algorithm for multi-session courses
    bool backtrack(vector<Course>& courses, int courseIndex, 
                   map<int, Timetable>& timetables) {
        // Base case: all courses scheduled
        if (courseIndex >= courses.size()) {
            cout << "[Backtrack] SUCCESS - All " << courses.size() << " courses scheduled!" << endl;
            return true;
        }
        
        Course& course = courses[courseIndex];
        int requiredSessions = course.getRequiredSessions();
        
        cout << "[Backtrack] Course " << (courseIndex+1) << "/" << courses.size() 
             << ": " << course.courseID << " (needs " << requiredSessions << " sessions)" << endl;
        
        // Skip courses with no enrollments
        if (course.enrolledStudents.empty()) {
            cout << "[Backtrack] Skipping " << course.courseID << " - no students" << endl;
            return backtrack(courses, courseIndex + 1, timetables);
        }
        
        // Try all classrooms
        for (int classroom = 1; classroom <= 5; classroom++) {
            vector<TimeSlot> selectedSlots;
            
            // Try to find required number of slots for this course
            if (findSlotsForCourse(course, requiredSessions, classroom, selectedSlots)) {
                // Schedule all sessions
                for (const auto& slot : selectedSlots) {
                    scheduleCourse(course, slot, classroom);
                }
                
                // Create scheduled course entry
                ScheduledCourse sc;
                sc.courseID = course.courseID;
                sc.courseName = course.courseName;
                sc.teacherID = course.teacherID;
                
                // Get teacher name
                Teacher teacher;
                if (!db.getTeacher(course.teacherID, teacher)) {
                    sc.teacherName = "Unknown";
                } else {
                    sc.teacherName = teacher.name;
                }
                
                sc.classroomID = classroom;
                sc.slots = selectedSlots;  // Multiple slots
                sc.studentIDs = course.enrolledStudents;
                
                // Add to timetable
                timetables[course.semester].schedule.push_back(sc);
                timetables[course.semester].semesterNumber = course.semester;
                
                // Recursively schedule remaining courses
                if (backtrack(courses, courseIndex + 1, timetables)) {
                    return true;  // Success!
                }
                
                // Backtrack: remove all sessions
                for (const auto& slot : selectedSlots) {
                    unscheduleCourse(course, slot, classroom);
                }
                timetables[course.semester].schedule.pop_back();
            }
        }
        
        // No valid configuration found
        cout << "[Backtrack] FAILED - Could not schedule " << course.courseID << endl;
        return false;
    }
    
public:
    TimetableGenerator(DatabaseManager& database) : db(database) {}
    
    // Generate timetables for all semesters
    bool generateAll() {
        cout << "[TimetableGenerator] Starting timetable generation..." << endl;
        
        // Clear existing schedules
        teacherSchedule.clear();
        classroomSchedule.clear();
        studentSchedule.clear();
        
        // Get all courses
        vector<Course> courses = db.getAllCourses();
        
        if (courses.empty()) {
            cout << "[TimetableGenerator] No courses found" << endl;
            return false;
        }
        
        cout << "[TimetableGenerator] Found " << courses.size() << " courses" << endl;
        
        // Sort courses by enrollment count (descending) - schedule fuller courses first
        sort(courses.begin(), courses.end(), 
             [](const Course& a, const Course& b) {
                 return a.currentEnrollmentCount > b.currentEnrollmentCount;
             });
        
        // Create timetable map
        map<int, Timetable> timetables;
        for (int sem = 1; sem <= 8; sem++) {
            timetables[sem].semesterNumber = sem;
        }
        
        // Run backtracking algorithm
        if (!backtrack(courses, 0, timetables)) {
            cout << "[TimetableGenerator] Failed to generate conflict-free timetable" << endl;
            return false;
        }
        
        // Save all timetables to database
        for (auto& pair : timetables) {
            if (!pair.second.schedule.empty()) {
                db.saveTimetable(pair.second);
                cout << "[TimetableGenerator] Saved timetable for semester " 
                     << pair.first << " (" << pair.second.schedule.size() 
                     << " courses)" << endl;
            }
        }
        
        // Update system config
        SystemConfig config = db.getConfig();
        config.isTimetableGenerated = true;
        db.updateConfig(config);
        
        cout << "[TimetableGenerator] Timetable generation completed successfully!" << endl;
        return true;
    }
    
    // API endpoint: POST /api/admin/generateTimetable
    static HTTPResponse generateTimetableAPI(const HTTPRequest& req, DatabaseManager& db) {
        TimetableGenerator generator(db);
        
        if (generator.generateAll()) {
            map<string, string> response;
            response["success"] = "true";
            response["message"] = "Timetable generated successfully";
            return HTTPServer::jsonSuccess(response);
        } else {
            return HTTPServer::jsonError("Failed to generate timetable");
        }
    }
};

#endif // TIMETABLE_GENERATOR_H
