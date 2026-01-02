// Student Dashboard Logic - FINAL FIX FOR TIMETABLE DISPLAY
let myEnrolledCourses = [];
let availableCourses = [];
let currentSemester = 1;
let studentData = null;

async function init() {
    const auth = checkAuth();
    if (!auth) return;

    const userInfo = getUserInfo();
    document.getElementById('user-name').textContent = userInfo.name || 'Student';
    document.getElementById('user-avatar').textContent = (userInfo.name || 'S')[0].toUpperCase();

    await loadStudentData();

    document.addEventListener('sectionLoad', (e) => {
        handleSectionLoad(e.detail.section);
    });
}

async function handleSectionLoad(section) {
    switch (section) {
        case 'courses':
            await loadRegistrationWindow();
            await loadCourses();
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
        const studentID = localStorage.getItem('userID');
        console.log('[Student] Loading data for ID:', studentID);

        if (!studentID) {
            showError('Student ID not found. Please login again.');
            return;
        }

        // CRITICAL FIX: Use correct query parameter format
        const response = await fetch(`${API_URL}/student/mydata?studentID=${encodeURIComponent(studentID)}&_t=${Date.now()}`, {
            headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
        });

        console.log('[Student] Response status:', response.status);

        if (!response.ok) {
            const errorText = await response.text();
            console.error('[Student] Error response:', errorText);
            throw new Error(`API returned ${response.status}`);
        }

        const data = await response.json();
        console.log('[Student] Response data:', data);

        if (data.success === 'true' || data.success === true) {
            studentData = {
                studentID: data.studentID,
                name: data.name,
                email: data.email,
                currentSemester: parseInt(data.currentSemester),
                enrolledCourses: []
            };

            currentSemester = parseInt(data.currentSemester);
            console.log('[Student] Current semester:', currentSemester);

            try {
                if (typeof data.enrolledCourses === 'string') {
                    myEnrolledCourses = JSON.parse(data.enrolledCourses);
                } else {
                    myEnrolledCourses = data.enrolledCourses || [];
                }
                studentData.enrolledCourses = myEnrolledCourses;
                console.log('[Student] Enrolled courses:', myEnrolledCourses);
            } catch (e) {
                console.error('[Student] Failed to parse enrolledCourses:', e);
                myEnrolledCourses = [];
            }

            const studentNameEl = document.getElementById('student-name');
            if (studentNameEl) {
                studentNameEl.textContent = studentData.name;
            }

            updateDashboardStats();
        } else {
            throw new Error('API call unsuccessful');
        }
    } catch (error) {
        console.error('[Student] Error:', error);
        showError('Failed to load student data');
    }
}

function updateDashboardStats() {
    document.getElementById('current-semester').textContent = currentSemester;
    document.getElementById('enrolled-count').textContent = myEnrolledCourses.length;
}

