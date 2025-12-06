# Frontend Testing Guide

## Prerequisites
- Docker container `uni-server` must be running on port 8080
- Web browser (Chrome, Firefox, Edge)

## Starting the Frontend

### Option 1: Simple HTTP Server (Python)
```bash
cd C:\Users\User\Pictures\BSCS-24119-PROJECT-3\frontend
python -m http.server 3000
```
Then open: http://localhost:3000

### Option 2: Live Server (VS Code Extension)
1. Install "Live Server" extension in VS Code
2. Right-click on `index.html`
3. Select "Open with Live Server"

### Option 3: Direct File Open
Simply double-click `index.html` (May have CORS issues)

## Testing Each Portal

### 1. Login Page (index.html)
**URL:** http://localhost:3000/index.html

**Test Credentials:**
- **Admin:**
  - Username: `admin`
  - Password: `admin123`

- **Student:** 
  - Username: `2021-CS-001`
  - Password: `pass123`
  
- **Teacher:**
  - Username: `prof_khan`
  - Password: `teach123`

**What to Test:**
- ✅ Login form appears
- ✅ Enter credentials and click "Login"
- ✅ Successful login redirects to appropriate portal
- ✅ Failed login shows error message

### 2. Student Portal (student.html)
**Features to Test:**
- ✅ Student profile loads (name, email, CGPA)
- ✅ Enroll in a course:
  - Course Code: `CS202`
  - Semester: `3`
- ✅ Success message appears after enrollment

### 3. Teacher Portal (teacher.html)
**Features to Test:**
- ✅ Teacher profile loads
- ✅ Click "Generate Timetable" button
- ✅ Timetable table displays with course schedules
- ✅ Each exam shows Course Code, Time Slot, and Room

### 4. Admin Dashboard (admin.html)
**Features to Test:**
- ✅ Add a new course:
  - Course Code: `CS401`
  - Course Name: `Machine Learning`
  - Semester: `7`
  - Teacher: `prof_khan`
  - Credit Hours: `3`
  - Max Students: `40`
- ✅ Success message appears
- ✅ Click "Generate Timetable"
- ✅ Timetable displays all courses

## Troubleshooting

### Issue: "Failed to fetch" / CORS Error
**Solution:** Make sure Docker container is running:
```bash
docker ps
# Should show uni-server running on port 8080
docker logs uni-server
# Should show "Server starting at http://localhost:8080"
```

### Issue: Timetable shows "No schedule generated"
**Solution:** Make sure courses and enrollments exist:
1. Login as student and enroll in courses
2. Then generate timetable

### Issue: Login redirects to wrong portal
**Solution:** Clear browser localStorage:
```javascript
// Open browser console (F12) and run:
localStorage.clear();
location.reload();
```

## Expected Results

### Successful Flow:
1. **Login** → Redirects based on role
2. **Student Portal** → Shows profile + can enroll
3. **Teacher Portal** → Shows profile + can view timetable
4. **Admin Dashboard** → Can add courses + generate timetable

### API Calls Made:
- `POST /login` - Authentication
- `GET /student/{rollNo}` - Get student data
- `GET /teacher/{username}` - Get teacher data
- `POST /course` - Add new course
- `POST /enroll` - Enroll student
- `POST /timetable` - Generate timetable

## Browser Console Checks

Open DevTools (F12) → Console tab

**Success Indicators:**
```
No errors in console
API calls return 200 OK
JSON responses visible in Network tab
```

**Error Indicators:**
```
CORS errors → Check server is running
404 errors → Check API endpoints
401 errors → Check login credentials
```

## Testing Checklist

- [ ] Docker container running
- [ ] Frontend served (http://localhost:3000)
- [ ] Login page loads
- [ ] Student login works
- [ ] Teacher login works
- [ ] Student can view profile
- [ ] Student can enroll in course
- [ ] Teacher can view profile
- [ ] Teacher can generate timetable
- [ ] Admin can add courses
- [ ] Admin can generate timetable
- [ ] Logout works from all portals
- [ ] No console errors

## Next Steps

Once all tests pass:
1. ✅ Phase 5 Complete
2. Ready for Phase 6: Integration & Testing
3. Consider deployment to production server
