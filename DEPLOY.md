# Deployment Instructions

## Prerequisites
- Docker installed on your machine or server.
- A cloud VPS (Virtual Private Server) like AWS EC2, DigitalOcean Droplet, or Google Compute Engine.

## Local Deployment (Testing)

1.  **Build the Docker Image:**
    ```bash
    docker build -t university-system .
    ```

2.  **Run the Container:**
    ```bash
    docker run -p 8080:8080 university-system
    ```

3.  **Test the API:**
    Use `curl` or the provided PowerShell scripts (`test_api.ps1`, `test_timetable.ps1`) to verify the server is running.
    ```bash
    curl http://localhost:8080/student/2021-CS-001
    ```

## Cloud Deployment (e.g., DigitalOcean/AWS)

1.  **Provision a Server:**
    - Launch an Ubuntu instance.
    - Allow traffic on port 8080 (Security Group / Firewall).

2.  **Install Docker on the Server:**
    ```bash
    sudo apt-get update
    sudo apt-get install -y docker.io
    ```

3.  **Deploy the Application:**
    - **Option A: Copy Files and Build on Server**
        - SCP your project files to the server.
        - Run the build and run commands as above.
    
    - **Option B: Push to Docker Hub (Recommended)**
        1.  **Local:**
            ```bash
            docker tag university-system yourusername/university-system
            docker push yourusername/university-system
            ```
        2.  **Server:**
            ```bash
            docker pull yourusername/university-system
            docker run -d -p 8080:8080 --name uni-sys yourusername/university-system
            ```

## Persistence
To ensure data persists across container restarts, mount a volume for the data files:
```bash
docker run -d -p 8080:8080 -v $(pwd)/data:/app/data university-system
```
*Note: You may need to update the application to save data to a specific `/app/data` directory.*