async function loadEnrolledCourses() {
    renderEnrolledCourses();
    updateDashboardStats();
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

let isRegistrationOpen = false;

async function loadRegistrationWindow() {
    try {
        const response = await fetch(`${API_URL}/admin/getRegistrationWindow?_t=${Date.now()}`, {
            headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
        });

        if (response.ok) {
            const data = await response.json();
            if (data.success === 'true') {
                const now = Math.floor(Date.now() / 1000);
                const startTime = parseInt(data.startTime);
                const endTime = parseInt(data.endTime);

                // Fix: Registration is open if we're within the time window (when times are set)
                // OR if isOpen is true and no time window is configured
                if (startTime > 0 && endTime > 0) {
                    // Time-based registration window
                    isRegistrationOpen = now >= startTime && now <= endTime;
                } else {
                    // Manual toggle (no time window set)
                    isRegistrationOpen = data.isOpen === 'true';
                }

                const statusEl = document.getElementById('reg-status-text');
                const badgeEl = document.getElementById('registration-timer');
                const infoEl = document.getElementById('reg-window-info');
                const timeEl = document.getElementById('reg-window-time');

                if (statusEl && badgeEl) {
                    badgeEl.classList.remove('hidden');
                    if (isRegistrationOpen) {
                        badgeEl.className = 'badge badge-success';
                        statusEl.textContent = 'Registration Open';
                    } else {
                        badgeEl.className = 'badge badge-danger';
                        statusEl.textContent = 'Registration Closed';
                    }
                }

                if (infoEl && timeEl && startTime && endTime) {
                    infoEl.classList.remove('hidden');
                    const startStr = new Date(startTime * 1000).toLocaleString();
                    const endStr = new Date(endTime * 1000).toLocaleString();
                    timeEl.textContent = `${startStr} - ${endStr}`;
                }
            }
        }
    } catch (e) {
        console.error('Failed to load reg window', e);
    }
}

async function loadCourses() {
    showLoading(true);
    try {
        document.getElementById('courses-semester').textContent = currentSemester;

        const response = await fetch(`${API_URL}/student/viewCourses?semester=${currentSemester}&_t=${Date.now()}`, {
            headers: { 'Authorization': `Bearer ${localStorage.getItem('token')}` }
        });

        if (!response.ok) throw new Error('Failed to fetch courses');

        const data = await response.json();

        if (data.success === 'true' && data.courses) {
            let coursesList = data.courses;
            if (typeof coursesList === 'string') {
                try {
                    coursesList = JSON.parse(coursesList);
                } catch (e) {
                    console.error('Failed to parse courses JSON', e);
                    coursesList = [];
                }
            }

            const enrolledIDs = new Set(myEnrolledCourses.map(c => c.courseID));

            const allCourses = coursesList.map(c => {
                const isEnrolled = enrolledIDs.has(c.courseID);
                return {
                    ...c,
                    isEnrolled: isEnrolled,
                    teacherDisplay: c.teacherName !== 'TBA' ? `${c.teacherName} (${c.teacherID})` : (c.teacherID || 'Not Assigned')
                };
            });

            renderAllCourses(allCourses);
        }
    } catch (error) {
        console.error('Error:', error);
        showError('Failed to load courses');
    } finally {
        showLoading(false);
    }
}

function renderAllCourses(courses) {
    const tbody = document.getElementById('all-courses-list');
    if (!tbody) return;

    if (courses.length === 0) {
        tbody.innerHTML = '<tr><td colspan="6" class="text-center">No courses found for this semester</td></tr>';
        return;
    }

    tbody.innerHTML = courses.map(c => {
        let actionBtn = '';

        if (c.isEnrolled) {
            actionBtn = `<button class="btn btn-danger btn-sm" onclick="dropCourse('${c.courseID}')" ${!isRegistrationOpen ? 'disabled' : ''}>Drop Course</button>`;
        } else {
            const isFull = (c.enrollmentCount || 0) >= 50;
            if (isFull) {
                actionBtn = `<button class="btn btn-secondary btn-sm" disabled>Full</button>`;
            } else {
                actionBtn = `<button class="btn btn-success btn-sm" onclick="enrollCourse('${c.courseID}')" ${!isRegistrationOpen ? 'disabled' : ''}>Enroll</button>`;
            }
        }

        return `
            <tr>
                <td><strong>${c.courseID}</strong></td>
                <td>${c.courseName}</td>
                <td>${c.teacherDisplay}</td>
                <td>${c.credits || 3} Credits</td>
                <td><span class="badge badge-info">${c.enrollmentCount || 0}/50</span></td>
                <td>${actionBtn}</td>
            </tr>
        `;
    }).join('');
}

async function enrollCourse(courseId) {
    try {
        showLoading(true);

        const studentID = localStorage.getItem('userID');

        const response = await fetch(`${API_URL}/student/enrollCourse`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({ studentID, courseID: courseId })
        });

        const data = await response.json();

        if (!response.ok || data.success !== 'true') {
            throw new Error(data.error || 'Enrollment failed');
        }

        showSuccess(`Successfully enrolled in ${courseId}!`);
        await loadStudentData();
        await loadCourses();

    } catch (error) {
        console.error('Enroll error:', error);
        showError('Failed to enroll: ' + error.message);
    } finally {
        showLoading(false);
    }
}

async function dropCourse(courseId) {
    if (!confirm(`Are you sure you want to drop ${courseId}?`)) return;

    try {
        showLoading(true);

        const studentID = localStorage.getItem('userID');

        const response = await fetch(`${API_URL}/student/dropCourse`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            },
            body: JSON.stringify({ studentID, courseID: courseId })
        });

        const data = await response.json();

        if (!response.ok || data.success !== 'true') {
            throw new Error(data.error || 'Drop course failed');
        }

        showSuccess(`Successfully dropped ${courseId}`);
        await loadStudentData();
        await loadCourses();

    } catch (error) {
        console.error('Drop error:', error);
        showError('Failed to drop course: ' + error.message);
    } finally {
        showLoading(false);
    }
}

// ============================================================================
// CRITICAL FIX: TIMETABLE LOADING
// ============================================================================

