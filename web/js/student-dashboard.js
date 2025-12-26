// Student Dashboard Logic
let myEnrolledCourses = [];
let availableCourses = [];
let currentSemester = 3;

async function init() {
    const auth = checkAuth();
    if (!auth) return;

    const userInfo = getUserInfo();
    document.getElementById('user-name').textContent = userInfo.name || 'Student';
    document.getElementById('user-avatar').textContent = (userInfo.name || 'S')[0].toUpperCase();

    // Load initial data
    await loadStudentData();

    // Listen for section changes
    document.addEventListener('sectionLoad', (e) => {
        handleSectionLoad(e.detail.section);
    });
}

async function handleSectionLoad(section) {
    switch (section) {
        case 'courses':
            await loadEnrolledCourses();
            break;
        case 'enroll':
            await loadAvailableCourses();
            break;
        case 'timetable':
            await loadTimetable();
            break;
        case 'profile':
            loadProfile();
            break;
    }
}

async function loadStudentData() {
    try {
        // In a real app, fetch student data from API
        // For now, usedefault values
        updateDashboardStats();
    } catch (error) {
        console.error('Error loading student data:', error);
    }
}

function updateDashboardStats() {
    document.getElementById('current-semester').textContent = currentSemester;
    document.getElementById('enrolled-count').textContent = myEnrolledCourses.length;
}

async function loadEnrolledCourses() {
    showLoading(true);

    try {
        // Mock data - Replace with: const data = await api.getStudentCourses(currentSemester);
        const mockCourses = [
            { courseID: 'CS301', courseName: 'Algorithms', teacherName: 'Dr. Smith', credits: 3 },
            { courseID: 'CS302', courseName: 'Database Systems', teacherName: 'Dr. Johnson', credits: 3 },
            { courseID: 'MATH301', courseName: 'Linear Algebra', teacherName: 'Dr. Williams', credits: 3 },
            { courseID: 'ENG301', courseName: 'Technical Writing', teacherName: 'Prof. Brown', credits: 3 }
        ];

        myEnrolledCourses = mockCourses;
        renderEnrolledCourses();
        updateDashboardStats();

    } catch (error) {
        showError('Failed to load enrolled courses: ' + error.message);
    } finally {
        showLoading(false);
    }
}

function renderEnrolledCourses() {
    const tbody = document.getElementById('enrolled-courses-list');

    if (myEnrolledCourses.length === 0) {
        tbody.innerHTML = '<tr><td colspan="5" class="text-center">No enrolled courses</td></tr>';
        return;
    }

    tbody.innerHTML = myEnrolledCourses.map(course => `
        <tr>
            <td><strong>${course.courseID}</strong></td>
            <td>${course.courseName}</td>
            <td>${course.teacherName}</td>
            <td><span class="badge badge-primary">${course.credits} Credits</span></td>
            <td>
                <button class="btn btn-danger btn-sm" onclick="dropCourse('${course.courseID}')">
                    Drop Course
                </button>
            </td>
        </tr>
    `).join('');
}

async function loadAvailableCourses() {
    showLoading(true);

    try {
        // Mock data
        const mockCourses = [
            { courseID: 'CS303', courseName: 'Computer Organization', teacherName: 'Dr. Davis', currentEnrollmentCount: 40, maxCapacity: 50 },
            { courseID: 'CS304', courseName: 'Software Engineering', teacherName: 'Prof. Miller', currentEnrollmentCount: 35, maxCapacity: 50 },
            { courseID: 'MATH302', courseName: 'Probability', teacherName: 'Dr. Wilson', currentEnrollmentCount: 45, maxCapacity: 50 }
        ];

        availableCourses = mockCourses;
        renderAvailableCourses();

    } catch (error) {
        showError('Failed to load available courses: ' + error.message);
    } finally {
        showLoading(false);
    }
}

function renderAvailableCourses() {
    const tbody = document.getElementById('available-courses-list');

    if (availableCourses.length === 0) {
        tbody.innerHTML = '<tr><td colspan="5" class="text-center">No available courses</td></tr>';
        return;
    }

    tbody.innerHTML = availableCourses.map(course => {
        const isFull = course.currentEnrollmentCount >= course.maxCapacity;
        return `
        <tr>
            <td><strong>${course.courseID}</strong></td>
            <td>${course.courseName}</td>
            <td>${course.teacherName}</td>
            <td>
                ${course.currentEnrollmentCount}/${course.maxCapacity}
                ${isFull ? '<span class="badge badge-danger ml-sm">Full</span>' : ''}
            </td>
            <td>
                <button class="btn btn-success btn-sm" 
                        onclick="enrollCourse('${course.courseID}')"
                        ${isFull ? 'disabled' : ''}>
                    ${isFull ? 'Full' : 'Enroll'}
                </button>
            </td>
        </tr>
    `}).join('');
}

