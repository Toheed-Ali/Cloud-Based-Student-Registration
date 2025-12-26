# Student Population Utility

## populate_all_semesters.exe

This utility automatically creates students for semesters 1, 2, 4, 5, 6, 7, 8 and enrolls them in their respective courses.

### What it does:
- **Semester 1 & 2:** Creates 50 students each from Batch 2025 (BSCS25xxx)
- **Semester 4:** Creates 50 students from Batch 2024 (BSCS24xxx)  
- **Semester 5 & 6:** Creates 50 students each from Batch 2023 (BSCS23xxx)
- **Semester 7 & 8:** Creates 50 students each from Batch 2022 (BSCS22xxx)
- **Semester 3:** Skipped (already populated manually)

### Student Details:
- **Student IDs:** BSCS[batch][number] (e.g., BSCS25201)
- **Emails:** bscs[batch][number]@itu.edu.pk
- **Names:** Random Pakistani names from a pool of 50 first names and 30 last names
- **Phone:** Unique numbers starting with 03xxxxxxxxx
- **Password:** 12345678 (for all students)

### Enrollment:
Each student is automatically enrolled in 5 courses for their semester:
- **Sem 1:** CS101, CS102, MATH101, ENG101, PHY101
- **Sem 2:** CS201, CS202, MATH201, MATH202, PHY201
- **Sem 3:** (Already enrolled)
- **Sem 4:** CS401, CS402, CS403, MATH401, MGT401
- **Sem 5:** CS501, CS502, CS503, CS504, ELEC501
- **Sem 6:** CS601, CS602, CS603, CS604, MGT601
- **Sem 7:** CS701, CS702, CS703, CS704, CS705
- **Sem 8:** CS801, CS802, CS803, CS804, CS805

### Usage:
```powershell
cd build\Release
.\populate_all_semesters.exe
```

### Output:
The utility will:
1. Create 350 students (50 per semester × 7 semesters)
2. Create user accounts for all students
3. Enroll each in 5 courses (1,750 total enrollments)
4. Show progress for each semester
5. Display final summary

### Note:
Run this ONCE to populate the entire university database!
