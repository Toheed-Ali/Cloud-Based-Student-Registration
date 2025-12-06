$baseUrl = "http://localhost:8080"

# 1. Login
Write-Host "Testing Login..."
$loginBody = @{
    username = "2021-CS-001"
    password = "pass123"
} | ConvertTo-Json
$response = Invoke-RestMethod -Uri "$baseUrl/login" -Method Post -Body $loginBody -ContentType "application/json"
Write-Host "Login Response: $($response | ConvertTo-Json -Depth 2)"

# 2. Get Student
Write-Host "`nTesting Get Student..."
$response = Invoke-RestMethod -Uri "$baseUrl/student/2021-CS-001" -Method Get
Write-Host "Student Response: $($response | ConvertTo-Json -Depth 2)"

# 3. Add Course
Write-Host "`nTesting Add Course..."
$courseBody = @{
    courseCode = "CS202"
    courseName = "Data Structures"
    semester = 3
    teacherUsername = "prof_khan"
    creditHours = 3
    maxStudents = 50
} | ConvertTo-Json
try {
    $response = Invoke-RestMethod -Uri "$baseUrl/course" -Method Post -Body $courseBody -ContentType "application/json"
    Write-Host "Add Course Response: $($response | ConvertTo-Json -Depth 2)"
} catch {
    Write-Host "Add Course Failed: $_"
}

# 4. Enroll Student
Write-Host "`nTesting Enroll Student..."
$enrollBody = @{
    rollNo = "2021-CS-001"
    courseCode = "CS202"
    semester = 3
} | ConvertTo-Json
try {
    $response = Invoke-RestMethod -Uri "$baseUrl/enroll" -Method Post -Body $enrollBody -ContentType "application/json"
    Write-Host "Enroll Response: $($response | ConvertTo-Json -Depth 2)"
} catch {
    Write-Host "Enroll Failed: $_"
}
