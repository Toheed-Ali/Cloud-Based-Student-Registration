#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <functional>
#include <map>
#include <iostream>
#include "../database/DatabaseManager.h"
#include "utils/JSONParser.h"

using namespace std;

// Simple HTTP request structure
struct HTTPRequest {
    string method;  // GET, POST
    string path;
    string body;
    map<string, string> headers;
};

// Simple HTTP response structure
struct HTTPResponse {
    int statusCode;
    string body;
    map<string, string> headers;
    
    HTTPResponse() : statusCode(200) {
        headers["Content-Type"] = "application/json";
    }
};

// Request handler function type
typedef function<HTTPResponse(const HTTPRequest&, DatabaseManager&)> RequestHandler;

class HTTPServer {
private:
    int port;
    DatabaseManager& db;
    map<pair<string, string>, RequestHandler> routes;  // (method, path) -> handler
    
public:
    HTTPServer(int serverPort, DatabaseManager& database);
    
    // Register route handlers
    void get(const string& path, RequestHandler handler);
    void post(const string& path, RequestHandler handler);
    
    // Start server (simplified - no actual socket implementation for now)
    void start();
    
    // Process a request (for testing)
    HTTPResponse handleRequest(const HTTPRequest& request);
    
    // Helper: Create JSON success response
    static HTTPResponse jsonSuccess(const map<string, string>& data);
    
    // Helper: Create JSON error response
    static HTTPResponse jsonError(const string& message, int statusCode = 400);
};

// ==================== Implementation ====================

inline HTTPServer::HTTPServer(int serverPort, DatabaseManager& database) 
    : port(serverPort), db(database) {}

inline void HTTPServer::get(const string& path, RequestHandler handler) {
    routes[{"GET", path}] = handler;
}

inline void HTTPServer::post(const string& path, RequestHandler handler) {
    routes[{"POST", path}] = handler;
}

inline HTTPResponse HTTPServer::handleRequest(const HTTPRequest& request) {
    auto it = routes.find({request.method, request.path});
    if (it != routes.end()) {
        return it->second(request, db);
    }
    
    return jsonError("Route not found", 404);
}

inline HTTPResponse HTTPServer::jsonSuccess(const map<string, string>& data) {
    HTTPResponse response;
    response.statusCode = 200;
    response.body = JSONParser::stringify(data);
    return response;
}

inline HTTPResponse HTTPServer::jsonError(const string& message, int statusCode) {
    HTTPResponse response;
    response.statusCode = statusCode;
    map<string, string> error;
    error["success"] = "false";
    error["error"] = message;
    response.body = JSONParser::stringify(error);
    return response;
}

inline void HTTPServer::start() {
    cout << "[HTTPServer] Server would start on port " << port << endl;
    cout << "[HTTPServer] " << routes.size() << " routes registered" << endl;
    cout << "[HTTPServer] Note: Full socket implementation requires cpp-httplib" << endl;
    
    // In production, this would use cpp-httplib:
    // httplib::Server svr;
    // for each route: svr.Post/Get(path, handler);
    // svr.listen("0.0.0.0", port);
}

#endif // HTTP_SERVER_H
