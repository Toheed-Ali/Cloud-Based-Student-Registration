#include "timetable.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <set>

using namespace std;

TimetableManager::TimetableManager(Database* database) : db(database) {}

void TimetableManager::addTimeSlot(int id, string day, string time) {
    timeSlots.push_back({id, day, time});
}

void TimetableManager::addRoom(string id, int capacity) {
    rooms.push_back({id, capacity});
}

void TimetableManager::buildConflictGraph() {
    conflictGraph.clear();
    courses = db->getAllCourseCodes();
    
    // Initialize graph nodes
    for (const auto& code : courses) {
        conflictGraph[code] = set<string>();
    }
    
    vector<Enrollment> enrollments = db->getAllEnrollments();
    
    // Map: Student -> Vector of CourseCodes
    map<string, vector<string>> studentCourses;
    for (const auto& e : enrollments) {
        studentCourses[e.studentRollNo].push_back(e.courseCode);
    }
    
    // Build edges
    for (const auto& entry : studentCourses) {
        const vector<string>& sCourses = entry.second;
        for (size_t i = 0; i < sCourses.size(); ++i) {
            for (size_t j = i + 1; j < sCourses.size(); ++j) {
                string c1 = sCourses[i];
                string c2 = sCourses[j];
                
                if (c1 != c2) {
                    conflictGraph[c1].insert(c2);
                    conflictGraph[c2].insert(c1);
                }
            }
        }
    }
}

int TimetableManager::getDegree(const string& courseCode) {
    return conflictGraph[courseCode].size();
}

void TimetableManager::generateTimetable() {
    buildConflictGraph();
    schedule.clear();
    courseColor.clear();
    
    if (timeSlots.empty()) {
        cout << "Error: No time slots defined." << endl;
        return;
    }
    
    // Welsh-Powell Algorithm
    
    // 1. Sort courses by degree (descending)
    vector<string> sortedCourses = courses;
    sort(sortedCourses.begin(), sortedCourses.end(), [&](const string& a, const string& b) {
        return getDegree(a) > getDegree(b);
    });
    
    // 2. Assign colors (timeslots)
    for (const auto& course : sortedCourses) {
        // Try to assign the first available timeslot
        for (const auto& slot : timeSlots) {
            bool conflict = false;
            // Check neighbors
            for (const auto& neighbor : conflictGraph[course]) {
                if (courseColor.count(neighbor) && courseColor[neighbor] == slot.id) {
                    conflict = true;
                    break;
                }
            }
            
            if (!conflict) {
                courseColor[course] = slot.id;
                
                Exam exam;
                exam.courseCode = course;
                exam.timeSlotID = slot.id;
                exam.roomID = rooms.empty() ? "TBD" : rooms[0].id; // Simple room assignment for now
                schedule.push_back(exam);
                break;
            }
        }
        
        if (courseColor.find(course) == courseColor.end()) {
            cout << "Warning: Could not schedule course " << course << " due to insufficient time slots." << endl;
        }
    }
}

vector<Exam> TimetableManager::getSchedule() {
    return schedule;
}

bool TimetableManager::hasConflict(const string& c1, const string& c2) {
    return conflictGraph[c1].count(c2) > 0;
}