async function enrollCourse(courseId) {
    try {
        showLoading(true);
        // await api.enrollInCourse(courseId);

        showSuccess(`Successfully enrolled in ${courseId}!`);

        // Move course from available to enrolled
        const course = availableCourses.find(c => c.courseID === courseId);
        if (course) {
            myEnrolledCourses.push({
                ...course,
                teacherName: course.teacherName,
                credits: 3
            });
            availableCourses = availableCourses.filter(c => c.courseID !== courseId);
            renderAvailableCourses();
            updateDashboardStats();
        }
    } catch (error) {
        showError('Failed to enroll: ' + error.message);
    } finally {
        showLoading(false);
    }
}

async function dropCourse(courseId) {
    if (!confirm(`Are you sure you want to drop ${courseId}?`)) return;

    try {
        showLoading(true);
        // await api.dropCourse(courseId);

        showSuccess(`Successfully dropped ${courseId}`);

        // Move course from enrolled to available
        const course = myEnrolledCourses.find(c => c.courseID === courseId);
        if (course) {
            availableCourses.push({
                courseID: course.courseID,
                courseName: course.courseName,
                teacherName: course.teacherName,
                currentEnrollmentCount: 40,
                maxCapacity: 50
            });
            myEnrolledCourses = myEnrolledCourses.filter(c => c.courseID !== courseId);
            renderEnrolledCourses();
            updateDashboardStats();
        }
    } catch (error) {
        showError('Failed to drop course: ' + error.message);
    } finally {
        showLoading(false);
    }
}

async function loadTimetable() {
    const timetableDiv = document.getElementById('timetable-content');

    // Mock timetable
    const mockTimetable = `
        <div style="overflow-x: auto;">
            <table>
                <thead>
                    <tr>
                        <th style="min-width: 100px;">Time</th>
                        <th>Monday</th>
                        <th>Tuesday</th>
                        <th>Wednesday</th>
                        <th>Thursday</th>
                        <th>Friday</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><strong>9:00-10:30</strong></td>
                        <td><div class="badge badge-primary">CS301<br>Room 101</div></td>
                        <td></td>
                        <td><div class="badge badge-success">MATH301<br>Room 203</div></td>
                        <td></td>
                        <td><div class="badge badge-primary">CS302<br>Room 105</div></td>
                    </tr>
                    <tr>
                        <td><strong>10:30-12:00</strong></td>
                        <td></td>
                        <td><div class="badge badge-warning">ENG301<br>Room 301</div></td>
                        <td></td>
                        <td><div class="badge badge-primary">CS301<br>Room 101</div></td>
                        <td></td>
                    </tr>
                    <tr>
                        <td><strong>13:00-14:30</strong></td>
                        <td><div class="badge badge-success">MATH301<br>Room 203</div></td>
                        <td></td>
                        <td><div class="badge badge-primary">CS302<br>Room 105</div></td>
                        <td></td>
                        <td></td>
                    </tr>
                    <tr>
                        <td><strong>14:30-16:00</strong></td>
                        <td></td>
                        <td><div class="badge badge-primary">CS302<br>Room 105</div></td>
                        <td></td>
                        <td><div class="badge badge-warning">ENG301<br>Room 301</div></td>
                        <td><div class="badge badge-success">MATH301<br>Room 203</div></td>
                    </tr>
                    <tr>
                        <td><strong>16:00-17:30</strong></td>
                        <td><div class="badge badge-primary">CS301<br>Room 101</div></td>
                        <td></td>
                        <td><div class="badge badge-warning">ENG301<br>Room 301</div></td>
                        <td></td>
                        <td></td>
                    </tr>
                </tbody>
            </table>
        </div>
    `;

    timetableDiv.innerHTML = mockTimetable;
}

function loadProfile() {
    const userInfo = getUserInfo();
    document.getElementById('profile-id').value = 'BSCS24119';
    document.getElementById('profile-name').value = userInfo.name || 'Student Name';
    document.getElementById('profile-email').value = 'bscs24119@itu.edu.pk';
    document.getElementById('profile-semester').value = currentSemester;
    document.getElementById('profile-courses').value = myEnrolledCourses.length;
}

// Initialize on load
window.addEventListener('DOMContentLoaded', init);
