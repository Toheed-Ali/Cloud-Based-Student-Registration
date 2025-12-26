#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "../database/DatabaseManager.h"

using namespace std;

// Student name pool
const vector<string> firstNames = {
    "Ahmed", "Ali", "Hassan", "Usman", "Bilal", "Hamza", "Omar", "Zain", "Faisal", "Imran",
    "Fahad", "Saad", "Adnan", "Asad", "Waseem", "Tariq", "Khalid", "Nadeem", "Rashid", "Naveed",
    "Ayesha", "Fatima", "Hira", "Sana", "Amna", "Zara", "Maha", "Laiba", "Nimra", "Anum",
    "Sara", "Aisha", "Mahnoor", "Aliza", "Rabia", "Hafsa", "Maryam", "Uzma", "Nida", "Sidra",
    "Arslan", "Danish", "Junaid", "Kamran", "Rizwan", "Shahzad", "Talha", "Waleed", "Yasir", "Zakir"
};

const vector<string> lastNames = {
    "Khan", "Ahmed", "Ali", "Hassan", "Malik", "Shah", "Butt", "Akhtar", "Raza", "Hussain",
    "Mahmood", "Sheikh", "Siddiqui", "Qureshi", "Mirza", "Haider", "Javed", "Iqbal", "Nawaz", "Sabir",
    "Aziz", "Bashir", "Farooq", "Karim", "Latif", "Naeem", "Rafiq", "Saleem", "Tahir", "Waqar"
};

struct SemesterInfo {
    int semester;
    int batchYear;
    int startNum;
    vector<string> courses;
};

int main() {
    cout << "========================================" << endl;
    cout << "  Populate All Semesters Utility" << endl;
    cout << "========================================\n" << endl;
    
    DatabaseManager db("data");
    db.initialize();
    
    // Define semester information
    vector<SemesterInfo> semesters = {
        {1, 25, 201, {"CS101", "CS102", "MATH101", "ENG101", "PHY101"}},
        {2, 25, 401, {"CS201", "CS202", "MATH201", "MATH202", "PHY201"}},
        {4, 24, 201, {"CS401", "CS402", "CS403", "MATH401", "MGT401"}},
        {5, 23, 201, {"CS501", "CS502", "CS503", "CS504", "ELEC501"}},
        {6, 23, 401, {"CS601", "CS602", "CS603", "CS604", "MGT601"}},
        {7, 22, 201, {"CS701", "CS702", "CS703", "CS704", "CS705"}},
        {8, 22, 401, {"CS801", "CS802", "CS803", "CS804", "CS805"}}
    };
    
    int totalStudents = 0;
    int totalEnrollments = 0;
    time_t currentTime = time(nullptr);
    
    for (const auto& semInfo : semesters) {
        cout << "\n========================================" << endl;
        cout << "Processing Semester " << semInfo.semester << " (Batch 20" << semInfo.batchYear << ")" << endl;
        cout << "========================================" << endl;
        
        int studentsAdded = 0;
        int enrollmentsAdded = 0;
        
        // Create 50 students for this semester
        for (int i = 0; i < 50; i++) {
            int studentNum = semInfo.startNum + i;
            
            // Generate student details
            string studentID = "BSCS" + to_string(semInfo.batchYear) + 
                             (studentNum < 100 ? "0" : "") + to_string(studentNum);
            string email = "bscs" + to_string(semInfo.batchYear) + 
                          (studentNum < 100 ? "0" : "") + to_string(studentNum) + "@itu.edu.pk";
            
            // Generate random name
            string firstName = firstNames[rand() % firstNames.size()];
            string lastName = lastNames[rand() % lastNames.size()];
            string name = firstName + " " + lastName;
            
            // Generate unique phone number
            string phone = "0" + to_string(300 + (rand() % 50)) + 
                          to_string(1000000 + (rand() % 9000000));
            
            // Create student
            Student student;
            student.studentID = studentID;
            student.email = email;
            student.name = name;
            student.currentSemester = semInfo.semester;
            student.contactInfo = phone;
            student.dateOfAdmission = currentTime - (8 - semInfo.semester) * 365 * 24 * 3600;
            
            if (!db.addStudent(student)) {
                cout << "  \u2717 Failed to add " << studentID << endl;
                continue;
            }
            
            // Create user account (password: 12345678)
            User user;
            user.userID = studentID;
            user.email = email;
            user.name = name;
            user.role = UserRole::STUDENT;
            user.passwordHash = "12345678"; // Simple password
            
            if (!db.createUser(user)) {
                cout << "  \u26a0 Failed to create user for " << studentID << endl;
            }
            
            studentsAdded++;
            
            // Enroll in semester courses
            for (const auto& courseID : semInfo.courses) {
                if (db.enrollStudent(studentID, courseID)) {
                    enrollmentsAdded++;
                } else {
                    // Course might not exist or already full
                }
            }
            
            // Progress indicator
            if ((i + 1) % 10 == 0) {
                cout << "  \u2713 Added " << (i + 1) << "/50 students..." << endl;
            }
        }
        
        cout << "\n  \u2713 Semester " << semInfo.semester << " Complete:" << endl;
        cout << "    Students: " << studentsAdded << endl;
        cout << "    Enrollments: " << enrollmentsAdded << endl;
        
        totalStudents += studentsAdded;
        totalEnrollments += enrollmentsAdded;
    }
    
    cout << "\n========================================" << endl;
    cout << "  FINAL SUMMARY" << endl;
    cout << "========================================" << endl;
    cout << "Total Students Created: " << totalStudents << endl;
    cout << "Total Course Enrollments: " << totalEnrollments << endl;
    cout << "========================================\n" << endl;
    
    return 0;
}
