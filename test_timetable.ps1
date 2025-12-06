$baseUrl = "http://localhost:8080"

# 5. Generate Timetable
Write-Host "`nTesting Generate Timetable..."
try {
    $response = Invoke-RestMethod -Uri "$baseUrl/timetable" -Method Post
    Write-Host "Timetable Response: $($response | ConvertTo-Json -Depth 5)"
}
catch {
    Write-Host "Timetable Failed: $_"
}
