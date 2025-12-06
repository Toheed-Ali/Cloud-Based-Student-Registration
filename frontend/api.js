// API Configuration
const API_BASE_URL = 'http://localhost:8080';

// API Client Class
class APIClient {
    constructor(baseURL = API_BASE_URL) {
        this.baseURL = baseURL;
        this.token = localStorage.getItem('authToken');
    }

    // Generic request handler
    async request(endpoint, options = {}) {
        const url = `${this.baseURL}${endpoint}`;
        const headers = {
            'Content-Type': 'application/json',
            ...options.headers
        };

        try {
            const response = await fetch(url, {
                ...options,
                headers
            });

            const data = await response.json();

            if (!response.ok) {
                throw new Error(data.message || 'Request failed');
            }

            return data;
        } catch (error) {
            console.error('API Error:', error);
            throw error;
        }
    }

    // Authentication
    async login(username, password) {
        const data = await this.request('/login', {
            method: 'POST',
            body: JSON.stringify({ username, password })
        });

        if (data.status === 'success') {
            localStorage.setItem('userRole', data.role);
            localStorage.setItem('userId', data.id);
        }

        return data;
    }

    logout() {
        localStorage.removeItem('userRole');
        localStorage.removeItem('userId');
        window.location.href = 'index.html';
    }

    // Student Operations
    async getStudent(rollNo) {
        return await this.request(`/student/${rollNo}`);
    }

    // Teacher Operations
    async getTeacher(username) {
        return await this.request(`/teacher/${username}`);
    }

    // Course Operations
    async addCourse(courseData) {
        return await this.request('/course', {
            method: 'POST',
            body: JSON.stringify(courseData)
        });
    }

    // Enrollment Operations
    async enrollStudent(rollNo, courseCode, semester) {
        return await this.request('/enroll', {
            method: 'POST',
            body: JSON.stringify({ rollNo, courseCode, semester })
        });
    }

    // Timetable Operations
    async generateTimetable() {
        return await this.request('/timetable', {
            method: 'POST'
        });
    }
}

// Helper Functions
function showAlert(message, type = 'success') {
    const alertDiv = document.createElement('div');
    alertDiv.className = `alert alert-${type}`;
    alertDiv.textContent = message;

    const container = document.querySelector('.container');
    if (container) {
        container.insertBefore(alertDiv, container.firstChild);
        setTimeout(() => alertDiv.remove(), 5000);
    }
}

function showLoading(show = true) {
    let spinner = document.getElementById('loading-spinner');

    if (show && !spinner) {
        spinner = document.createElement('div');
        spinner.id = 'loading-spinner';
        spinner.className = 'spinner';
        document.body.appendChild(spinner);
    } else if (!show && spinner) {
        spinner.remove();
    }
}

function checkAuth() {
    const role = localStorage.getItem('userRole');
    const userId = localStorage.getItem('userId');

    if (!role || !userId) {
        window.location.href = 'index.html';
        return null;
    }

    return { role, userId };
}

// Export API client instance
const api = new APIClient();
