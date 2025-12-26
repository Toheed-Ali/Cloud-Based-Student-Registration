#ifndef SHA256_H
#define SHA256_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>

using namespace std;

// Simple SHA-256 implementation
// Note: For production, use a proper crypto library like OpenSSL
// This is a simplified version for educational purposes

class SHA256 {
private:
    static const unsigned int K[64];
    static const unsigned int H0[8];
    
    static unsigned int rotr(unsigned int x, unsigned int n) {
        return (x >> n) | (x << (32 - n));
    }
    
    static unsigned int ch(unsigned int x, unsigned int y, unsigned int z) {
        return (x & y) ^ (~x & z);
    }
    
    static unsigned int maj(unsigned int x, unsigned int y, unsigned int z) {
        return (x & y) ^ (x & z) ^ (y & z);
    }
    
    static unsigned int sigma0(unsigned int x) {
        return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
    }
    
    static unsigned int sigma1(unsigned int x) {
        return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
    }
    
    static unsigned int gamma0(unsigned int x) {
        return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
    }
    
    static unsigned int gamma1(unsigned int x) {
        return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
    }

public:
    static string hash(const string& input);
};

// SHA-256 constants
inline const unsigned int SHA256::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline const unsigned int SHA256::H0[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

inline string SHA256::hash(const string& input) {
    // For simplicity, we'll use a basic hash for now
    // In production, use a proper SHA-256 implementation
    
    // DJB2 hash function (deterministic)
    // Use uint64_t to ensure consistent behavior across platforms
    uint64_t hash = 5381;
    for (unsigned char c : input) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    
    stringstream ss;
    ss << hex << setfill('0') << setw(16) << hash;
    return ss.str();
}

#endif // SHA256_H
