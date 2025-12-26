// Admin Dashboard Logic
let allStudents = [];
let allTeachers = [];
let allCourses = [];

async function init() {
    const auth = checkAuth();
    if (!auth) return;

    // Load dashboard stats
    await loadDashboardStats();

    // Listen for section changes
    document.addEventListener('sectionLoad', (e) => {
        handleSectionLoad(e.detail.section);
    });
}

async function handleSectionLoad(section) {
    switch (section) {
        case 'students':
            await loadStudents();
            break;
        case 'teachers':
            await loadTeachers();
            break;
        case 'courses':
            await loadCourses();
            break;
    }
}

async function loadDashboardStats() {
    try {
        // Load all data to get counts
        await Promise.all([
            loadStudents(),
            loadTeachers(),
            loadCourses()
        ]);

        document.getElementById('total-students').textContent = allStudents.length;
        document.getElementById('total-teachers').textContent = allTeachers.length;
        document.getElementById('total-courses').textContent = allCourses.length;
    } catch (error) {
        console.error('Error loading stats:', error);
    }
}

async function loadStudents() {
    try {
        const response = await fetch(`${API_URL}/admin/viewAllStudents`, {
            headers: {
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            }
        });

        if (!response.ok) throw new Error('Failed to load students');

        const data = await response.json();

        if (data.success === 'true' && data.students) {
            allStudents = data.students;
            renderStudents();
        }
    } catch (error) {
        console.error('Error loading students:', error);
        showError('Failed to load students');
    }
}

function renderStudents() {
    const tbody = document.getElementById('students-list');

    if (allStudents.length === 0) {
        tbody.innerHTML = '<tr><td colspan="5" class="text-center">No students found</td></tr>';
        return;
    }

    tbody.innerHTML = allStudents.map(s => `
        <tr>
            <td><strong>${s.studentID || s.id}</strong></td>
            <td>${s.name}</td>
            <td>${s.email}</td>
            <td>${s.currentSemester || s.semester || 1}</td>
            <td>
                <button class="btn btn-danger btn-sm" onclick="removeStudent('${s.studentID || s.id}')">
                    Remove
                </button>
            </td>
        </tr>
    `).join('');
}

function showAddStudentForm() {
    document.getElementById('add-student-form').classList.remove('hidden');
}

function hideAddStudentForm() {
    document.getElementById('add-student-form').classList.add('hidden');
}

async function addStudent() {
    const studentID = document.getElementById('new-student-id').value;
    const name = document.getElementById('new-student-name').value;
    const email = document.getElementById('new-student-email').value;
    const semester = document.getElementById('new-student-semester').value;
    let password = document.getElementById('new-student-password').value;

    if (!studentID || !name || !email) {
        showError('Please fill in all required fields');
        return;
    }

    // Auto-generate password from last 4 digits if empty
    if (!password) {
        password = studentID.slice(-4);
    }

    try {
        showLoading(true);

        const response = await fetch(`${API_URL}/admin/addStudent`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({
                studentID,
                name,
                email,
                password,
                semester: parseInt(semester)
            })
        });

        const data = await response.json();

        if (data.success === 'true') {
            showSuccess('Student added successfully!');
            hideAddStudentForm();
            // Clear form
            document.getElementById('new-student-id').value = '';
            document.getElementById('new-student-name').value = '';
            document.getElementById('new-student-email').value = '';
            document.getElementById('new-student-password').value = '';
            document.getElementById('new-student-semester').value = '1';
            await loadStudents();
            await loadDashboardStats();
        } else {
            showError(data.error || 'Failed to add student');
        }
    } catch (error) {
        showError('Error adding student: ' + error.message);
    } finally {
        showLoading(false);
    }
}

async function removeStudent(studentID) {
    if (!confirm(`Remove student ${studentID}?`)) return;

    try {
        showLoading(true);

        const response = await fetch(`${API_URL}/admin/removeStudent`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({ studentID })
        });

        const data = await response.json();

        if (data.success === 'true') {
            showSuccess('Student removed successfully');
            await loadStudents();
            await loadDashboardStats();
        } else {
            showError(data.error || 'Failed to remove student');
        }
    } catch (error) {
        showError('Error removing student: ' + error.message);
    } finally {
        showLoading(false);
    }
}

async function loadTeachers() {
    try {
        const response = await fetch(`${API_URL}/admin/viewAllTeachers`, {
            headers: {
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            }
        });

        if (!response.ok) throw new Error('Failed to load teachers');

        const data = await response.json();

        if (data.success === 'true' && data.teachers) {
            allTeachers = data.teachers;
            renderTeachers();
        }
    } catch (error) {
        console.error('Error loading teachers:', error);
        showError('Failed to load teachers');
    }
}

