# Web GUI Frontend

Modern web-based interface for the University Management System.

## Features

- **Login System** - Role-based authentication
- **Student Portal** - Course enrollment, timetable viewing
- **Teacher Portal** - Course information, teaching schedule
- **Admin Dashboard** - Manage students, teachers, courses, generate timetables

## Tech Stack

- HTML5
- CSS3 (Modern design with CSS variables)
- Vanilla JavaScript (No frameworks)
- Fetch API for backend communication

## Structure

```
web/
├── index.html       # Login page
├── student.html     # Student portal
├── teacher.html     # Teacher portal
├── admin.html       # Admin dashboard
├── css/
│   └── styles.css   # Shared styles
└── js/
    ├── api.js       # Backend API client
    ├── auth.js      # Authentication utilities
    └── student.js   # Student portal logic
```

## Demo Accounts

- **Admin:** admin@university.com / admin123
- **Student:** bscs24119@itu.edu.pk / 4119
- **Teacher:** T1100@itu.edu.pk / 1100

## Running Locally

1. Open `web/index.html` in your browser
2. The frontend currently uses mock data for demonstration
3. To connect to real backend, update `API_URL` in `js/api.js`

## Future Enhancements

- Connect to real backend API
- Add form modals for creating entities
- Real-time updates
- Export timetables to PDF
