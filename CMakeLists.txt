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

# Utility: populate_courses
add_executable(populate_courses
    utils/populate_courses.cpp
)

target_link_libraries(populate_courses database)

# Utility: populate_teachers
add_executable(populate_teachers
    utils/populate_teachers.cpp
)

target_link_libraries(populate_teachers database)

# Utility: populate_students
add_executable(populate_students
    utils/populate_students.cpp
)

target_link_libraries(populate_students database)

# Utility: enroll_students
add_executable(enroll_students
    utils/enroll_students.cpp
)

target_link_libraries(enroll_students database)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Print configuration
message(STATUS "Building UniversityManagementSystem")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Targets: server, admin_cli, student_cli, teacher_cli, test_database, test_backend")
