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
#include <sstream>
#include <iomanip>

using namespace std;
using namespace httplib;

// Helper function to URL decode a string
inline string urlDecode(const string& str) {
    string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '+') {
            result += ' ';
        } else if (str[i] == '%' && i + 2 < str.length()) {
            int value;
            stringstream ss;
            ss << hex << str.substr(i + 1, 2);
            ss >> value;
            result += static_cast<char>(value);
            i += 2;
        } else {
            result += str[i];
        }
    }
    return result;
}

// Helper function to parse query string
inline map<string, string> parseQueryString(const string& query) {
    map<string, string> params;
    
    if (query.empty()) return params;
    
    stringstream ss(query);
    string pair;
    
    while (getline(ss, pair, '&')) {
        size_t pos = pair.find('=');
        if (pos != string::npos) {
            string key = urlDecode(pair.substr(0, pos));
            string value = urlDecode(pair.substr(pos + 1));
            params[key] = value;
        } else {
            // Handle parameters without values
            string key = urlDecode(pair);
            if (!key.empty()) {
                params[key] = "";
            }
        }
    }
    
    return params;
}

// HTTP Request/Response structures for compatibility
struct HTTPRequest {
    string method;  // GET, POST
    string path;
    string body;
    map<string, string> headers;
    map<string, string> params;  // Query parameters
    
    HTTPRequest(const Request& req) {
        method = req.method;
        path = req.path;
        body = req.body;
        
        // Copy headers
        for (const auto& header : req.headers) {
            headers[header.first] = header.second;
        }
        
        // DEBUGGING: Print all available request info
        cout << "[HTTPRequest] ===================" << endl;
        cout << "[HTTPRequest] Method: " << method << endl;
        cout << "[HTTPRequest] Path: " << path << endl;
        
        // Check if path contains query string (it shouldn't, but let's verify)
        if (path.find('?') != string::npos) {
            cout << "[HTTPRequest] WARNING: Path contains '?' character!" << endl;
        }
        
        // Try to access target
        string target = "";
        try {
            target = req.target;
            cout << "[HTTPRequest] Target: " << target << endl;
        } catch (...) {
            cout << "[HTTPRequest] ERROR: Could not access req.target" << endl;
        }
        
        // CRITICAL FIX: Parse query string from BOTH target AND path
        // Some versions of cpp-httplib put params in path, others in target
        string querySource = target;
        size_t queryPos = querySource.find('?');
        
        if (queryPos == string::npos) {
            // Try path instead
            querySource = path;
            queryPos = querySource.find('?');
            cout << "[HTTPRequest] No '?' in target, trying path" << endl;
        }
        
        if (queryPos != string::npos) {
            string queryString = querySource.substr(queryPos + 1);
            params = parseQueryString(queryString);
            
            cout << "[HTTPRequest] Query string: '" << queryString << "'" << endl;
            cout << "[HTTPRequest] Parsed " << params.size() << " parameters:" << endl;
            for (const auto& p : params) {
                cout << "  [" << p.first << "] = [" << p.second << "]" << endl;
            }
        } else {
            cout << "[HTTPRequest] No query string found in target or path" << endl;
        }
        
        cout << "[HTTPRequest] ===================" << endl;
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
            json += "\"" + pair.first + "\":";
            
            // Check if value is already JSON (array or object) - use it raw
            if (!pair.second.empty() && (pair.second[0] == '[' || pair.second[0] == '{')) {
                json += pair.second;
            } else {
                // String value - quote and escape it
                json += "\"";
                for (char c : pair.second) {
                    if (c == '"') json += "\\\"";
                    else if (c == '\\') json += "\\\\";
                    else json += c;
                }
                json += "\"";
            }
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