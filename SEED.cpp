#include "constants.hpp"
#include "functions.hpp"
#include <iostream>

namespace seed {

void SEED::Print() const {
        static const char* hex = "0123456789abcdef";
        string s;
        s.reserve(64);
        for (int i = 0; i < 2; i++) {
            uint64_t v = limb[i];
            for (int j = 60; j >= 0; j -= 4) s.push_back(hex[(v >> j) & 15]);
        }
        cout << s.substr(0,16) << " " << s.substr(16,16) << "\n";
}

uint32_t SEED::G_function(uint32_t X){
    	return ss3[(X >> 24) & 0xFF] ^ ss2[(X >> 16) & 0xFF] ^ ss1[(X >> 8)  & 0xFF] ^ ss0[X & 0xFF];
}

uint64_t SEED::rotate_left(uint64_t x, unsigned int k) {
    	return (x << k) | (x >> (64 - k));
}

uint64_t SEED::rotate_right(uint64_t x, unsigned int k) {
    	return (x >> k) | (x << (64 - k));
}

void SEED::Generate_Round_Keys() {
	uint32_t A, B, C, D;
        uint64_t L0 = limb[0], L1 = limb[1];


        for (int i = 1; i <= 16; i++){
            A = L0 >> 32; B = L0 & 0xFFFFFFFF;
            C = L1 >> 32; D = L1 & 0xFFFFFFFF;

            Subkeys_1[i - 1] = G_function((uint32_t)(A + C) - KC_i[i - 1]);
            Subkeys_2[i - 1] = G_function((uint32_t)(B + KC_i[i - 1]) - D);

            if (i % 2 == 1) L0 = rotate_right(L0, 8);
            else L1 = rotate_left(L1, 8);
        }
}


uint64_t seed::SEED::F_function(uint64_t Half, const SEED& k, int i){
        uint32_t C = Half >> 32, D = Half & ((1ULL << 32) - 1), C1, D1, G1, G2, G3;

        C1 = C ^ k.Subkeys_1[i];
        D1 = D ^ k.Subkeys_2[i];
        G1 = G_function(C1 ^ D1);
        G2 = G_function(C1 + G1);
        G3 = G_function(G1 + G2);

        return ((unsigned long long)(G3 + G2) << 32) | G3;
}

SEED SEED::Encrypt_Block(const SEED& Block, const SEED& Key){
        SEED Cipherblock; uint64_t Left[17] = {0}, Right[17] = {0};
        Left[0] = Block.limb[0]; Right[0] = Block.limb[1];

        for (int i = 1; i <= 15; i++){
            Left[i] = Right[i - 1];
            Right[i] = Left[i - 1] ^ F_function(Right[i - 1], Key, i - 1);
        }

        Left[16] = Left[15] ^ F_function(Right[15], Key, 15);  Right[16] = Right[15];
        Cipherblock.limb[0] = Left[16];                        Cipherblock.limb[1] = Right[16];

        return Cipherblock;
    }

SEED SEED::Decrypt_Block(const SEED& Block, const SEED& Key){
        SEED Plaintextblock; uint64_t Left[17] = {0}, Right[17] = {0};
        Left[0] = Block.limb[0]; Right[0] = Block.limb[1];

        for (int i = 1; i <= 15; i++){
            Left[i] = Right[i - 1];
            Right[i] = Left[i - 1] ^ F_function(Right[i - 1], Key, 16 - i);
        }

        Left[16] = Left[15] ^ F_function(Right[15], Key, 0);  Right[16] = Right[15];
        Plaintextblock.limb[0] = Left[16];                     Plaintextblock.limb[1] = Right[16];

        return Plaintextblock;
    }

void seed::SEED::Unit_Test(){
    SEED v, c_real, k;
        for (int t = 0; t < 5; t++) {
            cout << "Test #" << t + 1 << "\n";

            v.limb[0] = Test_Plaintexts[2 * t];
            v.limb[1] = Test_Plaintexts[2 * t + 1];

            k.limb[0] = Test_Keys[2 * t];
            k.limb[1] = Test_Keys[2 * t + 1];

            c_real.limb[0] = Real_Ciphertexts[2 * t];
            c_real.limb[1] = Real_Ciphertexts[2 * t + 1];

            k.Generate_Round_Keys();

            cout << "Subkeys: \n";
            for (int i = 0; i < 16; i++) {
                cout << "K_ " << i + 1 << ",0: " << hex << k.Subkeys_1[i] << "\n";
                cout << "K_ " << i + 1 << ",1: " << hex << k.Subkeys_2[i] << "\n";
            }

            SEED c = v.Encrypt_Block(v, k);
            SEED d = c.Decrypt_Block(c, k);
            cout << "Key        : "; k.Print();
            cout << "Plaintext  : "; v.Print();
            cout << "Ciphertext : "; c.Print();
            cout << "Real c-text: "; c_real.Print();
            cout << "Decr c-text: "; d.Print();
            cout << "-----------------------------\n";
        }
}

}
