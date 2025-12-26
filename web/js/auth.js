// Authentication handling
function checkAuth() {
    const token = localStorage.getItem('token');
    const role = localStorage.getItem('userRole');

    if (!token) {
        window.location.href = 'index.html';
        return false;
    }

    return { token, role };
}

function getUserInfo() {
    return {
        name: localStorage.getItem('userName'),
        role: localStorage.getItem('userRole')
    };
}

function showError(message) {
    const errorDiv = document.getElementById('error-message');
    if (errorDiv) {
        errorDiv.textContent = message;
        errorDiv.className = 'alert alert-error';
        errorDiv.classList.remove('hidden');

        setTimeout(() => {
            errorDiv.classList.add('hidden');
        }, 5000);
    }
}

function showSuccess(message) {
    const successDiv = document.getElementById('success-message');
    if (successDiv) {
        successDiv.textContent = message;
        successDiv.className = 'alert alert-success';
        successDiv.classList.remove('hidden');

        setTimeout(() => {
            successDiv.classList.add('hidden');
        }, 3000);
    }
}

function showLoading(show = true) {
    const spinner = document.getElementById('loading-spinner');
    if (spinner) {
        spinner.classList.toggle('hidden', !show);
    }
}
