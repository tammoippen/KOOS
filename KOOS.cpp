/*
 *  KOOS.cpp
 *  KOOS
 *
 *  Created by Tammo Ippen on 30.03.11.
 *  Copyright 2011 RWTH Aachen. All rights reserved.
 *
 */

#include "KOOS.h"

KOOS::KOOS(const uint32_t *key, const uint32_t keySize): textSize(TEXT_SIZE), rounds(ROUNDS)
{
	if (key == NULL || keySize == 0)
		return;
	
	this->origKey = vector<uint32_t>(keySize);
	for (size_t i = 0; i < keySize; ++i)
		origKey[i] = key[i];
	
	this->extendedKey = vector<uint32_t>(keySize * this->rounds);
	
	uint8_t buff[4];
	char md[SHA512_DIGEST_STRING_LENGTH];
	SHA512_CTX c512;
	SHA512_Init(&c512);
	
	for (size_t i = 0; i < origKey.size(); ++i) {
		buff[0] = origKey[i] & 0xFF;
		buff[1] = (origKey[i] >> 8) & 0xFF;
		buff[2] = (origKey[i] >> 16) & 0xFF;
		buff[3] = (origKey[i] >> 24) & 0xFF;
		
		SHA512_Update(&c512, buff, 4);
		
		SHA512_End(&c512, md);
		
		for(size_t j = 0; j < 16; ++j)
			extendedKey[i*16+j] = hexToI(md + j*8);
		
	}
	
	this->paras = vector<KOOSParameter>(this->extendedKey.size() / 8);
	for (size_t i = 0; i < this->paras.size(); ++i) {
		uint64_t p = (uint64_t)(this->extendedKey[i*8] ^ 
					 this->extendedKey[i*8 + 1] ^ 
					 this->extendedKey[i*8 + 2] ^ 
					 this->extendedKey[i*8 + 3]) << 32;
		p |= this->extendedKey[i*8 + 4] ^ 
			 this->extendedKey[i*8 + 5] ^ 
			 this->extendedKey[i*8 + 6] ^ 
			 this->extendedKey[i*8 + 7];
		
		this->paras[i] = KOOSParameter(p);
	}
	
}

void KOOS::encryptBlock(uint32_t *plain)
{
	uint32_t *tmp = new uint32_t[this->textSize], val1, val2, val3;
	for (size_t i = 0; i < this->paras.size(); ++i) {
		uint32_t startKeyIndex = i*8;
		KOOSParameter p = this->paras[i];
		
		for (size_t j = 0; j < NUMBER_ORDER; ++j) {
			switch (permutateS8(p.opOrder, j)) {
				case ADDRESS_PERMUTATION: // OK
					for (size_t k = 0; k < this->textSize; ++k)
						tmp[k] = plain[k];
					for (size_t k = 0; k < this->textSize; ++k)
						plain[k] = tmp[permutateS8(p.addrPerm, k)];
					break;
				
				case KEY_WHITENING: // OK
					for (size_t k = 0; k < this->textSize; ++k) {
						OPERATION(plain[k], extendedKey[startKeyIndex+k], p.ops[k]);
					}
					break;

				case FEISTEL_NETWORK: // OK
					switch (p.fFunction) {
						case E_FUNCTION:
							eFunction(plain[p.feistelAddr], extendedKey[startKeyIndex+4], extendedKey[startKeyIndex+5], extendedKey[startKeyIndex+6], val1, val2, val3);
							break;
						case F_BOX:
							val1 = fBox(extendedKey[startKeyIndex+4], plain[p.feistelAddr]);
							val2 = fBox(extendedKey[startKeyIndex+5], plain[p.feistelAddr]);
							val3 = fBox(extendedKey[startKeyIndex+6], plain[p.feistelAddr]);
							break;
						case MWC_RAND:
							val1 = MWC(extendedKey[startKeyIndex+4], plain[p.feistelAddr]);
							val2 = MWC(extendedKey[startKeyIndex+5], plain[p.feistelAddr]);
							val3 = MWC(extendedKey[startKeyIndex+6], plain[p.feistelAddr]);
							break;
						default:
							break;
					}
					feistel(plain, p.feistelAddr, val1, val2, val3, p.fFunction);
					OPERATION(plain[p.feistelAddr], extendedKey[startKeyIndex+7],p.ops[4]);
					break;

				case BLOCK_PERMUTATION: // OK
					blockPerm(plain[p.blockAddr1], plain[p.blockAddr2], p.blockPerm);
					break;

				case KEYLESS_MIXING: // OK
					OPERATION(plain[p.opAddr1], plain[p.opAddr2], p.ops[5]);
					OPERATION(plain[p.opAddr3], plain[p.opAddr4], p.ops[6]);
					break;
				//*/
				default:
					break;
			}
		}
	}
	
	delete [] tmp;
}

