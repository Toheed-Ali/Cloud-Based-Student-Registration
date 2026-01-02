#include "../database/DatabaseManager.h"
#include "../backend/utils/SHA256.h"
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
    cout << "=== Teacher Population Utility ===" << endl;
    cout << "Initializing database..." << endl;
    
    DatabaseManager db("./data");
    db.initialize();
    
    // Teacher data: teacherID|email|name|courseID|department|phone
    vector<string> teacherData = {
        "T1100|T1100@itu.edu.pk|teacher1|CS101|Computer Science|03214356789",
        "T1101|T1101@itu.edu.pk|teacher2|CS102|Computer Science|03221234567",
        "T1102|T1102@itu.edu.pk|teacher32|MATH101|Mathematics|03010000021",
        "T1103|T1103@itu.edu.pk|teacher30|ENG101|English|03010000019",
        "T1104|T1104@itu.edu.pk|teacher39|PHY101|Physics|03010000028",
        "T1205|T1205@itu.edu.pk|teacher3|CS201|Computer Science|02345678654",
        "T1206|T1206@itu.edu.pk|teacher4|CS202|Computer Science|03331234567",
        "T1207|T1207@itu.edu.pk|teacher33|MATH201|Mathematics|03010000022",
        "T1208|T1208@itu.edu.pk|teacher34|MATH202|Mathematics|03010000023",
        "T1209|T1209@itu.edu.pk|teacher40|PHY201|Physics|03010000029",
        "T1300|T1300@itu.edu.pk|teacher5|CS301|Computer Science|03237654321",
        "T1301|T1301@itu.edu.pk|teacher6|CS302|Computer Science|03337354321",
        "T1302|T1302@itu.edu.pk|teacher7|CS303|Computer Science|03337364321",
        "T1303|T1303@itu.edu.pk|teacher35|MATH301|Mathematics|03010000024",
        "T1304|T1304@itu.edu.pk|teacher31|ENG301|English|03010000020",
        "T1405|T1405@itu.edu.pk|teacher8|CS401|Computer Science|03137358321",
        "T1406|T1406@itu.edu.pk|teacher9|CS402|Computer Science|03237351321",
        "T1407|T1407@itu.edu.pk|teacher10|CS403|Computer Science|03437354321",
        "T1408|T1408@itu.edu.pk|teacher36|MATH401|Mathematics|03010000025",
        "T1409|T1409@itu.edu.pk|teacher37|MGT401|Management|03010000026",
        "T1500|T1500@itu.edu.pk|teacher11|CS501|Computer Science|03231674321",
        "T1501|T1501@itu.edu.pk|teacher12|CS502|Computer Science|03010000001",
        "T1502|T1502@itu.edu.pk|teacher13|CS503|Computer Science|03010000002",
        "T1503|T1503@itu.edu.pk|teacher14|CS504|Computer Science|03010000003",
        "T1504|T1504@itu.edu.pk|teacher29|ELEC501|Computer Science|03010000018",
        "T1605|T1605@itu.edu.pk|teacher15|CS601|Computer Science|03010000004",
        "T1606|T1606@itu.edu.pk|teacher16|CS602|Computer Science|03010000005",
        "T1607|T1607@itu.edu.pk|teacher17|CS603|Computer Science|03010000006",
        "T1608|T1608@itu.edu.pk|teacher18|CS604|Computer Science|03010000007",
        "T1609|T1609@itu.edu.pk|teacher38|MGT601|Management|03010000027",
        "T1700|T1700@itu.edu.pk|teacher19|CS701|Computer Science|03010000008",
        "T1701|T1701@itu.edu.pk|teacher20|CS702|Computer Science|03010000009",
        "T1702|T1702@itu.edu.pk|teacher21|CS703|Computer Science|03010000010",
        "T1703|T1703@itu.edu.pk|teacher22|CS704|Computer Science|03010000011",
        "T1704|T1704@itu.edu.pk|teacher23|CS705|Computer Science|03010000012",
        "T1805|T1805@itu.edu.pk|teacher24|CS801|Computer Science|03010000013",
        "T1806|T1806@itu.edu.pk|teacher25|CS802|Computer Science|03010000014",
        "T1807|T1807@itu.edu.pk|teacher26|CS803|Computer Science|03010000015",
        "T1808|T1808@itu.edu.pk|teacher27|CS804|Computer Science|03010000016",
        "T1809|T1809@itu.edu.pk|teacher28|CS805|Computer Science|03010000017"
    };
    
    int successCount = 0;
    int failCount = 0;
    
    cout << "\nAdding " << teacherData.size() << " teachers..." << endl;
    cout << "----------------------------------------" << endl;
    
    for (const auto& line : teacherData) {
        vector<string> parts = split(line, '|');
        
        if (parts.size() < 6) {
            cerr << "ERROR: Invalid teacher data: " << line << endl;
            failCount++;
            continue;
        }
        
        string teacherID = parts[0];
        string email = parts[1];
        string name = parts[2];
        string courseID = parts[3];
        string department = parts[4];
        string phone = parts[5];
        
        // Check if teacher already exists
        Teacher existingTeacher;
        if (db.getTeacher(teacherID, existingTeacher)) {
            cout << "[SKIP] " << teacherID << " - Already exists" << endl;
            continue;
        }
        
        // Create User account for teacher
        User user;
        user.userID = teacherID;
        user.email = email;
        user.passwordHash = SHA256::hash("teacher123"); // Default password
        user.role = UserRole::TEACHER;
        user.name = name;
        
        // Add user account
        if (!db.createUser(user)) {
            cerr << "[FAIL] " << teacherID << " - Failed to create user account" << endl;
            failCount++;
            continue;
        }
        
        // Create Teacher record
        Teacher teacher;
        teacher.teacherID = teacherID;
        teacher.email = email;
        teacher.name = name;
        teacher.assignedCourseID = courseID;
        teacher.department = department;
        teacher.contactInfo = phone;
        
        // Add teacher
        if (db.addTeacher(teacher)) {
            cout << "[OK] " << teacherID << " - " << name 
                 << " (" << department << ", " << courseID << ")" << endl;
            successCount++;
        } else {
            cerr << "[FAIL] " << teacherID << " - Failed to add teacher record" << endl;
            failCount++;
        }
    }
    
    cout << "----------------------------------------" << endl;
    cout << "Summary:" << endl;
    cout << "  Successfully added: " << successCount << endl;
    cout << "  Failed: " << failCount << endl;
    cout << "  Total teachers: " << teacherData.size() << endl;
    cout << "\nDefault password for all teachers: teacher123" << endl;
    cout << "\nDone!" << endl;
    
    return 0;
}