async function loadTimetable() {
    const timetableDiv = document.getElementById('timetable-content');
    timetableDiv.innerHTML = '<div class="spinner" style="margin: 2rem auto;"></div>';

    try {
        // Use actual student ID
        const studentID = studentData?.studentID || localStorage.getItem('userID');

        if (!studentID) {
            throw new Error('Student ID not available');
        }

        console.log('[Timetable] Fetching for student:', studentID);

        // CRITICAL FIX: Use correct URL format with proper encoding
        const url = `${API_URL}/student/viewTimetable?studentID=${encodeURIComponent(studentID)}&_t=${Date.now()}`;
        console.log('[Timetable] Fetching from:', url);

        const response = await fetch(url, {
            headers: {
                'Authorization': `Bearer ${localStorage.getItem('token')}`
            }
        });

        // HANDLE STATUS CODES SPECIFICALLY FOR UX
        if (!response.ok) {
            const errorText = await response.text();
            console.error('[Timetable] Error response:', errorText);

            // Check specific status codes for UX flow
            if (response.status === 403) throw new Error('Status:RegistrationOpen');
            if (response.status === 400 || response.status === 404) throw new Error('Status:NotGenerated');

            // Default error parsing
            let msg = `HTTP ${response.status}`;
            try { const d = JSON.parse(errorText); if (d.error) msg = d.error; } catch (e) { }
            throw new Error(msg);
        }

        const data = await response.json();

        if (data.success === 'true' && data.timetable) {
            let timetableList = data.timetable;

            // Parse if string
            if (typeof timetableList === 'string') {
                try {
                    timetableList = JSON.parse(timetableList);
                } catch (e) {
                    throw new Error('Failed to parse timetable data');
                }
            }

            if (timetableList.length === 0) {
                timetableDiv.innerHTML = `
                    <div class="empty-state">
                        <div class="empty-state-icon">📅</div>
                        <div class="empty-state-text" style="color: var(--text-primary); margin-bottom: 0.5rem; font-weight: 600;">No Classes Scheduled</div>
                        <p class="text-secondary">You don't have any enrolled courses with scheduled classes yet.</p>
                    </div>
                `;
                return;
            }

            renderTimetable(timetableList);
        } else {
            // If success=false but status was 200 (unlikely but possible)
            if (data.error && data.error.includes('generated')) throw new Error('Status:NotGenerated');
            throw new Error(data.error || 'No timetable data available');
        }

    } catch (error) {
        console.error('[Timetable] Error:', error);

        let title = 'Timetable Not Available';
        let desc = 'The timetable hasn\'t been generated yet or you haven\'t enrolled in any courses.';
        let icon = '📅';
        let style = 'color: var(--text-secondary);';

        if (error.message.includes('Status:RegistrationOpen')) {
            title = 'Registration Period Active';
            desc = 'Timetables are hidden while course registration is open. Please check back after registration closes.';
            icon = '⚠️';
            style = 'color: var(--warning);';
        } else if (error.message.includes('Status:NotGenerated')) {
            title = 'Timetable Pending';
            desc = 'The administration has not yet generated the final timetable for your semester.';
            icon = '⏳';
            style = 'color: var(--primary);';
        }

        timetableDiv.innerHTML = `
            <div class="empty-state">
                <div class="empty-state-icon" style="font-size: 3rem; margin-bottom: 1rem;">${icon}</div>
                <div class="empty-state-text" style="${style} font-weight: 600; font-size: 1.25rem; margin-bottom: 0.5rem;">${title}</div>
                <p class="text-secondary" style="max-width: 400px; margin: 0 auto; line-height: 1.5;">${desc}</p>
                ${error.message.includes('Status:') ? '' : `<p class="text-secondary" style="font-size: 0.8rem; margin-top: 1rem;">Error: ${error.message}</p>`}
            </div>
        `;
    }
}

