#pragma once
#include <fstream>
#include <cstdint>
#include <vector>
#include <string>

using namespace std;

namespace seed{

inline SEED bytes_to_block(const vector<uint8_t>& data, size_t offset);
inline void block_to_bytes(const SEED& b, vector<uint8_t>& data, size_t offset);

inline void Padding(vector<uint8_t>& data);
inline void Unpadding(vector<uint8_t>& data);

inline void Encrypt_Data(const vector<uint8_t>& input, vector<uint8_t>& output, const SEED& Key);
inline void Decrypt_Data(const vector<uint8_t>& input, vector<uint8_t>& output, const SEED& Key);

inline void Encrypt_File(const string& input, const string& output, const string& key_file);
inline void Decrypt_File(const string& input, const string& output, const string& key_file);

}
