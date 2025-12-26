void viewTimetable(APIClient& client, const string& studentID) {
    Student student;
    if (!client.getStudent(studentID, student)) {
        cout << "\n✗ Student not found!" << endl;
        return;
    }
    
    Timetable timetable;
    if (!client.studentViewTimetable(student.currentSemester, timetable) || timetable.schedule.empty()) {
        cout << "\n⚠ Timetable not generated yet. Contact admin." << endl;
        return;
    }
    
    cout << "\n========================================" << endl;
    cout << "   MY TIMETABLE - SEMESTER " << student.currentSemester << endl;
    cout << "========================================\n" << endl;
    
    // Build a 2D grid: [day][hour] -> course info
    string grid[5][5];  // 5 days × 5 time slots
    for (int d = 0; d < 5; d++) {
        for (int h = 0; h < 5; h++) {
            grid[d][h] = "-";
        }
    }
    
    // Fill grid with enrolled courses
    for (const auto& sc : timetable.schedule) {
        // Check if student is enrolled
        bool enrolled = false;
        for (const auto& courseID : student.enrolledCourses) {
            if (courseID == sc.courseID) {
                enrolled = true;
                break;
            }
        }
        
        if (enrolled) {
            // Place course in all its time slots
            for (const auto& slot : sc.slots) {
                grid[slot.day][slot.hour] = sc.courseID;
            }
        }
    }
    
    // Display grid
    const string days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    const string times[] = {"9:00-10:30", "10:30-12:00", "LUNCH", "13:00-14:30", "14:30-16:00", "16:00-17:30"};
    
    // Header
    cout << left << setw(14) << "Time";
    for (int d = 0; d < 5; d++) {
        cout << setw(12) << days[d];
    }
    cout << endl;
    cout << string(74, '=') << endl;
    
    // Rows for each time slot
    for (int h = 0; h < 5; h++) {
        // Time column
        if (h == 2) {
            // Insert lunch break row before slot 2
            cout << left << setw(14) << "12:00-13:00";
            for (int d = 0; d < 5; d++) {
                cout << setw(12) << "LUNCH";
            }
            cout << endl;
        }
        
        cout << left << setw(14) << times[h < 2 ? h : h + 1];  // Skip lunch in times array
        
        // Course columns
        for (int d = 0; d < 5; d++) {
            cout << setw(12) << grid[d][h];
        }
        cout << endl;
    }
    
    cout << "\n" << string(74, '=') << endl;
    cout << "Total Courses Enrolled: " << student.enrolledCourses.size() << "/5" << endl;
    cout << string(74, '=') << "\n" << endl;
}
