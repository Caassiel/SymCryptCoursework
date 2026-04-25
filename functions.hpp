#pragma once
#include <cstdint>

namespace seed{

class SEED{

public:
	uint64_t limb[2];
	uint32_t Subkeys_1[16];
	uint32_t Subkeys_2[16];

	void Print() const;

	static uint32_t G_function(uint32_t X);
	static uint64_t rotate_left(uint64_t x, unsigned int k);
    	static uint64_t rotate_right(uint64_t x, unsigned int k);
	void Generate_Round_Keys();
   	
	static uint64_t F_function(uint64_t half, const SEED& key, int round);

    	SEED Encrypt_Block(const SEED& block, const SEED& key);
	SEED Decrypt_Block(const SEED& block, const SEED& key);
	
   	void Unit_Test();

};

}

