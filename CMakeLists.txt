cmake_minimum_required(VERSION 3.10)
project(UniversityManagementSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Executable
# add_executable(university_system ${SOURCES})

# Testing (using simple custom tests for now, or GoogleTest if available, but sticking to simple as per plan)
enable_testing()
add_executable(btree_test tests/btree_test.cpp)
target_include_directories(btree_test PRIVATE src)

add_executable(hashtable_test tests/hashtable_test.cpp)
target_include_directories(hashtable_test PRIVATE src)

add_executable(database_test tests/database_test.cpp src/database/database.cpp)
target_include_directories(database_test PRIVATE src)

add_executable(server src/server/server.cpp src/database/database.cpp src/timetable/timetable.cpp)
target_include_directories(server PRIVATE src src/server)
if(WIN32)
    target_link_libraries(server ws2_32)
endif()

add_executable(timetable_test tests/timetable_test.cpp src/timetable/timetable.cpp src/database/database.cpp)
target_include_directories(timetable_test PRIVATE src)