function renderTeachers() {
    const tbody = document.getElementById('teachers-list');

    if (allTeachers.length === 0) {
        tbody.innerHTML = '<tr><td colspan="5" class="text-center">No teachers found</td></tr>';
        return;
    }

    tbody.innerHTML = allTeachers.map(t => `
        <tr>
            <td><strong>${t.teacherID || t.id}</strong></td>
            <td>${t.name}</td>
            <td>${t.email}</td>
            <td>${t.department || 'N/A'}</td>
            <td>
                <button class="btn btn-danger btn-sm" onclick="removeTeacher('${t.teacherID || t.id}')">
                    Remove
                </button>
            </td>
        </tr>
    `).join('');
}

function showAddTeacherForm() {
    document.getElementById('add-teacher-form').classList.remove('hidden');
}

function hideAddTeacherForm() {
    document.getElementById('add-teacher-form').classList.add('hidden');
}

async function addTeacher() {
    const teacherID = document.getElementById('new-teacher-id').value;
    const name = document.getElementById('new-teacher-name').value;
    const email = document.getElementById('new-teacher-email').value;
    const department = document.getElementById('new-teacher-dept').value;
    let password = document.getElementById('new-teacher-password').value;

    if (!teacherID || !name || !email || !department) {
        showError('Please fill in all required fields');
        return;
    }

    // Auto-generate password from last 4 digits if empty
    if (!password) {
        password = teacherID.slice(-4);
    }

    try {
        showLoading(true);

        const response = await fetch(`${API_URL}/admin/addTeacher`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({
                teacherID,
                name,
                email,
                password,
                department
            })
        });

        const data = await response.json();

        if (data.success === 'true') {
            showSuccess('Teacher added successfully!');
            hideAddTeacherForm();
            // Clear form
            document.getElementById('new-teacher-id').value = '';
            document.getElementById('new-teacher-name').value = '';
            document.getElementById('new-teacher-email').value = '';
            document.getElementById('new-teacher-password').value = '';
            document.getElementById('new-teacher-dept').value = '';
            await loadTeachers();
            await loadDashboardStats();
        } else {
            showError(data.error || 'Failed to add teacher');
        }
    } catch (error) {
        showError('Error adding teacher: ' + error.message);
    } finally {
        showLoading(false);
    }
}

async function removeTeacher(teacherID) {
    if (!confirm(`Remove teacher ${teacherID}?`)) return;

    try {
        showLoading(true);

        const response = await fetch(`${API_URL}/admin/removeTeacher`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({ teacherID })
        });

        const data = await response.json();

        if (data.success === 'true') {
            showSuccess('Teacher removed successfully');
            await loadTeachers();
            await loadDashboardStats();
        } else {
            showError(data.error || 'Failed to remove teacher');
        }
    } catch (error) {
        showError('Error removing teacher: ' + error.message);
    } finally {
        showLoading(false);
    }
}

async function loadCourses() {
    try {
        // Load courses from all semesters
        const promises = [];
        for (let sem = 1; sem <= 8; sem++) {
            promises.push(
                fetch(`${API_URL}/student/viewCourses?semester=${sem}`, {
                    headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
                }).then(r => r.json())
            );
        }

        const results = await Promise.all(promises);
        allCourses = [];

        results.forEach(data => {
            if (data.success === 'true' && data.courses) {
                allCourses = allCourses.concat(data.courses);
            }
        });

        renderCourses();
    } catch (error) {
        console.error('Error loading courses:', error);
        showError('Failed to load courses');
    }
}

function renderCourses() {
    const tbody = document.getElementById('courses-list');

    if (allCourses.length === 0) {
        tbody.innerHTML = '<tr><td colspan="5" class="text-center">No courses found</td></tr>';
        return;
    }

    tbody.innerHTML = allCourses.map(c => `
        <tr>
            <td><strong>${c.courseID}</strong></td>
            <td>${c.courseName}</td>
            <td>${c.semester}</td>
            <td>${c.teacherID || 'Not Assigned'}</td>
            <td><span class="badge badge-primary">${c.enrollmentCount || 0}/50</span></td>
        </tr>
    `).join('');
}

async function generateTimetable() {
    const semester = document.getElementById('timetable-semester').value;
    const statusDiv = document.getElementById('generation-status');

    statusDiv.classList.remove('hidden');
    statusDiv.innerHTML = '⏳ Generating timetable for semester ' + semester + '...';

    try {
        const response = await fetch(`${API_URL}/admin/generateTimetable`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({ semester: parseInt(semester) })
        });

        const data = await response.json();

        if (data.success === 'true') {
            statusDiv.className = 'alert alert-success mt-md';
            statusDiv.innerHTML = '✅ Timetable generated successfully for semester ' + semester + '!';
            showSuccess('Timetable generated successfully!');
        } else {
            statusDiv.className = 'alert alert-error mt-md';
            statusDiv.innerHTML = '❌ Failed: ' + (data.error || 'Unknown error');
            showError(data.error || 'Failed to generate timetable');
        }
    } catch (error) {
        statusDiv.className = 'alert alert-error mt-md';
        statusDiv.innerHTML = '❌ Error: ' + error.message;
        showError('Error generating timetable: ' + error.message);
    }
}

function logout() {
    localStorage.clear();
    window.location.href = 'index.html';
}

// Initialize on load
window.addEventListener('DOMContentLoaded', init);
