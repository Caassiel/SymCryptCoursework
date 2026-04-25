#include <iostream>
#include <fstream>

#include "constants.hpp"
#include "functions.hpp"
#include "helpers.hpp"
#include "file_handling.cpp"
#include "SEED.cpp"

using namespace std;
using namespace seed;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "SEED block cipher — encrypt and decrypt files.\n\n"
             << "Usage:\n"
             << "  SEED test\n"
             << "  SEED encrypt <input> <output> <key>\n"
             << "  SEED decrypt <input> <output> <key>\n";
        return 1;
    }

    string mode = argv[1];

    if (mode == "test") {
        SEED K;
        K.Unit_Test();
    } else if (mode == "encrypt" || mode == "decrypt") {
        if (argc != 5) {
            cerr << "Usage: SEED " << mode << " <input> <output> <key>\n";
            return 1;
        }
        if (mode == "encrypt") {
            Encrypt_File(argv[2], argv[3], argv[4]);
            cout << "Encryption complete.\n";
        } else {
            Decrypt_File(argv[2], argv[3], argv[4]);
            cout << "Decryption complete.\n";
        }
    } else {
        cerr << "Unknown mode: '" << mode << "'\n"
             << "Valid modes: test, encrypt, decrypt\n";
        return 1;
    }

    return 0;
}
