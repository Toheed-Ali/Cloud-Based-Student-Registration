// API Configuration
const API_URL = 'http://18.223.44.164:8080/api';

// API Client
class APIClient {
    constructor(baseURL) {
        this.baseURL = baseURL;
        this.token = localStorage.getItem('token');
    }

    async request(endpoint, options = {}) {
        const url = `${this.baseURL}${endpoint}`;
        const headers = {
            'Content-Type': 'application/json',
            ...options.headers
        };

        if (this.token) {
            headers['Authorization'] = `Bearer ${this.token}`;
        }

        try {
            const response = await fetch(url, {
                ...options,
                headers
            });

            const data = await response.json();

            if (!response.ok) {
                throw new Error(data.error || 'Request failed');
            }

            return data;
        } catch (error) {
            console.error('API Error:', error);
            throw error;
        }
    }

    // Auth endpoints
    async login(email, password) {
        const data = await this.request('/login', {
            method: 'POST',
            body: JSON.stringify({ email, password })
        });
        if (data.token) {
            this.token = data.token;
            localStorage.setItem('token', data.token);
            localStorage.setItem('userRole', data.role);
            localStorage.setItem('userName', data.name);
            localStorage.setItem('userID', data.userID); // Store userID for API calls
        }
        return data;
    }

    logout() {
        this.token = null;
        localStorage.removeItem('token');
        localStorage.removeItem('userRole');
        localStorage.removeItem('userName');
        window.location.href = 'index.html';
    }

    // Student endpoints
    async getStudentCourses(semester) {
        return this.request(`/student/courses?semester=${semester}`);
    }

    async enrollInCourse(courseId) {
        return this.request('/student/enroll', {
            method: 'POST',
            body: JSON.stringify({ courseId })
        });
    }

    async dropCourse(courseId) {
        return this.request('/student/drop', {
            method: 'POST',
            body: JSON.stringify({ courseId })
        });
    }

    async getStudentTimetable() {
        return this.request('/student/timetable');
    }

    // Admin endpoints
    async getAllStudents() {
        return this.request('/admin/students');
    }

    async addStudent(student) {
        return this.request('/admin/students', {
            method: 'POST',
            body: JSON.stringify(student)
        });
    }

    async getAllTeachers() {
        return this.request('/admin/teachers');
    }

    async addTeacher(teacher) {
        return this.request('/admin/teachers', {
            method: 'POST',
            body: JSON.stringify(teacher)
        });
    }

    async getAllCourses() {
        return this.request('/admin/courses');
    }

    async addCourse(course) {
        return this.request('/admin/courses', {
            method: 'POST',
            body: JSON.stringify(course)
        });
    }

    async generateTimetable() {
        return this.request('/admin/generateTimetable', {
            method: 'POST'
        });
    }

    // Teacher endpoints
    async getTeacherCourse() {
        return this.request('/teacher/course');
    }

    async getTeacherTimetable() {
        return this.request('/teacher/timetable');
    }
}

// Initialize API client
const api = new APIClient(API_URL);
