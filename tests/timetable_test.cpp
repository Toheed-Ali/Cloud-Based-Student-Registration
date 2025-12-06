#include "../src/timetable/timetable.h"
#include "../src/database/database.h"
#include <iostream>
#include <cassert>

using namespace std;

void testConflictGraph() {
    cout << "Testing Conflict Graph..." << endl;
    Database db;
    // Setup data
    // Course A, B, C
    db.addCourse({"CS101", "Intro", 1, "T1", 3, 50, 0});
    db.addCourse({"CS102", "PF", 1, "T1", 3, 50, 0});
    db.addCourse({"CS103", "OOP", 2, "T2", 3, 50, 0});
    
    // Student 1 takes A and B (Conflict A-B)
    db.addStudent({"S1", "Student 1", "s1@test.com", "pass", 1, 3.0, STUDENT});
    db.enrollStudent("S1", "CS101", 1);
    db.enrollStudent("S1", "CS102", 1);
    
    // Student 2 takes B and C (Conflict B-C)
    db.addStudent({"S2", "Student 2", "s2@test.com", "pass", 1, 3.0, STUDENT});
    db.enrollStudent("S2", "CS102", 1);
    db.enrollStudent("S2", "CS103", 2);
    
    TimetableManager tm(&db);
    tm.generateTimetable(); // Builds graph internally
    
    assert(tm.hasConflict("CS101", "CS102") == true);
    assert(tm.hasConflict("CS102", "CS103") == true);
    assert(tm.hasConflict("CS101", "CS103") == false); // No common student
    
    cout << "Conflict Graph Passed!" << endl;
}

void testScheduling() {
    cout << "Testing Scheduling..." << endl;
    Database db;
    db.addCourse({"C1", "C1", 1, "T", 3, 50, 0});
    db.addCourse({"C2", "C2", 1, "T", 3, 50, 0});
    
    // Conflict C1-C2
    db.addStudent({"S1", "S1", "e", "p", 1, 0, STUDENT});
    db.enrollStudent("S1", "C1", 1);
    db.enrollStudent("S1", "C2", 1);
    
    TimetableManager tm(&db);
    tm.addTimeSlot(1, "Mon", "9:00");
    tm.addTimeSlot(2, "Mon", "12:00");
    
    tm.generateTimetable();
    
    vector<Exam> schedule = tm.getSchedule();
    assert(schedule.size() == 2);
    
    int slot1 = -1, slot2 = -1;
    for(const auto& exam : schedule) {
        if (exam.courseCode == "C1") slot1 = exam.timeSlotID;
        if (exam.courseCode == "C2") slot2 = exam.timeSlotID;
    }
    
    assert(slot1 != -1 && slot2 != -1);
    assert(slot1 != slot2); // Should be different slots due to conflict
    
    cout << "Scheduling Passed!" << endl;
}

int main() {
    testConflictGraph();
    testScheduling();
    cout << "All Timetable Tests Passed!" << endl;
    return 0;
}