function renderTimetable(timetableData) {
    const timetableDiv = document.getElementById('timetable-content');

    console.log('[Timetable] Rendering', timetableData.length, 'courses');

    // Create 5x5 grid (5 days × 5 hours)
    const grid = Array(5).fill(null).map(() => Array(5).fill(null));

    // Fill grid with course objects
    timetableData.forEach(course => {
        console.log('[Timetable] Processing course:', course.courseID);

        if (course.slots && Array.isArray(course.slots)) {
            console.log('[Timetable] Course has', course.slots.length, 'slots');

            course.slots.forEach(slot => {
                const day = parseInt(slot.day);
                const hour = parseInt(slot.hour);

                console.log('[Timetable] Slot:', { day, hour, dayName: slot.dayName, time: slot.time });

                if (day >= 0 && day < 5 && hour >= 0 && hour < 5) {
                    grid[day][hour] = {
                        courseID: course.courseID,
                        courseName: course.courseName,
                        teacherName: course.teacherName,
                        classroom: course.classroom
                    };
                    console.log('[Timetable] Placed at grid[', day, '][', hour, ']');
                } else {
                    console.warn('[Timetable] Invalid slot position:', day, hour);
                }
            });
        } else {
            console.warn('[Timetable] Course has no slots:', course);
        }
    });

    console.log('[Timetable] Final grid:', grid);

    const days = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday'];
    const times = ['9:00-10:30', '10:30-12:00', '13:00-14:30', '14:30-16:00', '16:00-17:30'];

    let html = `
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
        // Add lunch break before slot 2
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
            const cell = grid[day][hour];

            if (!cell) {
                html += '<td style="padding: 0.75rem; border: 1px solid var(--timetable-border); text-align: center; color: var(--timetable-empty-text);">—</td>';
            } else {
                html += `
                    <td style="padding: 0.6rem; border: 1px solid var(--timetable-border); vertical-align: top;">
                        <div style="background: linear-gradient(135deg, var(--timetable-card-bg-start) 0%, var(--timetable-card-bg-end) 100%); padding: 0.65rem; border-radius: 6px; border-left: 3px solid var(--timetable-card-border);">
                            <div style="font-weight: 700; font-size: 0.9rem; color: var(--timetable-card-title); margin-bottom: 0.3rem;">${cell.courseName}</div>
                            <div style="font-size: 0.75rem; color: var(--timetable-card-subtext); font-weight: 500;">
                                ${cell.teacherName || 'TBA'} | Room: ${cell.classroom}
                            </div>
                        </div>
                    </td>
                `;
            }
        }

        html += '</tr>';
    }

    html += '</tbody></table></div>';

    // Add PDF Download Button
    const downloadBtnHtml = `
        <div style="display: flex; justify-content: flex-end; margin-bottom: 1rem;">
            <button id="download-pdf-btn" class="btn btn-primary" onclick="downloadTimetablePDF()">
                <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="margin-right:8px">
                    <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
                    <polyline points="7 10 12 15 17 10"></polyline>
                    <line x1="12" y1="15" x2="12" y2="3"></line>
                </svg>
                Download PDF
            </button>
        </div>
    `;

    timetableDiv.innerHTML = downloadBtnHtml + html;

    console.log('[Timetable] Rendering complete');
}

function downloadTimetablePDF() {
    const tableElement = document.querySelector('#timetable-content table');
    if (!tableElement) return;

    // Use a wrapper to add a title and ensure styling
    const wrapper = document.createElement('div');
    wrapper.innerHTML = `
        <h1 style="text-align: center; font-family: sans-serif; margin-bottom: 20px; color: #333;">My Timetable</h1>
        <p style="text-align: center; font-family: sans-serif; mb: 20px; color: #666;">Generated on ${new Date().toLocaleDateString()}</p>
    `;

    // Clone the table to avoid messing with live DOM
    const cloneTable = tableElement.cloneNode(true);
    wrapper.appendChild(cloneTable);

    const opt = {
        margin: 0.5,
        filename: 'student_timetable.pdf',
        image: { type: 'jpeg', quality: 0.98 },
        html2canvas: { scale: 2 },
        jsPDF: { unit: 'in', format: 'letter', orientation: 'landscape' }
    };

    // Show loading state on button
    const btn = document.getElementById('download-pdf-btn');
    const originalText = btn.innerHTML;
    btn.innerHTML = 'Generating PDF...';
    btn.disabled = true;

    html2pdf().set(opt).from(wrapper).save().then(() => {
        btn.innerHTML = originalText;
        btn.disabled = false;
    }).catch(err => {
        console.error('PDF generation failed:', err);
        btn.innerHTML = originalText;
        btn.disabled = false;
        showError('Failed to generate PDF');
    });
}

function loadProfile() {
    if (!studentData) {
        showError('Student data not loaded');
        return;
    }

    document.getElementById('profile-id').value = studentData.studentID;
    document.getElementById('profile-name').value = studentData.name;
    document.getElementById('profile-email').value = studentData.email;
    document.getElementById('profile-semester').value = currentSemester;
    document.getElementById('profile-courses').value = myEnrolledCourses.length;

    console.log('[Profile] Loaded with:', studentData);
}

window.addEventListener('DOMContentLoaded', init);