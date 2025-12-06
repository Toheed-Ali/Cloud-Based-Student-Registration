#ifndef TIMETABLE_H
#define TIMETABLE_H

#include "../database/database.h"
#include <vector>
#include <map>
#include <set>
#include <string>

using namespace std;

struct TimeSlot {
    int id;
    string day;
    string time;
};

struct Room {
    string id;
    int capacity;
};

struct Exam {
    string courseCode;
    int timeSlotID;
    string roomID;
};

class TimetableManager {
private:
    Database* db;
    map<string, set<string>> conflictGraph; // Adjacency list: Course -> Set of conflicting courses
    vector<string> courses; // List of all course codes
    map<string, int> courseColor; // Course -> Color (TimeSlot ID)
    
    vector<TimeSlot> timeSlots;
    vector<Room> rooms;
    vector<Exam> schedule;

    void buildConflictGraph();
    int getDegree(const string& courseCode);

public:
    TimetableManager(Database* database);
    
    void addTimeSlot(int id, string day, string time);
    void addRoom(string id, int capacity);
    
    void generateTimetable();
    vector<Exam> getSchedule();
    
    // For testing/debug
    bool hasConflict(const string& c1, const string& c2);
};

#endif
