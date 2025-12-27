// Admin Dashboard Logic - WITH COMPREHENSIVE DIAGNOSTICS
let allStudents = [];
let allTeachers = [];
let allCourses = [];

async function init() {
    const auth = checkAuth();
    if (!auth) return;

    await loadDashboardStats();

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
        case 'timetable':
            await checkExistingTimetables();
            break;
        case 'registration':
            await loadRegistrationWindow();
            break;
    }
}

async function loadDashboardStats() {
    try {
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
            headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
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
                <button class="btn btn-danger btn-sm" onclick="removeStudent('${s.studentID || s.id}')">Remove</button>
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

    if (!password) password = studentID.slice(-4);

    try {
        showLoading(true);
        const response = await fetch(`${API_URL}/admin/addStudent`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({ studentID, name, email, password, semester: parseInt(semester) })
        });

        const data = await response.json();
        if (data.success === 'true') {
            showSuccess('Student added successfully!');
            hideAddStudentForm();
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
            headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
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
                <button class="btn btn-danger btn-sm" onclick="removeTeacher('${t.teacherID || t.id}')">Remove</button>
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

    if (!password) password = teacherID.slice(-4);

    try {
        showLoading(true);
        const response = await fetch(`${API_URL}/admin/addTeacher`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({ teacherID, name, email, password, department })
        });

        const data = await response.json();
        if (data.success === 'true') {
            showSuccess('Teacher added successfully!');
            hideAddTeacherForm();
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

        allCourses.sort((a, b) => {
            if (a.semester !== b.semester) return a.semester - b.semester;
            return a.courseID.localeCompare(b.courseID);
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
            <td><span class="badge badge-primary">${c.currentEnrollmentCount || c.enrollmentCount || 0}/50</span></td>
        </tr>
    `).join('');
}

// ============================================================================
// TIMETABLE MANAGEMENT - WITH COMPREHENSIVE DIAGNOSTICS
// ============================================================================

async function generateTimetable() {
    // Guard: Prevent execution if registration is known to be open
    const instructions = document.getElementById('timetable-instructions');
    const btn = document.querySelector('button[onclick="generateTimetable()"]');

    // Check if button is disabled or warning is showing
    if ((btn && btn.disabled) || (instructions && instructions.innerHTML.includes('Registration is currently OPEN'))) {
        console.warn('[Timetable] Generation blocked: Registration is open.');
        return;
    }

    const statusDiv = document.getElementById('generation-status');
    const container = document.getElementById('all-timetables-container');

    console.log('[Timetable] === GENERATION STARTED ===');

    statusDiv.classList.remove('hidden', 'alert-error', 'alert-success');
    statusDiv.classList.add('alert-info');
    statusDiv.innerHTML = '⏳ Generating timetables for all semesters...';
    container.classList.add('hidden');

    try {
        const response = await fetch(`${API_URL}/admin/generateTimetable`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({})
        });

        const data = await response.json();

        if (data.success === 'true') {
            statusDiv.classList.remove('alert-info');
            statusDiv.classList.add('alert-success');
            statusDiv.innerHTML = '✅ Timetables generated successfully! Loading...';
            showSuccess('Timetables generated successfully!');

            // Wait 3 seconds for backend file I/O
            await new Promise(resolve => setTimeout(resolve, 3000));
            await displayAllTimetables();

            const instructions = document.getElementById('timetable-instructions');
            if (instructions) instructions.classList.add('hidden');
            statusDiv.classList.add('hidden');

        } else {
            // Check for specific backend error about registration
            if (data.error && data.error.includes('registration is open')) {
                // Do NOT show error toast/alert, just refresh state to show warning
                await checkExistingTimetables();
                return;
            }
            throw new Error(data.error || 'Generation failed');
        }
    } catch (error) {
        console.error('[Timetable] Generation error:', error);
        statusDiv.classList.remove('alert-info');
        statusDiv.classList.add('alert-error');
        statusDiv.innerHTML = '❌ Failed: ' + error.message;

        // Only show toast if it's NOT the registration error (extra safety)
        if (!error.message.includes('registration is open')) {
            showError(error.message || 'Failed to generate timetables');
        } else {
            // If it IS registration error, hide the fail message and refresh
            statusDiv.classList.add('hidden');
            await checkExistingTimetables();
        }
    }
}

async function checkExistingTimetables() {
    console.log('[Timetable] === CHECKING EXISTING TIMETABLES ===');

    const container = document.getElementById('all-timetables-container');
    const grid = document.getElementById('timetables-grid');
    const instructions = document.getElementById('timetable-instructions');
    const generateBtn = document.querySelector('button[onclick="generateTimetable()"]');
    const statusDiv = document.getElementById('generation-status');

    // Check Registration Status First
    try {
        const response = await fetch(`${API_URL}/admin/getRegistrationWindow`, {
            headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
        });
        const data = await response.json();

        if (data.success === 'true') {
            const now = Math.floor(Date.now() / 1000);
            const start = parseInt(data.startTime);
            const end = parseInt(data.endTime);
            const isOpen = (start > 0 && end > 0 && now >= start && now <= end);

            if (isOpen) {
                console.log('[Timetable] Registration is OPEN. Blocking generation view.');

                // Show Warning
                if (instructions) {
                    instructions.classList.remove('hidden');
                    instructions.innerHTML = '⚠️<br>Registration is currently OPEN.<br>Timetable generation is disabled until registration closes';
                    instructions.className = 'alert alert-warning mb-md';
                    instructions.style.textAlign = 'center';
                    instructions.style.display = 'block';
                }

                // Hide Error Status if any
                if (statusDiv) {
                    statusDiv.classList.add('hidden');
                    statusDiv.innerHTML = '';
                    statusDiv.className = 'alert hidden mt-md';
                }

                // Disable Button
                if (generateBtn) {
                    generateBtn.disabled = true;
                    generateBtn.style.opacity = '0.5';
                    generateBtn.style.cursor = 'not-allowed';
                    generateBtn.title = 'Registration must be closed first';
                }

                container.classList.add('hidden');
                grid.innerHTML = '';
                return; // STOP here
            } else {
                console.log('[Timetable] Registration is CLOSED. Allowing access.');
                if (generateBtn) {
                    generateBtn.disabled = false;
                    generateBtn.style.opacity = '1';
                    generateBtn.style.cursor = 'pointer';
                    generateBtn.title = '';
                }

                // Restore default instructions
                if (instructions && instructions.innerHTML.includes('Registration is currently OPEN')) {
                    instructions.className = 'text-secondary mb-md';
                    instructions.innerHTML = 'Click the button above to generate timetables for all semesters (1-8). This process schedules all courses while avoiding conflicts.';
                    instructions.classList.remove('hidden');
                    instructions.style.textAlign = 'left';
                }
            }
        }
    } catch (e) {
        console.error('[Timetable] Failed to check registration status:', e);
    }

    container.classList.remove('hidden');
    grid.innerHTML = '<div class="spinner" style="margin: 2rem auto;"></div>';

    await displayAllTimetables();
}

async function displayAllTimetables() {
    console.log('[Timetable] === DISPLAY ALL TIMETABLES ===');

    const container = document.getElementById('all-timetables-container');
    const grid = document.getElementById('timetables-grid');

    container.classList.remove('hidden');
    grid.innerHTML = '<div class="spinner" style="margin: 2rem auto;"></div>';

    let allTimetablesHTML = '';
    let foundCount = 0;
    const diagnostics = [];

    try {
        // Fetch all 8 semesters
        for (let semester = 1; semester <= 8; semester++) {
            try {
                const url = `${API_URL}/admin/viewTimetable?semester=${semester}&_t=${Date.now()}`;
                console.log(`[Timetable] Fetching semester ${semester} from:`, url);

                const response = await fetch(url, {
                    headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
                });

                console.log(`[Timetable] Semester ${semester} response:`, {
                    status: response.status,
                    ok: response.ok,
                    statusText: response.statusText
                });

                const responseText = await response.text();
                console.log(`[Timetable] Semester ${semester} raw response:`, responseText.substring(0, 200));

                if (!response.ok) {
                    diagnostics.push(`Semester ${semester}: HTTP ${response.status} - ${response.statusText}`);
                    console.warn(`[Timetable] Semester ${semester} failed with status ${response.status}`);
                    continue;
                }

                let data;
                try {
                    data = JSON.parse(responseText);
                } catch (e) {
                    console.error(`[Timetable] Failed to parse JSON for semester ${semester}:`, e);
                    diagnostics.push(`Semester ${semester}: JSON parse error - ${e.message}`);
                    continue;
                }

                console.log(`[Timetable] Semester ${semester} parsed data:`, {
                    success: data.success,
                    hasTimetable: !!data.timetable,
                    timetableType: typeof data.timetable
                });

                if (data.success === 'true' && data.timetable) {
                    let timetableData = data.timetable;

                    // Parse if string
                    if (typeof timetableData === 'string') {
                        console.log(`[Timetable] Parsing timetable string for semester ${semester}`);
                        timetableData = JSON.parse(timetableData);
                    }

                    console.log(`[Timetable] Semester ${semester} has ${timetableData.length} courses`);
                    diagnostics.push(`Semester ${semester}: SUCCESS - ${timetableData.length} courses`);

                    if (timetableData.length > 0) {
                        foundCount++;
                        allTimetablesHTML += renderSemesterTimetable(semester, timetableData);
                    } else {
                        diagnostics.push(`Semester ${semester}: Empty timetable`);
                    }
                } else {
                    diagnostics.push(`Semester ${semester}: No timetable data or success=false`);
                }

            } catch (error) {
                console.error(`[Timetable] Error loading semester ${semester}:`, error);
                diagnostics.push(`Semester ${semester}: Exception - ${error.message}`);
            }
        }

        console.log('[Timetable] === LOADING SUMMARY ===');
        console.log(`Found ${foundCount} semesters with timetables`);
        console.log('Diagnostics:', diagnostics);

        if (foundCount > 0) {
            grid.innerHTML = allTimetablesHTML;
            console.log('[Timetable] Successfully rendered timetables');

            // Hide instructions to keep UI clean
            const instructions = document.getElementById('timetable-instructions');
            if (instructions) {
                instructions.classList.add('hidden');
            }

            const statusDiv = document.getElementById('generation-status');
            if (statusDiv) statusDiv.classList.add('hidden');
        } else {
            console.warn('[Timetable] No timetables found');

            // Check if errors are just "Not Found" (404), which is normal for clean state
            const isJustMissing = diagnostics.every(d => d.includes('404') || d.includes('Not Found'));

            if (isJustMissing) {
                grid.innerHTML = `
                    <div class="empty-state">
                        <div class="empty-state-icon">📅</div>
                        <h3 class="empty-state-text" style="color: var(--text-primary); font-weight: 600;">No Timetables Generated Yet</h3>
                        <p class="text-secondary mb-lg">Registration is closed and the system is ready to generate new schedules.</p>
                        <button class="btn btn-primary btn-lg" onclick="generateTimetable()">
                            <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="margin-right:8px">
                                <path d="M23 4v6h-6"></path>
                                <path d="M1 20v-6h6"></path>
                                <path d="M3.51 9a9 9 0 0 1 14.85-3.36L23 10M1 14l4.64 4.36A9 9 0 0 0 20.49 15"></path>
                            </svg>
                            Generate Timetables Now
                        </button>
                    </div>
                `;
            } else {
                // Real errors occurred
                grid.innerHTML = `
                    <div class="card">
                        <div class="card-body">
                            <p class="text-center text-danger"><strong>Error Loading Timetables</strong></p>
                            <p class="text-center text-secondary" style="font-size: 0.875rem;">Diagnostics:</p>
                            <ul style="font-size: 0.875rem; color: #666; max-width: 600px; margin: 1rem auto; text-align: left;">
                                ${diagnostics.map(d => `<li>${d}</li>`).join('')}
                            </ul>
                            <p class="text-center" style="margin-top: 1rem;">
                                <button class="btn btn-primary" onclick="generateTimetable()">Try Generating Anyway</button>
                            </p>
                        </div>
                    </div>
                `;
            }
        }

    } catch (error) {
        console.error('[Timetable] Display error:', error);
        grid.innerHTML = `
            <div class="card">
                <div class="card-body">
                    <p class="text-center text-danger">Failed to load timetables: ${error.message}</p>
                    <p class="text-center text-secondary" style="font-size: 0.875rem;">Check browser console for details</p>
                </div>
            </div>
        `;
    }
}

function renderSemesterTimetable(semester, courses) {
    console.log(`[Timetable] Rendering semester ${semester} with ${courses.length} courses`);

    const grid = Array(5).fill(null).map(() => Array(5).fill(null).map(() => []));

    courses.forEach(course => {
        if (course.slots && Array.isArray(course.slots)) {
            course.slots.forEach(slot => {
                const day = parseInt(slot.day);
                const hour = parseInt(slot.hour);

                if (day >= 0 && day < 5 && hour >= 0 && hour < 5) {
                    grid[day][hour].push({
                        courseID: course.courseID,
                        courseName: course.courseName,
                        teacherName: course.teacherName,
                        classroom: course.classroom,
                        studentCount: course.studentCount || 0
                    });
                }
            });
        }
    });

    const days = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday'];
    const times = ['9:00-10:30', '10:30-12:00', '13:00-14:30', '14:30-16:00', '16:00-17:30'];

    let html = `
        <div class="card mb-lg">
            <div class="card-header">
                <h3 class="card-title">Semester ${semester} Timetable (${courses.length} courses)</h3>
            </div>
            <div class="card-body">
     <div style="overflow-x: auto;">
            <table style="width: 100%; border-collapse: collapse;">
                <thead>
                    <tr style="background: var(--timetable-header-bg);">
                        <th style="padding: 0.75rem; border: 1px solid var(--timetable-border); min-width: 100px; font-weight: 600;">Time</th>
                        ${days.map(day => `<th style="padding: 0.75rem; border: 1px solid var(--timetable-border); font-weight: 600;">${day}</th>`).join('')}
                    </tr>
                </thead>
                <tbody>
    `;

    for (let hour = 0; hour < 5; hour++) {
        // Add lunch break before slot 2 (12:00-13:00)
        if (hour === 2) {
            html += `
                <tr style="background: var(--timetable-lunch-bg);">
                    <td style="padding: 0.75rem; border: 1px solid var(--timetable-border); font-weight: 600;">12:00-13:00</td>
                    ${Array(5).fill('<td style="padding: 0.75rem; border: 1px solid var(--timetable-border); text-align: center; color: var(--timetable-lunch-text); font-weight: 500;">LUNCH BREAK</td>').join('')}
                </tr>
            `;
        }

        html += `<tr><td style="padding: 0.75rem; border: 1px solid var(--timetable-border); font-weight: 500; background: var(--timetable-header-bg);">${times[hour]}</td>`;

        for (let day = 0; day < 5; day++) {
            const classes = grid[day][hour];

            if (classes.length === 0) {
                html += '<td style="padding: 0.75rem; border: 1px solid var(--timetable-border); text-align: center; color: var(--timetable-empty-text);">—</td>';
            } else {
                html += '<td style="padding: 0.6rem; border: 1px solid var(--timetable-border); vertical-align: top;">';

                classes.forEach((cls, idx) => {
                    if (idx > 0) html += '<hr style="margin: 0.5rem 0; border: none; border-top: 1px dashed var(--border);">';
                    html += `
                        <div style="background: linear-gradient(135deg, var(--timetable-card-bg-start) 0%, var(--timetable-card-bg-end) 100%); padding: 0.65rem; border-radius: 6px; border-left: 3px solid var(--timetable-card-border);">
                            <div style="font-weight: 700; font-size: 0.9rem; color: var(--timetable-card-title); margin-bottom: 0.3rem;">${cls.courseName}</div>
                            <div style="font-size: 0.85rem; font-weight: 600; color: var(--timetable-card-text); margin-bottom: 0.4rem; line-height: 1.3;">${cls.courseID}</div>
                            <div style="display: flex; flex-direction: column; gap: 0.2rem; font-size: 0.75rem; color: var(--timetable-card-subtext);">
                                <div><span style="font-weight: 600;">Instructor:</span> ${cls.teacherName}</div>
                                <div><span style="font-weight: 600;">Room:</span> ${cls.classroom} <span style="margin-left: 0.5rem;">|</span> <span style="font-weight: 600; margin-left: 0.5rem;">Enrolled:</span> ${cls.studentCount} students</div>
                            </div>
                        </div>
                    `;
                });

                html += '</td>';
            }
        }
        html += '</tr>';
    }

    html += '</tbody></table></div></div></div>';
    return html;
}

async function loadRegistrationWindow() {
    try {
        const response = await fetch(`${API_URL}/admin/getRegistrationWindow`, {
            headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
        });

        if (!response.ok) throw new Error('Failed to load registration window');
        const data = await response.json();

        if (data.success === 'true') {
            const startTime = parseInt(data.startTime);
            const endTime = parseInt(data.endTime);

            if (startTime > 0) {
                const startParams = new Date(startTime * 1000);
                startParams.setMinutes(startParams.getMinutes() - startParams.getTimezoneOffset());
                document.getElementById('reg-start-time').value = startParams.toISOString().slice(0, 16);
            }

            if (endTime > 0) {
                const endParams = new Date(endTime * 1000);
                endParams.setMinutes(endParams.getMinutes() - endParams.getTimezoneOffset());
                document.getElementById('reg-end-time').value = endParams.toISOString().slice(0, 16);
            }
        }
    } catch (error) {
        console.error('Error loading registration window:', error);
    }
}

async function setRegistrationWindow() {
    const startVal = document.getElementById('reg-start-time').value;
    const endVal = document.getElementById('reg-end-time').value;

    if (!startVal || !endVal) {
        showError('Please select both start and end times');
        return;
    }

    const startDate = new Date(startVal);
    const endDate = new Date(endVal);

    if (endDate <= startDate) {
        showError('End time must be after start time');
        return;
    }

    const startTime = Math.floor(startDate.getTime() / 1000);
    const endTime = Math.floor(endDate.getTime() / 1000);

    try {
        showLoading(true);
        const response = await fetch(`${API_URL}/admin/setRegistrationWindow`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({ startTime, endTime, isOpen: true })
        });

        const data = await response.json();
        if (data.success === 'true') {
            showSuccess('Registration window updated successfully!');
        } else {
            showError(data.error || 'Failed to update registration window');
        }
    } catch (error) {
        showError('Error updating registration window: ' + error.message);
    } finally {
        showLoading(false);
    }
}

function logout() {
    localStorage.clear();
    window.location.href = 'index.html';
}

window.addEventListener('DOMContentLoaded', init);