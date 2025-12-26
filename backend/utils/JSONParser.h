#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

// Simple JSON parser for basic request/response handling
// Supports: strings, numbers, booleans, arrays (strings only)
class JSONParser {
public:
    // Parse JSON string to key-value map
    static map<string, string> parse(const string& json);
    
    // Create JSON string from map
    static string stringify(const map<string, string>& data);
    
    // Create JSON array of strings
    static string stringifyArray(const vector<string>& arr);
    
    // Helper: Extract string value
    static string getString(const map<string, string>& data, const string& key, const string& defaultValue = "");
    
    // Helper: Extract int value
    static int getInt(const map<string, string>& data, const string& key, int defaultValue = 0);
    
    // Helper: Extract bool value
    static bool getBool(const map<string, string>& data, const string& key, bool defaultValue = false);

private:
    static string trim(const string& str);
    static string unquote(const string& str);
};

// ==================== Implementation ====================

inline string JSONParser::trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    if (start == string::npos) return "";
    return str.substr(start, end - start + 1);
}

inline string JSONParser::unquote(const string& str) {
    string s = trim(str);
    if (s.length() >= 2 && s[0] == '"' && s[s.length()-1] == '"') {
        return s.substr(1, s.length()-2);
    }
    return s;
}

inline map<string, string> JSONParser::parse(const string& json) {
    map<string, string> result;
    
    // Find opening and closing braces
    size_t start = json.find('{');
    size_t end = json.rfind('}');
    if (start == string::npos || end == string::npos) {
        return result;
    }
    
    string content = json.substr(start + 1, end - start - 1);
    
    // Simple tokenizer - split by comma (not inside quotes)
    vector<string> pairs;
    string current;
    bool inQuotes = false;
    
    for (char c : content) {
        if (c == '"') {
            inQuotes = !inQuotes;
            current += c;
        } else if (c == ',' && !inQuotes) {
            if (!current.empty()) {
                pairs.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        pairs.push_back(current);
    }
    
    // Parse each key-value pair
    for (const string& pair : pairs) {
        size_t colonPos = pair.find(':');
        if (colonPos != string::npos) {
            string key = unquote(pair.substr(0, colonPos));
            string value = trim(pair.substr(colonPos + 1));
            result[key] = value;
        }
    }
    
    return result;
}

inline string JSONParser::stringify(const map<string, string>& data) {
    if (data.empty()) {
        return "{}";
    }
    
    stringstream ss;
    ss << "{";
    
    bool first = true;
    for (const auto& pair : data) {
        if (!first) ss << ",";
        first = false;
        
        ss << "\"" << pair.first << "\":";
        
        // Check if value is already a JSON object/array or a number/boolean
        string value = trim(pair.second);
        if (!value.empty() && (value[0] == '{' || value[0] == '[' || 
            value[0] == '"' || value == "true" || value == "false" ||
            (value[0] >= '0' && value[0] <= '9') || value[0] == '-')) {
            ss << value;
        } else {
            ss << "\"" << value << "\"";
        }
    }
    
    ss << "}";
    return ss.str();
}

inline string JSONParser::stringifyArray(const vector<string>& arr) {
    if (arr.empty()) {
        return "[]";
    }
    
    stringstream ss;
    ss << "[";
    
    for (size_t i = 0; i < arr.size(); i++) {
        if (i > 0) ss << ",";
        ss << "\"" << arr[i] << "\"";
    }
    
    ss << "]";
    return ss.str();
}

inline string JSONParser::getString(const map<string, string>& data, const string& key, const string& defaultValue) {
    auto it = data.find(key);
    if (it != data.end()) {
        return unquote(it->second);
    }
    return defaultValue;
}

inline int JSONParser::getInt(const map<string, string>& data, const string& key, int defaultValue) {
    auto it = data.find(key);
    if (it != data.end()) {
        try {
            return stoi(unquote(it->second));
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

inline bool JSONParser::getBool(const map<string, string>& data, const string& key, bool defaultValue) {
    auto it = data.find(key);
    if (it != data.end()) {
        string value = unquote(it->second);
        return value == "true" || value == "1";
    }
    return defaultValue;
}

#endif // JSON_PARSER_H
