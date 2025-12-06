# Phase 4: Complete Docker Deployment Guide for Windows

This guide walks you through building, deploying, and testing the University Management System using Docker Desktop on Windows.

---

## Prerequisites

### 1. Install Docker Desktop
1. Download Docker Desktop from https://www.docker.com/products/docker-desktop/
2. Install and restart your computer
3. Open Docker Desktop and ensure it's running (whale icon in system tray)
4. Verify installation:
   ```powershell
   docker --version
   ```
   Expected output: `Docker version 24.x.x` or similar

---

## Step 1: Build the Docker Image

### Using PowerShell (Recommended)

1. **Open PowerShell** (Run as Administrator if needed)

2. **Navigate to your project directory:**
   ```powershell
   cd C:\Users\User\Pictures\BSCS-24119-PROJECT-3
   ```

3. **Build the Docker image:**
   ```powershell
   docker build -t university-system .
   ```
   
   **What happens:**
   - Docker reads the `Dockerfile`
   - Stage 1: Downloads GCC image and builds your C++ application
   - Stage 2: Creates a minimal runtime image with just the executable
   - This takes 5-10 minutes on first run

4. **Verify the image was created:**
   ```powershell
   docker images | Select-String "university-system"
   ```
   
   Expected output:
   ```
   university-system   latest   abc123def456   2 minutes ago   200MB
   ```

### Using CMD

If you prefer CMD, use these commands instead:
```cmd
cd C:\Users\User\Pictures\BSCS-24119-PROJECT-3
docker build -t university-system .
docker images
```

---

## Step 2: Run the Container

### Option A: Using PowerShell/CMD

**Start the container:**
```powershell
docker run -d -p 8080:8080 --name uni-server university-system
```

**Explanation:**
- `-d` = Run in detached mode (background)
- `-p 8080:8080` = Map port 8080 on your PC to port 8080 in container
- `--name uni-server` = Give the container a friendly name
- `university-system` = The image to use

**Check if it's running:**
```powershell
docker ps
```

Expected output:
```
CONTAINER ID   IMAGE               STATUS         PORTS                  NAMES
abc123def456   university-system   Up 10 seconds  0.0.0.0:8080->8080/tcp uni-server
```

### Option B: Using Docker Desktop GUI

1. Open **Docker Desktop**
2. Go to **Containers** tab
3. You should see `uni-server` listed with status "Running"
4. Click on it to view logs and details

---

## Step 3: Test the Deployment

### Test 1: Check Server Health

**Open a new PowerShell window and run:**
```powershell
curl http://localhost:8080/student/2021-CS-001
```

**Expected Response:**
```json
{
  "rollNumber": "2021-CS-001",
  "name": "Ahmed Ali",
  "email": "ahmed@test.com",
  "semester": 1,
  "cgpa": 3.5
}
```

If `curl` doesn't work, use:
```powershell
Invoke-RestMethod -Uri http://localhost:8080/student/2021-CS-001
```

### Test 2: Run Full API Test Suite

**Using the provided test script:**
```powershell
cd C:\Users\User\Pictures\BSCS-24119-PROJECT-3
powershell -ExecutionPolicy Bypass -File test_api.ps1
```

**Expected Output:**
```
Testing Login...
Login Response: { "status": "success", "role": "student", "id": "2021-CS-001" }

Testing Get Student...
Student Response: { ... }

Testing Add Course...
Add Course Response: { "status": "success", "message": "Course added" }

Testing Enroll Student...
Enroll Response: { "status": "success", "message": "Enrolled successfully" }
```

### Test 3: Test Timetable Generation

```powershell
powershell -ExecutionPolicy Bypass -File test_timetable.ps1
```

**Expected Output:**
```
Testing Generate Timetable...
Timetable Response: {
  "status": "success",
  "schedule": [
    { "courseCode": "CS101", "timeSlotID": 1, "roomID": "R-101" }
  ]
}
```

---

## Step 4: View Logs (Troubleshooting)

### Using PowerShell
```powershell
docker logs uni-server
```

Expected output should include:
```
Server starting at http://localhost:8080
```

### Using Docker Desktop
1. Open **Docker Desktop**
2. Click on **Containers**
3. Click on `uni-server`
4. Click **Logs** tab to see real-time output

---

## Step 5: Stop and Remove Container

### Stop the container:
```powershell
docker stop uni-server
```

### Remove the container:
```powershell
docker rm uni-server
```

### Restart fresh:
```powershell
docker run -d -p 8080:8080 --name uni-server university-system
```

---

## Common Issues & Solutions

### Issue 1: "Port 8080 is already in use"
**Solution:**
```powershell
# Find what's using port 8080
netstat -ano | findstr :8080

# Stop the old container
docker stop uni-server
docker rm uni-server

# Or use a different port
docker run -d -p 9000:8080 --name uni-server university-system
# Then test with http://localhost:9000
```

### Issue 2: "Cannot connect to Docker daemon"
**Solution:**
1. Open Docker Desktop
2. Wait for it to fully start (whale icon should be steady)
3. Try the command again

### Issue 3: Build fails with "no such file or directory"
**Solution:**
```powershell
# Make sure you're in the project root
cd C:\Users\User\Pictures\BSCS-24119-PROJECT-3

# Verify Dockerfile exists
ls Dockerfile

# Try building again
docker build -t university-system .
```

### Issue 4: "Connection refused" when testing
**Solution:**
```powershell
# Check if container is running
docker ps

# Check container logs for errors
docker logs uni-server

# Restart the container
docker restart uni-server

# Wait 5 seconds, then test again
Start-Sleep -Seconds 5
curl http://localhost:8080/student/2021-CS-001
```

---

## Verification Checklist

- [ ] Docker Desktop is installed and running
- [ ] Image `university-system` appears in `docker images`
- [ ] Container `uni-server` appears in `docker ps`
- [ ] Can access http://localhost:8080/student/2021-CS-001
- [ ] All test scripts pass successfully
- [ ] Server logs show no errors

---

## Next Steps

Once everything works locally:
1. **Push to Docker Hub** (for cloud deployment):
   ```powershell
   docker tag university-system yourusername/university-system
   docker push yourusername/university-system
   ```

2. **Deploy to Cloud** (AWS, DigitalOcean, etc.):
   - Follow instructions in `DEPLOY.md`

---

## Quick Reference Commands

```powershell
# Build image
docker build -t university-system .

# Run container
docker run -d -p 8080:8080 --name uni-server university-system

# View running containers
docker ps

# View logs
docker logs uni-server

# Stop container
docker stop uni-server

# Remove container
docker rm uni-server

# Remove image
docker rmi university-system

# Test API
curl http://localhost:8080/student/2021-CS-001

# Run test scripts
powershell -ExecutionPolicy Bypass -File test_api.ps1
powershell -ExecutionPolicy Bypass -File test_timetable.ps1
```
