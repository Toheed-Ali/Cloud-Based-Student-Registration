// Teacher Dashboard Logic
let teacherData = {};
let assignedCourse = {};
let enrolledStudents = [];

async function init() {
    const auth = checkAuth();
    if (!auth);

    const userInfo = getUserInfo();
    document.getElementById('user-name').textContent = userInfo.name || 'Teacher';

    // Load teacher data
    await loadTeacherData();

    // Listen for section changes
    document.addEventListener('sectionLoad', (e) => {
        handleSectionLoad(e.detail.section);
    });
}

async function handleSectionLoad(section) {
    switch (section) {
        case 'course':
            displayCourseInfo();
            break;
        case 'students':
            await loadEnrolledStudents();
            break;
        case 'timetable':
            await loadTimetable();
            break;
    }
}

async function loadTeacherData() {
    try {
        const teacherID = localStorage.getItem('userID') || 'T1100';

        // Get teacher's assigned course
        const response = await fetch(`${API_URL}/teacher/viewCourse?teacherID=${teacherID}`, {
            headers: {
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            }
        });

        if (!response.ok) throw new Error('Failed to load course');

        const data = await response.json();

        if (data.success === 'true' && data.course) {
            assignedCourse = data.course;
            updateDashboard();
        }
    } catch (error) {
        console.error('Error loading teacher data:', error);
        showError('Failed to load teacher data');
    }
}

function updateDashboard() {
    document.getElementById('course-name').textContent = assignedCourse.courseName || 'No Course';
    document.getElementById('enrolled-count').textContent = assignedCourse.enrollmentCount || 0;
}

function displayCourseInfo() {
    document.getElementById('course-id').value = assignedCourse.courseID || '';
    document.getElementById('course-title').value = assignedCourse.courseName || '';
    document.getElementById('course-semester').value = assignedCourse.semester || '';
    document.getElementById('course-enrollment').value = (assignedCourse.enrollmentCount || 0) + '/50';
}

async function loadEnrolledStudents() {
    try {
        const teacherID = localStorage.getItem('userID') || 'T1100';

        const response = await fetch(`${API_URL}/teacher/viewEnrolledStudents?teacherID=${teacherID}`, {
            headers: {
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            }
        });

        if (!response.ok) throw new Error('Failed to load students');

        const data = await response.json();

        if (data.success === 'true' && data.students) {
            enrolledStudents = data.students;
            renderStudents();
        }
    } catch (error) {
        console.error('Error loading students:', error);
        showError('Failed to load students');
    }
}

function renderStudents() {
    const tbody = document.getElementById('students-list');

    if (enrolledStudents.length === 0) {
        tbody.innerHTML = '<tr><td colspan="4" class="text-center">No students enrolled</td></tr>';
        return;
    }

    tbody.innerHTML = enrolledStudents.map(s => `
        <tr>
            <td><strong>${s.studentID || s.id}</strong></td>
            <td>${s.name}</td>
            <td>${s.email}</td>
            <td>${s.semester || s.currentSemester || 'N/A'}</td>
        </tr>
    `).join('');
}

async function loadTimetable() {
    const timetableDiv = document.getElementById('timetable-content');
    timetableDiv.innerHTML = '<div class="spinner"></div>';

    try {
        const teacherID = localStorage.getItem('userID') || 'T1100';

        const response = await fetch(`${API_URL}/teacher/viewTimetable?teacherID=${teacherID}`, {
            headers: {
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            }
        });

        if (!response.ok) throw new Error('Failed to fetch timetable');

        const data = await response.json();

        if (data.success === 'true' && data.timetable) {
            renderTimetable(data.timetable);
        } else {
            throw new Error('No timetable available');
        }
    } catch (error) {
        console.error('Error:', error);
        timetableDiv.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon">📅</div>
                <div class="empty-state-text">Timetable not available</div>
                <p class="text-secondary">The timetable hasn't been generated yet.</p>
            </div>
        `;
    }
}

function renderTimetable(timetableData) {
    const timetableDiv = document.getElementById('timetable-content');

    // Create a schedule grid
    const schedule = Array(5).fill(null).map(() => Array(5).fill(null));

    if (timetableData.slots) {
        timetableData.slots.forEach(slot => {
            const dayIndex = parseInt(slot.day);
            const hourIndex = Math.floor((parseInt(slot.hour) - 9) / 1.5);
            if (dayIndex >= 0 && dayIndex < 5 && hourIndex >= 0 && hourIndex < 5) {
                schedule[hourIndex][dayIndex] = {
                    courseName: assignedCourse.courseName,
                    classroom: timetableData.classroom || 'TBA'
                };
            }
        });
    }

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
            const cls = schedule[hour][day];
            if (cls) {
                html += `<td><div class="badge badge-success">${cls.courseName}<br>Room ${cls.classroom}</div></td>`;
            } else {
                html += '<td></td>';
            }
        }
        html += '</tr>';
    }

    html += '</tbody></table></div>';
    timetableDiv.innerHTML = html;
}

function logout() {
    localStorage.clear();
    window.location.href = 'index.html';
}

// Initialize on load
window.addEventListener('DOMContentLoaded', init);
