# Cloud-Based Student Registration & Timetable Engine

🎓 **Advanced Data Structures Course Project** | BSCS-24119 | Information Technology University (ITU)

A high-performance university management system implementing **B-Trees** and **Hash Tables** for efficient student registration, course management, and automated timetable generation with conflict resolution.

---

## 🌟 Key Features

### ✅ **Core Functionality**
- **Student Registration System** with batch & semester management
- **Course Management** with enrollment tracking (max 50 students/course)
- **Teacher Assignment** to courses
- **Automated Timetable Generation** with backtracking algorithm
  - Conflict-free scheduling (no room/teacher/student clashes)
  - 1.5-hour class durations
  - 3 sessions/week for CS courses, 2 sessions/week for others
  - Mandatory lunch break (1:00 PM - 2:00 PM)
  - 5 classrooms, Monday-Friday scheduling

### 🚀 **Data Structures & Algorithms**
- **B-Trees (Order 5)**: Sorted indexing for students, courses, teachers
- **Hash Tables (101 buckets)**: O(1) lookups by ID/email
- **IndexedStorage**: Dual-index architecture combining both for optimal performance
- **Backtracking Algorithm**: Constraint satisfaction for timetable generation

### 🏗️ **Architecture**
```
┌─────────────────┐
│  Frontend CLI   │  (Student, Teacher, Admin interfaces)
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Backend Service │  (Authentication, Registration, Timetable Generation)
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Database Layer  │  (IndexedStorage with B-Tree + Hash Table)
└─────────────────┘
```

---

## 📊 Performance Highlights

| Operation | Before (Linear) | After (Indexed) | Speedup |
|-----------|-----------------|-----------------|---------|
| Student Lookup | O(n) ~400ms | **O(1) ~0.5ms** | **800x** |
| Sorted Iteration | O(n log n) | **O(n) via B-Tree** | Pre-sorted! |
| Update Operations | O(n) | **O(1)** | **400x** |

---

## 🛠️ Tech Stack

- **Language**: C++17
- **Build System**: CMake 3.10+
- **Data Structures**: Custom B-Tree & Hash Table implementations
- **Algorithms**: Backtracking, SHA-256 hashing
- **Architecture**: 3-tier (Frontend → Backend → Database)

---

## 📁 Project Structure

```
BSCS-24119-PROJECT-3/
├── backend/               # Business logic layer
│   ├── AdminService.h     # Admin operations
│   ├── StudentService.h   # Student registration/enrollment
│   ├── TeacherService.h   # Teacher management
│   └── TimetableGenerator.h  # Backtracking scheduler
│
├── database/              # Data persistence layer
│   ├── BTree.h            # B-Tree implementation (555 lines)
│   ├── HashTable.h        # Hash Table implementation (219 lines)
│   ├── IndexedStorage.h   # Dual-index wrapper (258 lines)
│   ├── DatabaseManager.h/cpp  # Main database interface
│   ├── DataModels.h       # Entity definitions
│   └── Serialization.h    # Data serialization
│
├── frontend/              # User interface layer
│   ├── admin_cli.cpp      # Admin CLI
│   ├── student_cli.cpp    # Student CLI
│   └── teacher_cli.cpp    # Teacher CLI
│
├── utils/                 # Utility programs
│   └── populate_all_semesters.cpp  # Test data generator
│
├── tests/                 # Unit tests
│   └── test_indexed_storage.cpp
│
└── CMakeLists.txt         # Build configuration
```

---

## 🚀 Quick Start

### Prerequisites
- CMake 3.10+
- C++17 compiler (GCC 7+, MSVC 2017+, Clang 5+)
- Windows/Linux/macOS

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/Toheed-Ali/Cloud-Based-Student-Registration.git
cd Cloud-Based-Student-Registration

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . --config Release

# Run admin CLI
cd Release
./admin_cli.exe

# Run student CLI
./student_cli.exe

# Run teacher CLI
./teacher_cli.exe
```

### Default Admin Credentials
```
Email: admin@university.com
Password: admin123
```

---

## 📖 Usage Examples

### Admin Operations
```bash
./admin_cli.exe
# 1. Add students (batch registration)
# 2. Add courses
# 3. Assign teachers to courses
# 4. Generate timetables for all semesters
```

### Student Operations
```bash
./student_cli.exe
# Login with student ID
# 1. Enroll in courses
# 2. View timetable
# 3. Drop courses
```

### Population Utility
```bash
# Generate 50 students for all semesters (1-8)
./populate_all_semesters.exe
```

---

## 🎯 Data Structures Deep Dive

### B-Tree Implementation
- **Order**: 5 (4-8 keys per node)
- **Operations**: Insert, Search, Remove, Traverse
- **Time Complexity**: O(log n) for all operations
- **Use Case**: Sorted iteration, range queries
- **File**: `database/BTree.h`

### Hash Table Implementation
- **Buckets**: 101 (prime number for better distribution)
- **Collision Resolution**: Chaining
- **Hash Function**: `std::hash` with modulo
- **Time Complexity**: O(1) average case
- **Use Case**: Fast lookups by ID/email
- **File**: `database/HashTable.h`

### IndexedStorage Architecture
```cpp
template<typename T>
class IndexedStorage {
    BTree<string, size_t> btree;        // ID → offset (sorted)
    HashTable<string, size_t> hashTable; // ID → offset (fast lookup)
    
    bool add(T entity);      // Adds to BOTH indexes
    bool get(string id);     // Uses hash (O(1))
    vector<T> getAll();      // Uses B-Tree (sorted)
};
```

---

## 🧪 Testing

```bash
# Run IndexedStorage tests
./test_indexed_storage.exe

# Expected output:
# ✓ Adding students...
# ✓ Found: Test Student 1 (hash lookup)
# ✓ Getting all students (B-Tree sorted)
# ✓ Updated name: Updated Student 1
# All tests passed!
```

---

## 📚 Documentation

- [Implementation Plan](implementation_plan.md) - Detailed B-Tree/Hashing implementation
- [Walkthrough](walkthrough.md) - Verification of DSA requirements
- [Task Checklist](task.md) - Development progress tracking
- [Population Utility README](utils/README_POPULATE.md) - Test data generation

---

## 🎓 Academic Context

**Course**: Data Structures & Algorithms  
**Institution**: Information Technology University (ITU)  
**Project Type**: Semester Project  
**Requirements Met**:
- ✅ 3-tier architecture (Frontend → Backend → Database)
- ✅ B-Tree implementation for indexing
- ✅ Hash Table implementation for fast lookups
- ✅ C++ implementation (except GUI)
- ✅ Complete CRUD operations
- ✅ Automated timetable generation with conflict resolution

---

## 👨‍💻 Developer

**Toheed Ali**  
BSCS-24119  
Information Technology University

---

## 📝 License

This project is developed for academic purposes as part of the DSA course at ITU.

---

## 🙏 Acknowledgments

- ITU Faculty for project requirements and guidance
- Data Structures & Algorithms course content
- CMake and C++ community resources
