/*
 *  utilities.h
 *  KOOS
 *
 *  Created by Tammo Ippen on 30.03.11.
 *  Copyright 2011 RWTH Aachen. All rights reserved.
 *
 */
#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <inttypes.h>
#include <iostream>

// equates 2^16+1
#define PRIM 0x10001
// 4 x 32-bit words = 128 bit
#define TEXT_SIZE 4
#define NUMBER_OPERATIONS 5
#define NUMBER_F_FUNCTIONS 3
#define NUMBER_ORDER 5

#define ROUNDS 32

#define ADD(p,k) (p+k)
#define SUB(c,k) (c-k)
#define XOR(k,p) (k^p)
#define LROT(in,r) (((in) << (r)) | ((in) >> (32-(r))))
#define RROT(in,r) (((in) >> (r)) | ((in) << (32-(r))))
#define MWC(key,state) ((uint32_t)(((36969 * ((key) & 65535) + ((key) >> 16)) << 16) + (18000 * ((state) & 65535) + ((state) >> 16))))

#define OPERATION(plain, key, op) \
switch (op) { \
	case ADDITION: \
		plain = ADD(plain,key); \
		break; \
	case SUBSTRACTION: \
		plain = SUB(plain,key); \
		break; \
	case XORS: \
		plain = XOR(plain,key); \
		break; \
	case ROTATION: \
		plain = LROT(plain, (key*(2*key+1)) >> 27); \
		break; \
	case MULTIPLICATION: \
		MULT(key,plain); \
		break; \
	default: \
		break; \
}

#define OPERATION_INV(cypher, key, op) \
switch (op) { \
	case ADDITION: \
		cypher = SUB(cypher,key); \
		break; \
	case SUBSTRACTION: \
		cypher = ADD(cypher,key); \
		break; \
	case XORS: \
		cypher = XOR(cypher,key); \
		break; \
	case ROTATION: \
		cypher = RROT(cypher, (key*(2*key+1)) >> 27); \
		break; \
	case MULTIPLICATION: \
		MULT_INV(key,cypher); \
		break; \
	default: \
		break; \
}

#define MULT(key, plain) \
{ \
	uint64_t leftK = key >> 16, rightK = key & 0x0000FFFF; \
	uint64_t leftT = plain >> 16, rightT = plain & 0x0000FFFF; \
	\
	uint32_t leftR = ( (leftK+1) * (leftT+1) ) % PRIM; \
	uint32_t rightR = ( (rightK+1) * (rightT+1) ) % PRIM; \
	--leftR; \
	--rightR; \
	\
	plain = (leftR << 16) | (rightR); \
}

#define MULT_INV(key, cypher) \
{ \
	uint64_t leftK, rightK, leftT, rightT; \
	uint32_t leftR, rightR; \
	\
	leftK = getInv((key >> 16) + 1); \
	rightK = getInv((key & 0x0000FFFF) + 1); \
	\
	leftT = cypher >> 16; \
	rightT = cypher & 0x0000FFFF; \
	\
   	leftR = ( leftK * (leftT+1) ) % PRIM; \
   	rightR = ( rightK * (rightT+1) ) % PRIM; \
	\
	--leftR; \
	--rightR; \
	\
	cypher = (leftR << 16) | (rightR); \
}

enum feistel {
	E_FUNCTION, F_BOX, MWC_RAND 
};

enum operations {
	ADDITION, SUBSTRACTION, XORS, ROTATION, MULTIPLICATION
};

enum order {
	ADDRESS_PERMUTATION, KEY_WHITENING, FEISTEL_NETWORK, BLOCK_PERMUTATION, KEYLESS_MIXING
};

int64_t xggt(int64_t a, int64_t b, int64_t &x, int64_t &y);
uint32_t getInv(const uint32_t x);

uint8_t permutateS8(const uint32_t p, const uint8_t x);
uint32_t createPermutation(uint64_t &x, const uint8_t mod);
uint32_t createInvPermutation(uint32_t p, uint8_t mod);

uint32_t fBox(const uint32_t k, const uint32_t t);
void eFunction(const uint32_t in, const uint32_t key1, const uint32_t key2, const uint32_t key3,
			   uint32_t &l, uint32_t &m, uint32_t &r);

void feistel(uint32_t *text, const uint32_t srcIndex, const uint32_t val1, 
			 const uint32_t val2, const uint32_t val3,  const uint32_t fFunction);
void feistelInv(uint32_t *text, const uint32_t srcIndex, const uint32_t val1, 
				const uint32_t val2, const uint32_t val3,  const uint32_t fFunction);

void blockPerm(uint32_t &op1, uint32_t &op2, const uint32_t perm);

uint32_t hexToI(const char *h);
#endif