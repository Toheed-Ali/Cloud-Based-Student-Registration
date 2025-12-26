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
        // Get student ID from localStorage (set during login)
        const studentID = localStorage.getItem('userID') || 'BSCS24119';

        // Fetch student data to get enrolled courses
        const response = await fetch(`${API_URL}/student/viewCourses?studentID=${studentID}`, {
            headers: {
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            }
        });

        if (!response.ok) {
            throw new Error('Failed to fetch courses');
        }

        const data = await response.json();

        // Parse the courses data
        if (data.success === 'true' && data.courses) {
            const courses = JSON.parse(data.courses);
            // For now, show all courses as enrolled (until we have a proper enrolled list endpoint)
            myEnrolledCourses = courses.map(c => ({
                courseID: c.courseID,
                courseName: c.courseName,
                teacherName: c.teacherID,
                credits: 3
            }));
        }

        renderEnrolledCourses();
        updateDashboardStats();

    } catch (error) {
        console.error('Error:', error);
        showError('Failed to load enrolled courses: ' + error.message);
        // Fallback to mock data on error
        myEnrolledCourses = [
            { courseID: 'CS301', courseName: 'Algorithms', teacherName: 'Dr. Smith', credits: 3 },
            { courseID: 'CS302', courseName: 'Database Systems', teacherName: 'Dr. Johnson', credits: 3 }
        ];
        renderEnrolledCourses();
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
        // Fetch courses for current semester
        const response = await fetch(`${API_URL}/student/viewCourses?semester=${currentSemester}`, {
            headers: {
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            }
        });

        if (!response.ok) {
            throw new Error('Failed to fetch available courses');
        }

        const data = await response.json();

        if (data.success === 'true' && data.courses) {
            const courses = JSON.parse(data.courses);
            availableCourses = courses.map(c => ({
                courseID: c.courseID,
                courseName: c.courseName,
                teacherName: c.teacherID,
                currentEnrollmentCount: c.enrollmentCount || 0,
                maxCapacity: 50
            }));
        }

        renderAvailableCourses();

    } catch (error) {
        console.error('Error:', error);
        showError('Failed to load available courses: ' + error.message);
        // Fallback to mock data
        availableCourses = [
            { courseID: 'CS303', courseName: 'Computer Organization', teacherName: 'Dr. Davis', currentEnrollmentCount: 40, maxCapacity: 50 }
        ];
        renderAvailableCourses();
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
    timetableDiv.innerHTML = '<div class="spinner"></div>';

    try {
        const studentID = localStorage.getItem('userID') || 'BSCS24119';

        const response = await fetch(`${API_URL}/student/viewTimetable?studentID=${studentID}`, {
            headers: {
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            }
        });

        if (!response.ok) {
            throw new Error('Failed to fetch timetable');
        }

        const data = await response.json();

        if (data.success === 'true' && data.timetable) {
            const timetableData = JSON.parse(data.timetable);
            renderTimetable(timetableData);
        } else {
            throw new Error('No timetable data available');
        }

    } catch (error) {
        console.error('Error:', error);
        timetableDiv.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon">📅</div>
                <div class="empty-state-text">Timetable not available</div>
                <p class="text-secondary">The timetable hasn't been generated yet or you don't have any enrolled courses.</p>
            </div>
        `;
    }
}

function renderTimetable(timetableData) {
    const timetableDiv = document.getElementById('timetable-content');

    // Create a 2D grid for the timetable
    const schedule = Array(5).fill(null).map(() => Array(5).fill(null).map(() => []));

    // Fill the schedule grid
    timetableData.forEach(course => {
        course.slots.forEach(slot => {
            const dayIndex = parseInt(slot.day);
            const hourIndex = Math.floor((parseInt(slot.hour) - 9) / 1.5);
            if (dayIndex >= 0 && dayIndex < 5 && hourIndex >= 0 && hourIndex < 5) {
                schedule[hourIndex][dayIndex].push({
                    courseID: course.courseID,
                    courseName: course.courseName,
                    classroom: course.classroom
                });
            }
        });
    });

    const days = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday'];
    const times = ['9:00-10:30', '10:30-12:00', '13:00-14:30', '14:30-16:00', '16:00-17:30'];

    let html = `
        <div style="overflow-x: auto;">
            <table>
                <thead>
                    <tr>
                        <th style="min-width: 100px;">Time</th>
                        ${days.map(day => `<th>${day}</th>`).join('')}
                    </tr>
                </thead>
                <tbody>
    `;

    for (let hour = 0; hour < 5; hour++) {
        html += `<tr><td><strong>${times[hour]}</strong></td>`;
        for (let day = 0; day < 5; day++) {
            const classes = schedule[hour][day];
            if (classes.length > 0) {
                html += '<td>';
                classes.forEach(cls => {
                    html += `<div class="badge badge-primary mb-xs">${cls.courseID}<br>Room ${cls.classroom || 'TBA'}</div>`;
                });
                html += '</td>';
            } else {
                html += '<td></td>';
            }
        }
        html += '</tr>';
    }

    html += '</tbody></table></div>';
    timetableDiv.innerHTML = html;
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
