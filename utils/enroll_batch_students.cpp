#include <iostream>
#include <vector>
#include "../database/DatabaseManager.h"

using namespace std;

int main() {
    cout << "========================================" << endl;
    cout << "  Batch Student Enrollment Utility" << endl;
    cout << "========================================\n" << endl;
    
    DatabaseManager db("data");
    db.initialize();
    
    // Students to enroll (all semester 3 students)
    vector<string> studentIDs = {
        "BSCS24037", "BSCS24041", "BSCS24043", "BSCS24045", "BSCS24049",
        "BSCS24053", "BSCS24063", "BSCS24071", "BSCS24073", "BSCS24075",
        "BSCS24077", "BSCS24081", "BSCS24083", "BSCS24087", "BSCS24089",
        "BSCS24091", "BSCS24093", "BSCS24095", "BSCS24097", "BSCS24101",
        "BSCS24103", "BSCS24105", "BSCS24112", "BSCS24113", "BSCS24115",
        "BSCS24117", "BSCS24121", "BSCS24122", "BSCS24131", "BSCS24133",
        "BSCS24135", "BSCS24137", "BSCS24139", "BSCS24143", "BSCS24145",
        "BSCS24147", "BSCS24149", "BSCS24151", "BSCS24153", "BSCS24155"
    };
    
    // Semester 3 courses to enroll in
    vector<string> courseIDs = {
        "CS301",    // Algorithms
        "CS302",    // Database Systems
        "CS303",    // Computer Organization
        "ENG301",   // Technical Writing
        "MATH301"   // Linear Algebra
    };
    
    int totalEnrollments = 0;
    int failedEnrollments = 0;
    
    for (const auto& studentID : studentIDs) {
        cout << "\n--- Enrolling " << studentID << " ---" << endl;
        
        Student student;
        if (!db.getStudent(studentID, student)) {
            cout << "✗ Student " << studentID << " not found!" << endl;
            continue;
        }
        
        cout << "Student: " << student.name << " (Semester " << student.currentSemester << ")" << endl;
        
        for (const auto& courseID : courseIDs) {
            // Check if already enrolled
            bool alreadyEnrolled = false;
            for (const auto& enrolled : student.enrolledCourses) {
                if (enrolled == courseID) {
                    alreadyEnrolled = true;
                    break;
                }
            }
            
            if (alreadyEnrolled) {
                cout << "  ⚠ Already enrolled in " << courseID << endl;
                continue;
            }
            
            if (db.enrollStudent(studentID, courseID)) {
                cout << "  ✓ Enrolled in " << courseID << endl;
                totalEnrollments++;
                
                // Refresh student data
                db.getStudent(studentID, student);
            } else {
                cout << "  ✗ Failed to enroll in " << courseID << endl;
                failedEnrollments++;
            }
        }
    }
    
    cout << "\n========================================" << endl;
    cout << "  Enrollment Summary" << endl;
    cout << "========================================" << endl;
    cout << "Total successful enrollments: " << totalEnrollments << endl;
    cout << "Failed enrollments: " << failedEnrollments << endl;
    cout << "========================================\n" << endl;
    
    return 0;
}
