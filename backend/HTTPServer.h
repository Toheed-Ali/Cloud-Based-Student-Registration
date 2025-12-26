#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

// Must define before including httplib to avoid Windows conflicts
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include "../external/httplib.h"
#include "../database/DatabaseManager.h"
#include "utils/JSONParser.h"
#include <functional>
#include <iostream>

using namespace std;
using namespace httplib;

// HTTP Request/Response structures for compatibility
struct HTTPRequest {
    string method;  // GET, POST
    string path;
    string body;
    map<string, string> headers;
    
    HTTPRequest(const Request& req) {
        method = req.method;
        path = req.path;
        body = req.body;
        for (const auto& header : req.headers) {
            headers[header.first] = header.second;
        }
    }
};

struct HTTPResponse {
    int statusCode;
    string body;
    map<string, string> headers;
    
    HTTPResponse(int code = 200, const string& content = "") 
        : statusCode(code), body(content) {}
};

class HTTPServer {
private:
    Server svr;
    DatabaseManager& db;
    int port;
    
    // CORS middleware
    void enableCORS(Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
    
public:
    HTTPServer(int p, DatabaseManager& database) : port(p), db(database) {
        // Handle OPTIONS requests for CORS preflight
        svr.Options(".*", [this](const Request& req, Response& res) {
            enableCORS(res);
            res.set_content("", "text/plain");
        });
    }
    
    // Register GET endpoint
    void get(const string& path, function<HTTPResponse(const HTTPRequest&, DatabaseManager&)> handler) {
        svr.Get(path, [this, handler](const Request& req, Response& res) {
            enableCORS(res);
            try {
                HTTPRequest httpReq(req);
                HTTPResponse httpRes = handler(httpReq, db);
                
                res.status = httpRes.statusCode;
                for (const auto& header : httpRes.headers) {
                    res.set_header(header.first, header.second);
                }
                res.set_content(httpRes.body, "application/json");
            } catch (const exception& e) {
                res.status = 500;
                res.set_content("{\"error\":\"" + string(e.what()) + "\"}", "application/json");
            }
        });
    }
    
    // Register POST endpoint
    void post(const string& path, function<HTTPResponse(const HTTPRequest&, DatabaseManager&)> handler) {
        svr.Post(path, [this, handler](const Request& req, Response& res) {
            enableCORS(res);
            try {
                HTTPRequest httpReq(req);
                HTTPResponse httpRes = handler(httpReq, db);
                
                res.status = httpRes.statusCode;
                for (const auto& header : httpRes.headers) {
                    res.set_header(header.first, header.second);
                }
                res.set_content(httpRes.body, "application/json");
            } catch (const exception& e) {
                res.status = 500;
                res.set_content("{\"error\":\"" + string(e.what()) + "\"}", "application/json");
            }
        });
    }
    
    // Start server (blocking)
    void start() {
        cout << "\n[Server] Starting HTTP server on port " << port << "..." << endl;
        cout << "[Server] Press Ctrl+C to stop" << endl << endl;
        
        if (!svr.listen("0.0.0.0", port)) {
            cerr << "[ERROR] Failed to start server on port " << port << endl;
            throw runtime_error("Failed to start HTTP server");
        }
    }
    
    // Stop server
    void stop() {
        svr.stop();
        cout << "[Server] Server stopped" << endl;
    }
    
    // Helper: Create JSON success response
    static HTTPResponse jsonSuccess(const map<string, string>& data) {
        HTTPResponse response;
        response.statusCode = 200;
        string json = "{";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) json += ",";
            json += "\"" + pair.first + "\":\"" + pair.second + "\"";
            first = false;
        }
        json += "}";
        response.body = json;
        return response;
    }
    
    // Helper: Create JSON error response
    static HTTPResponse jsonError(const string& message, int statusCode = 400) {
        HTTPResponse response;
        response.statusCode = statusCode;
        response.body = "{\"success\":\"false\",\"error\":\"" + message + "\"}";
        return response;
    }
};

#endif // HTTP_SERVER_H