void KOOS::decryptBlock(uint32_t *cypher)
{
	uint32_t *tmp = new uint32_t[this->textSize], val1, val2, val3;
	for (int32_t i = this->paras.size()-1; i >= 0 ; --i) { // OK
		uint32_t startKeyIndex = i*8;
		KOOSParameter p = this->paras[i];
		
		for (int32_t j = NUMBER_ORDER-1; j >= 0; --j) { // OK
			switch (permutateS8(p.opOrder, j)) {
				case ADDRESS_PERMUTATION: // OK
					for (size_t k = 0; k < this->textSize; ++k)
						tmp[k] = cypher[k];
					for (size_t k = 0; k < this->textSize; ++k)
						cypher[permutateS8(p.addrPerm, k)] = tmp[k];
					break;
					
				case KEY_WHITENING: // OK
					for (size_t k = 0; k < this->textSize; ++k) {
						OPERATION_INV(cypher[k], extendedKey[startKeyIndex+k], p.ops[k]);
					}
					break;
					
				case FEISTEL_NETWORK: // OK
					OPERATION_INV(cypher[p.feistelAddr], extendedKey[startKeyIndex+7],p.ops[4]);
					
					switch (p.fFunction) {
						case E_FUNCTION:
							eFunction(cypher[p.feistelAddr], extendedKey[startKeyIndex+4], extendedKey[startKeyIndex+5], extendedKey[startKeyIndex+6], val1, val2, val3);
							break;
						case F_BOX:
							val1 = fBox(extendedKey[startKeyIndex+4], cypher[p.feistelAddr]);
							val2 = fBox(extendedKey[startKeyIndex+5], cypher[p.feistelAddr]);
							val3 = fBox(extendedKey[startKeyIndex+6], cypher[p.feistelAddr]);
							break;
						case MWC_RAND:
							val1 = MWC(extendedKey[startKeyIndex+4], cypher[p.feistelAddr]);
							val2 = MWC(extendedKey[startKeyIndex+5], cypher[p.feistelAddr]);
							val3 = MWC(extendedKey[startKeyIndex+6], cypher[p.feistelAddr]);
							break;
						default:
							break;
					}
					feistelInv(cypher, p.feistelAddr, val1, val2, val3, p.fFunction);
					break;
					
				case BLOCK_PERMUTATION: // OK
					blockPerm(cypher[p.blockAddr1], cypher[p.blockAddr2], createInvPermutation(p.blockPerm,8));
					break;
					
				case KEYLESS_MIXING: // OK
					OPERATION_INV(cypher[p.opAddr3], cypher[p.opAddr4], p.ops[6]);
					OPERATION_INV(cypher[p.opAddr1], cypher[p.opAddr2], p.ops[5]);
					break;
				//*/
				default:
					break;
			}
		}
	}
	
	delete [] tmp;
}



void KOOS::encryptECB(uint32_t *plain, const uint32_t plainSize)
{
	if(plainSize % this->textSize != 0) // needs to have right size!
		return;
	
	for(size_t i = 0; i < plainSize / this->textSize; ++i) // encrypt each block on its one
	{
		encryptBlock(&plain[i*this->textSize]);
	}
}


void KOOS::decryptECB(uint32_t *cypher, const uint32_t cypherSize)
{
	if(cypherSize % this->textSize != 0) // needs to have right size!
		return;
	
	for(size_t i = 0; i < cypherSize / this->textSize; ++i) // decrypt each block on its one
	{
		decryptBlock(&cypher[i*this->textSize]);
	}
}


void KOOS::encryptCBC(uint32_t *plain, const uint32_t plainSize, const uint32_t *iv)
{
	if(plainSize % this->textSize != 0) // needs to have right size!
		return;
	
	for(size_t i = 0; i < plainSize / this->textSize; ++i)
	{
		if(i == 0) 	// in first iteration XOR initialization vector on textblock
			for(size_t j = 0; j < this->textSize; ++j)
				plain[i*this->textSize+j] ^= iv[j];
		else		// in all other iterations XOR last textblock on current textblock
			for(size_t j = 0; j < this->textSize; ++j)
				plain[i*this->textSize+j] ^= plain[(i-1)*this->textSize+j];
		
		encryptBlock(&plain[i*this->textSize]);
	}
}


void KOOS::decryptCBC(uint32_t *cypher, const uint32_t cypherSize, const uint32_t *iv)
{
	uint32_t *last_cypher;
	uint8_t part = 0;
	if(cypherSize % this->textSize != 0) // needs to have right size!
		return;
	
	// need to save the last two cypherblocks for xor
	last_cypher = new uint32_t[this->textSize*2];
	
	for(size_t i = 0; i < cypherSize / this->textSize; ++i)
	{
		// write current cypher in first or second part of last_cypher
		for(size_t j = 0; j < this->textSize; ++j)
			last_cypher[j+part*this->textSize] = cypher[i*this->textSize+j];
		
		decryptBlock(&cypher[i*this->textSize]);
		
		if(i == 0)
			for(size_t j = 0; j < this->textSize; ++j)
				cypher[i*this->textSize+j] ^= iv[j];
		else
			for(size_t j = 0; j < this->textSize; ++j) // XOR with the other part of last_cypher
				cypher[i*this->textSize+j] ^= last_cypher[(1-part)*this->textSize+j];
		// make sure part has only 1 or 0 as values (changing every iteration)
		part = (part == 0 ? 1 : 0);
	}
	delete [] last_cypher;
}



