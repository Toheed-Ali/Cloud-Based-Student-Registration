#include "../database/DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// Helper function to split string by delimiter
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    cout << "=== Course Population Utility ===" << endl;
    cout << "Initializing database..." << endl;
    
    DatabaseManager db("./data");
    db.initialize();
    
    // Course data: courseID|courseName|semester|teacherID|studentIDs|enrollmentCount
    vector<string> courseData = {
        "CS101|Introduction to Programming|1|T1100||0",
        "CS102|Introduction to Computing|1|T1101||0",
        "CS201|Data Structures|2|T1205||0",
        "CS202|Object Oriented Programming|2|T1206||0",
        "CS301|Algorithms|3|T1300||0",
        "CS302|Database Systems|3|T1301||0",
        "CS303|Computer Organization|3|T1302||0",
        "CS401|Operating Systems|4|T1405||0",
        "CS402|Software Engineering|4|T1406||0",
        "CS403|Computer Networks|4|T1407||0",
        "CS501|Artificial Intelligence|5|T1500||0",
        "CS502|Web Technologies|5|T1501||0",
        "CS503|Theory of Computation|5|T1502||0",
        "CS504|Computer Graphics|5|T1503||0",
        "CS601|Machine Learning|6|T1605||0",
        "CS602|Mobile App Development|6|T1606||0",
        "CS603|Compiler Construction|6|T1607||0",
        "CS604|Information Security|6|T1608||0",
        "CS701|Cloud Computing|7|T1700||0",
        "CS702|Big Data Analytics|7|T1701||0",
        "CS703|Parallel Computing|7|T1702||0",
        "CS704|Human Computer Interaction|7|T1703||0",
        "CS705|Senior Project I|7|T1704||0",
        "CS801|Blockchain Technology|8|T1805||0",
        "CS802|Internet of Things|8|T1806||0",
        "CS803|Advanced Databases|8|T1807||0",
        "CS804|Senior Project II|8|T1808||0",
        "CS805|Professional Ethics|8|T1809||0",
        "ELEC501|Digital Logic Design|5|T1504||0",
        "ENG101|English Composition|1|T1103||0",
        "ENG301|Technical Writing|3|T1304||0",
        "MATH101|Calculus I|1|T1102||0",
        "MATH201|Calculus II|2|T1207||0",
        "MATH202|Discrete Mathematics|2|T1208||0",
        "MATH301|Linear Algebra|3|T1303||0",
        "MATH401|Probability and Statistics|4|T1408||0",
        "MGT401|Management Principles|4|T1409||0",
        "MGT601|Project Management|6|T1609||0",
        "PHY101|Physics I|1|T1104||0",
        "PHY201|Physics II|2|T1209||0"
    };
    
    int successCount = 0;
    int failCount = 0;
    
    cout << "\nAdding " << courseData.size() << " courses..." << endl;
    cout << "----------------------------------------" << endl;
    
    for (const auto& line : courseData) {
        vector<string> parts = split(line, '|');
        
        if (parts.size() < 4) {
            cerr << "ERROR: Invalid course data: " << line << endl;
            failCount++;
            continue;
        }
        
        Course course;
        course.courseID = parts[0];
        course.courseName = parts[1];
        course.semester = stoi(parts[2]);
        course.teacherID = parts[3];
        course.credits = 3; // Default credits
        course.currentEnrollmentCount = 0;
        // enrolledStudents will be empty initially
        
        // Check if course already exists
        Course existingCourse;
        if (db.getCourse(course.courseID, existingCourse)) {
            cout << "[SKIP] " << course.courseID << " - Already exists" << endl;
            continue;
        }
        
        // Add course
        if (db.addCourse(course)) {
            cout << "[OK] " << course.courseID << " - " << course.courseName 
                 << " (Semester " << course.semester << ")" << endl;
            successCount++;
        } else {
            cerr << "[FAIL] " << course.courseID << " - Failed to add" << endl;
            failCount++;
        }
    }
    
    cout << "----------------------------------------" << endl;
    cout << "Summary:" << endl;
    cout << "  Successfully added: " << successCount << endl;
    cout << "  Failed: " << failCount << endl;
    cout << "  Total courses: " << courseData.size() << endl;
    cout << "\nDone!" << endl;
    
    return 0;
}
