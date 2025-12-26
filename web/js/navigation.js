// Navigation Management
class NavigationManager {
    constructor() {
        this.sidebar = document.querySelector('.app-sidebar');
        this.navItems = document.querySelectorAll('.nav-item');
        this.contentSections = document.querySelectorAll('[data-section]');
        this.initializeNavigation();
    }

    initializeNavigation() {
        // Add click handlers to nav items
        this.navItems.forEach(item => {
            item.addEventListener('click', () => {
                const section = item.getAttribute('data-nav');
                this.navigateTo(section);
            });
        });

        // Show first section by default
        const firstSection = this.navItems[0]?.getAttribute('data-nav');
        if (firstSection) {
            this.navigateTo(firstSection);
        }
    }

    navigateTo(section) {
        // Update active nav item
        this.navItems.forEach(item => {
            if (item.getAttribute('data-nav') === section) {
                item.classList.add('active');
            } else {
                item.classList.remove('active');
            }
        });

        // Show corresponding content section
        this.contentSections.forEach(content => {
            if (content.getAttribute('data-section') === section) {
                content.classList.remove('hidden');
                content.classList.add('visible');

                // Trigger section load event
                const event = new CustomEvent('sectionLoad', { detail: { section } });
                document.dispatchEvent(event);
            } else {
                content.classList.add('hidden');
                content.classList.remove('visible');
            }
        });
    }

    toggleSidebar() {
        this.sidebar?.classList.toggle('collapsed');
    }
}

// Mobile menu toggle
function toggleMobileMenu() {
    const sidebar = document.querySelector('.app-sidebar');
    sidebar?.classList.toggle('mobile-open');
}

// Initialize navigation
let navManager;
document.addEventListener('DOMContentLoaded', () => {
    navManager = new NavigationManager();
});
