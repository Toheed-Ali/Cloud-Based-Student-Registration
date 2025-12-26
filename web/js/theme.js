// Theme Management
class ThemeManager {
    constructor() {
        this.theme = this.getStoredTheme() || this.getSystemTheme();
        this.applyTheme(this.theme);
    }

    getStoredTheme() {
        return localStorage.getItem('theme');
    }

    getSystemTheme() {
        return window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
    }

    applyTheme(theme) {
        document.documentElement.setAttribute('data-theme', theme);
        localStorage.setItem('theme', theme);
        this.theme = theme;
        this.updateToggleIcon();
    }

    toggle() {
        const newTheme = this.theme === 'light' ? 'dark' : 'light';
        this.applyTheme(newTheme);
    }

    updateToggleIcon() {
        const toggle = document.getElementById('theme-toggle');
        if (toggle) {
            toggle.innerHTML = this.theme === 'light' ? '🌙' : '☀️';
            toggle.setAttribute('aria-label', `Switch to ${this.theme === 'light' ? 'dark' : 'light'} mode`);
        }
    }
}

// Initialize theme
const themeManager = new ThemeManager();

// Listen for system theme changes
window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', (e) => {
    if (!localStorage.getItem('theme')) {
        themeManager.applyTheme(e.matches ? 'dark' : 'light');
    }
});
