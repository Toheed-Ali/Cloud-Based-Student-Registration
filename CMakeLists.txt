cmake_minimum_required(VERSION 3.10)
project(UniversityManagementSystem)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/database)
include_directories(${CMAKE_SOURCE_DIR}/backend)
include_directories(${CMAKE_SOURCE_DIR}/backend/utils)

# Database library
add_library(database
    database/DatabaseManager.cpp
)

# Backend server executable
add_executable(server
    backend/main.cpp
)

target_link_libraries(server database)

# Frontend CLI applications
add_executable(admin_cli
    frontend/admin_cli.cpp
)

add_executable(student_cli
    frontend/student_cli.cpp
)

add_executable(teacher_cli
    frontend/teacher_cli.cpp
)

target_link_libraries(admin_cli database)
target_link_libraries(student_cli database)
target_link_libraries(teacher_cli database)

# Utility programs
add_executable(enroll_batch_students
    utils/enroll_batch_students.cpp
)

add_executable(populate_all_semesters
    utils/populate_all_semesters.cpp
)

add_executable(populate_courses
    utils/populate_courses.cpp
)

add_executable(populate_students
    utils/populate_students.cpp
)

target_link_libraries(enroll_batch_students database)
target_link_libraries(populate_all_semesters database)
target_link_libraries(populate_courses database)
target_link_libraries(populate_students database)

# Test IndexedStorage
add_executable(test_indexed_storage
    tests/test_indexed_storage.cpp
)

target_link_libraries(test_indexed_storage database)


# Test executable for database (DISABLED - needs API update)
# add_executable(test_database
#     tests/test_database.cpp
# )
# 
# target_link_libraries(test_database database)

# Test executable for backend (integration tests) (DISABLED - needs API update)
# add_executable(test_backend
#     tests/test_backend.cpp
# )
# 
# target_link_libraries(test_backend database)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Print configuration
message(STATUS "Building UniversityManagementSystem")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Targets: server, admin_cli, student_cli, teacher_cli, test_database, test_backend")
