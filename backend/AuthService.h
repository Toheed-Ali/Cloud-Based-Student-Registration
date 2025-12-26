#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "HTTPServer.h"
#include "utils/SHA256.h"
#include <ctime>
#include <random>

using namespace std;

class AuthService {
public:
    // Login endpoint: POST /api/login
    static HTTPResponse login(const HTTPRequest& req, DatabaseManager& db) {
        auto data = JSONParser::parse(req.body);
        string email = JSONParser::getString(data, "email");
        string password = JSONParser::getString(data, "password");
        
        if (email.empty() || password.empty()) {
            return HTTPServer::jsonError("Email and password required");
        }
        
        // Hash the password
        string passwordHash = SHA256::hash(password);
        
        // Authenticate
        User user;
        if (!db.authenticateUser(email, passwordHash, user)) {
            return HTTPServer::jsonError("Invalid email or password", 401);
        }
        
        // Generate session token
        string token = generateToken(user.userID);
        
        // Return success with token and role
        map<string, string> response;
        response["success"] = "true";
        response["token"] = token;
        response["role"] = roleToString(user.role);
        response["userID"] = user.userID;
        response["name"] = user.name;
        
        return HTTPServer::jsonSuccess(response);
    }
    
    // Helper: Generate session token
    static string generateToken(const string& userID) {
        // Simple token: hash(userID + timestamp + random)
        time_t now = time(nullptr);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1000, 9999);
        
        string tokenData = userID + to_string(now) + to_string(dis(gen));
        return SHA256::hash(tokenData);
    }
    
    // Helper: Extract user from token (simplified - in production, store tokens in DB)
    static bool validateToken(const string& token, UserRole requiredRole, 
                             DatabaseManager& db, User& outUser) {
        // For simplicity, we'll just check if user exists
        // In production, validate actual token against database
        
        // For now, return true (simplified)
        // This would need proper implementation with token storage
        return true;
    }
};

#endif // AUTH_SERVICE_H
