#include <fstream>
#include <cstdint>
#include <vector>

#include "helpers.hpp"
using namespace std;

namespace seed{

SEED bytes_to_block(const vector<uint8_t>& data, size_t offset) {
    SEED b{};
    for (int i = 0; i < 8; i++)  b.limb[0] = (b.limb[0] << 8) | data[offset + i];
    for (int i = 8; i < 16; i++) b.limb[1] = (b.limb[1] << 8) | data[offset + i];
    return b;
}

void block_to_bytes(const SEED& b, vector<uint8_t>& data, size_t offset) {
    for (int i = 0; i < 8; i++) data[offset + i]     = (b.limb[0] >> (56 - 8*i)) & 0xFF;
    for (int i = 0; i < 8; i++) data[offset + 8 + i] = (b.limb[1] >> (56 - 8*i)) & 0xFF;
}

void Padding(vector<uint8_t>& data) {
    uint8_t pad = 16 - (data.size() % 16);
    data.insert(data.end(), pad, pad);
}

void Unpadding(vector<uint8_t>& data) {
    if (data.empty()) return;
    uint8_t pad = data.back();
    if (pad < 1 || pad > 16) return;
    data.resize(data.size() - pad);
}

void Encrypt_Data(const vector<uint8_t>& input, vector<uint8_t>& output, const SEED& Key) {
    vector<uint8_t> padded = input;
    Padding(padded);
    output.resize(padded.size());

    for (size_t offset = 0; offset < padded.size(); offset += 16) {
        SEED block       = bytes_to_block(padded, offset);
        SEED encrypted   = block.Encrypt_Block(block, Key);
        block_to_bytes(encrypted, output, offset);
    }
}

void Decrypt_Data(const vector<uint8_t>& input, vector<uint8_t>& output, const SEED& Key) {
    if (input.size() % 16 != 0) {
        cerr << "Length of ciphertext is not a multiple of 128.\n";
        exit(1);
    }

    output.resize(input.size());

    for (size_t offset = 0; offset < input.size(); offset += 16) {
        SEED block       = bytes_to_block(input, offset);
        SEED decrypted   = block.Decrypt_Block(block, Key);
        block_to_bytes(decrypted, output, offset);
    }

    Unpadding(output);
}

void Encrypt_File(const string& input, const string& output, const string& key_file) {
    ifstream in(input, ios::binary);
    if (!in) { cerr << "Cannot open input file\n"; return; }

    ifstream key(key_file, ios::binary);
    vector<uint8_t> Key((istreambuf_iterator<char>(key)), istreambuf_iterator<char>());
    SEED K = bytes_to_block(Key, 0);
    K.Generate_Round_Keys();

    vector<uint8_t> Plaintext((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();
    vector<uint8_t> Ciphertext;
    Encrypt_Data(Plaintext, Ciphertext, K);

    ofstream out(output, ios::binary);
    out.write(reinterpret_cast<char*>(Ciphertext.data()), Ciphertext.size());
}

void Decrypt_File(const string& input, const string& output, const string& key_file) {
    ifstream in(input, ios::binary);
    if (!in) { cerr << "Cannot open input file\n"; return; }

    ifstream key(key_file, ios::binary);
    vector<uint8_t> Key((istreambuf_iterator<char>(key)), istreambuf_iterator<char>());
    SEED K = bytes_to_block(Key, 0);
    K.Generate_Round_Keys();

    vector<uint8_t> Ciphertext((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();
    vector<uint8_t> Plaintext;
    Decrypt_Data(Ciphertext, Plaintext, K);

    ofstream out(output, ios::binary);
    out.write(reinterpret_cast<char*>(Plaintext.data()), Plaintext.size());
}


}

