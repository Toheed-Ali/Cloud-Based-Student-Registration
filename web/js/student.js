// Student Dashboard Logic
let myEnrolledCourses = [];
let availableCourses = [];
let currentSemester = 3;

async function init() {
    const auth = checkAuth();
    if (!auth) return;

    const userInfo = getUserInfo();
    document.getElementById('user-name').textContent = userInfo.name;

    loadCourses();
    loadTimetable();
}

async function loadCourses() {
    showLoading(true);

    try {
        // Mock data for demonstration (replace with real API call)
        // const response = await api.getStudentCourses(currentSemester);

        // Mock data
        const mockCourses = [
            {
                courseID: 'CS301',
                courseName: 'Algorithms',
                teacherName: 'teacher5',
                currentEnrollmentCount: 45,
                maxCapacity: 50,
                enrolled: true
            },
            {
                courseID: 'CS302',
                courseName: 'Database Systems',
                teacherName: 'teacher6',
                currentEnrollmentCount: 43,
                maxCapacity: 50,
                enrolled: true
            },
            {
                courseID: 'CS303',
                courseName: 'Computer Organization',
                teacherName: 'teacher7',
                currentEnrollmentCount: 40,
                maxCapacity: 50,
                enrolled: false
            },
            {
                courseID: 'MATH301',
                courseName: 'Linear Algebra',
                teacherName: 'teacher35',
                currentEnrollmentCount: 38,
                maxCapacity: 50,
                enrolled: true
            },
            {
                courseID: 'ENG301',
                courseName: 'Technical Writing',
                teacherName: 'teacher31',
                currentEnrollmentCount: 35,
                maxCapacity: 50,
                enrolled: true
            }
        ];

        myEnrolledCourses = mockCourses.filter(c => c.enrolled);
        availableCourses = mockCourses.filter(c => !c.enrolled);

        renderCourses();
        renderEnrolledCourses();
        updateStats();

    } catch (error) {
        showError('Failed to load courses: ' + error.message);
    } finally {
        showLoading(false);
    }
}

function renderCourses() {
    const tbody = document.getElementById('courses-list');

    if (availableCourses.length === 0) {
        tbody.innerHTML = '<tr><td colspan="5" class="text-center">No available courses</td></tr>';
        return;
    }

    tbody.innerHTML = availableCourses.map(course => `
        <tr>
            <td>${course.courseID}</td>
            <td>${course.courseName}</td>
            <td>${course.teacherName}</td>
            <td>${course.currentEnrollmentCount}/${course.maxCapacity}</td>
            <td>
                <button class="btn btn-success" onclick="enrollCourse('${course.courseID}')">
                    Enroll
                </button>
            </td>
        </tr>
    `).join('');
}

function renderEnrolledCourses() {
    const tbody = document.getElementById('enrolled-list');

    if (myEnrolledCourses.length === 0) {
        tbody.innerHTML = '<tr><td colspan="4" class="text-center">No enrolled courses</td></tr>';
        return;
    }

    tbody.innerHTML = myEnrolledCourses.map(course => `
        <tr>
            <td>${course.courseID}</td>
            <td>${course.courseName}</td>
            <td>${course.teacherName}</td>
            <td>
                <button class="btn btn-danger" onclick="dropCourse('${course.courseID}')">
                    Drop
                </button>
            </td>
        </tr>
    `).join('');
}

function updateStats() {
    document.getElementById('current-semester').textContent = currentSemester;
    document.getElementById('enrolled-count').textContent = myEnrolledCourses.length;
    document.getElementById('available-count').textContent = availableCourses.length;
}

async function enrollCourse(courseId) {
    try {
        showLoading(true);
        // await api.enrollInCourse(courseId);
        showSuccess(`Successfully enrolled in ${courseId}`);

        // Move course from available to enrolled
        const course = availableCourses.find(c => c.courseID === courseId);
        if (course) {
            course.enrolled = true;
            myEnrolledCourses.push(course);
            availableCourses = availableCourses.filter(c => c.courseID !== courseId);
            renderCourses();
            renderEnrolledCourses();
            updateStats();
        }
    } catch (error) {
        showError('Failed to enroll: ' + error.message);
    } finally {
        showLoading(false);
    }
}

async function dropCourse(courseId) {
    if (!confirm('Are you sure you want to drop this course?')) return;

    try {
        showLoading(true);
        // await api.dropCourse(courseId);
        showSuccess(`Successfully dropped ${courseId}`);

        // Move course from enrolled to available
        const course = myEnrolledCourses.find(c => c.courseID === courseId);
        if (course) {
            course.enrolled = false;
            availableCourses.push(course);
            myEnrolledCourses = myEnrolledCourses.filter(c => c.courseID !== courseId);
            renderCourses();
            renderEnrolledCourses();
            updateStats();
        }
    } catch (error) {
        showError('Failed to drop course: ' + error.message);
    } finally {
        showLoading(false);
    }
}

async function loadTimetable() {
    const timetableDiv = document.getElementById('timetable-content');

    // Mock timetable data
    const mockTimetable = `
        <div style="overflow-x: auto;">
            <table>
                <thead>
                    <tr>
                        <th>Time</th>
                        <th>Monday</th>
                        <th>Tuesday</th>
                        <th>Wednesday</th>
                        <th>Thursday</th>
                        <th>Friday</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>9:00-10:30</td>
                        <td>CS301<br><small>Room 101</small></td>
                        <td></td>
                        <td>MATH301<br><small>Room 203</small></td>
                        <td></td>
                        <td>CS302<br><small>Room 105</small></td>
                    </tr>
                    <tr>
                        <td>10:30-12:00</td>
                        <td></td>
                        <td>ENG301<br><small>Room 301</small></td>
                        <td></td>
                        <td>CS301<br><small>Room 101</small></td>
                        <td></td>
                    </tr>
                    <tr>
                        <td>13:00-14:30</td>
                        <td>MATH301<br><small>Room 203</small></td>
                        <td></td>
                        <td>CS302<br><small>Room 105</small></td>
                        <td></td>
                        <td></td>
                    </tr>
                    <tr>
                        <td>14:30-16:00</td>
                        <td></td>
                        <td>CS302<br><small>Room 105</small></td>
                        <td></td>
                        <td>ENG301<br><small>Room 301</small></td>
                        <td>MATH301<br><small>Room 203</small></td>
                    </tr>
                    <tr>
                        <td>16:00-17:30</td>
                        <td>CS301<br><small>Room 101</small></td>
                        <td></td>
                        <td>ENG301<br><small>Room 301</small></td>
                        <td></td>
                        <td></td>
                    </tr>
                </tbody>
            </table>
        </div>
    `;

    timetableDiv.innerHTML = mockTimetable;
}

// Initialize on load
window.addEventListener('DOMContentLoaded', init);
