#include <iostream>
#include <vector>
#include "../database/DatabaseManager.h"
#include "../database/DataModels.h"

using namespace std;

int main() {
    cout << "========================================" << endl;
    cout << "   Course Population Utility" << endl;
    cout << "========================================\n" << endl;
    
    // Initialize database
    DatabaseManager db("data");
    db.initialize();
    
    // Course data: courseID, courseName, semester, teacherID, capacity
    vector<tuple<string, string, int, string, int>> courses = {
        {"CS101", "Introduction to Programming", 1, "T1100", 50},
        {"CS102", "Introduction to Computing", 1, "T1101", 50},
        {"CS201", "Data Structures", 2, "T1205", 50},
        {"CS202", "Object Oriented Programming", 2, "T1206", 50},
        {"CS301", "Algorithms", 3, "T1300", 50},
        {"CS302", "Database Systems", 3, "T1301", 50},
        {"CS303", "Computer Organization", 3, "T1302", 50},
        {"CS401", "Operating Systems", 4, "T1405", 50},
        {"CS402", "Software Engineering", 4, "T1406", 50},
        {"CS403", "Computer Networks", 4, "T1407", 50},
        {"CS501", "Artificial Intelligence", 5, "T1500", 50},
        {"CS502", "Web Technologies", 5, "T1501", 50},
        {"CS503", "Theory of Computation", 5, "T1502", 50},
        {"CS504", "Computer Graphics", 5, "T1503", 50},
        {"CS601", "Machine Learning", 6, "T1605", 50},
        {"CS602", "Mobile App Development", 6, "T1606", 50},
        {"CS603", "Compiler Construction", 6, "T1607", 50},
        {"CS604", "Information Security", 6, "T1608", 50},
        {"CS701", "Cloud Computing", 7, "T1700", 49},
        {"CS702", "Big Data Analytics", 7, "T1701", 50},
        {"CS703", "Parallel Computing", 7, "T1702", 50},
        {"CS704", "Human Computer Interaction", 7, "T1703", 50},
        {"CS705", "Senior Project I", 7, "T1704", 50},
        {"CS801", "Blockchain Technology", 8, "T1805", 50},
        {"CS802", "Internet of Things", 8, "T1806", 50},
        {"CS803", "Advanced Databases", 8, "T1807", 50},
        {"CS804", "Senior Project II", 8, "T1808", 50},
        {"CS805", "Professional Ethics", 8, "T1809", 50},
        {"ELEC501", "Digital Logic Design", 5, "T1504", 50},
        {"ENG101", "English Composition", 1, "T1103", 50},
        {"ENG301", "Technical Writing", 3, "T1304", 50},
        {"MATH101", "Calculus I", 1, "T1102", 50},
        {"MATH201", "Calculus II", 2, "T1207", 50},
        {"MATH202", "Discrete Mathematics", 2, "T1208", 50},
        {"MATH301", "Linear Algebra", 3, "T1303", 50},
        {"MATH401", "Probability and Statistics", 4, "T1408", 50},
        {"MGT401", "Management Principles", 4, "T1409", 50},
        {"MGT601", "Project Management", 6, "T1609", 50},
        {"PHY101", "Physics I", 1, "T1104", 50},
        {"PHY201", "Physics II", 2, "T1209", 50}
    };
    
    int added = 0;
    int skipped = 0;
    
    cout << "Adding " << courses.size() << " courses...\n" << endl;
    
    for (const auto& courseData : courses) {
        Course course;
        course.courseID = get<0>(courseData);
        course.courseName = get<1>(courseData);
        course.semester = get<2>(courseData);
        course.teacherID = get<3>(courseData);
        course.currentEnrollmentCount = 0;
        // Note: Course struct uses enrolledStudents vector, not a capacity field
        
        if (db.addCourse(course)) {
            cout << "✓ Added: " << course.courseID << " - " << course.courseName << endl;
            added++;
        } else {
            cout << "✗ Skipped: " << course.courseID << " (already exists or teacher not found)" << endl;
            skipped++;
        }
    }
    
    cout << "\n========================================" << endl;
    cout << "Summary:" << endl;
    cout << "  Courses added: " << added << endl;
    cout << "  Courses skipped: " << skipped << endl;
    cout << "========================================" << endl;
    
    return 0;
}
