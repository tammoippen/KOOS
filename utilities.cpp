/*
 *  utilities.cpp
 *  KOOS
 *
 *  Created by Tammo Ippen on 30.03.11.
 *  Copyright 2011 RWTH Aachen. All rights reserved.
 *
 */


#include "utilities.h"

int64_t xggt(int64_t a, int64_t b, int64_t &x, int64_t &y)
{
	int64_t last_x, last_y, quotient, t;
	x = 0;
	y = 1;
	last_x = 1;
	last_y = 0;
	while(b != 0)
	{
		quotient = a / b;
		t = b;
		b = a % b;
		a = t;
		
		t = x;
		x = last_x - quotient*x;
		last_x = t;
		
		t = y;
		y = last_y - quotient*y;
		last_y = t;
	}
	x = last_x;
	y = last_y;
	return a;
}

uint32_t getInv(const uint32_t x)
{
	int64_t d, s, t;
	if(x == 0 || x >= PRIM)
		return 0;
	
	d = xggt(x, PRIM, s, t);
	if(s < 0)
		s += PRIM;
	return s;
}


uint8_t permutateS8(const uint32_t p, const uint8_t x)
{
	uint8_t ret;
	
	if(x >= 8) // x not out of range
		return x;
	
	ret = (p >> (28 - x*4)) & 0xF;
	
	if(ret >= 8) // permutation not out of range
		return x;
	
	return ret;
}


uint32_t createPermutation(uint64_t &x, const uint8_t mod)
{
	uint8_t arr[8], check[8], i, j;
	uint32_t ret = 0;
	if(mod > 8)
		return ~ret;
	
	for (i = 0; i < mod; ++i)
		check[i] = i;
	
	
	for (i = 0; i < mod; ++i)
	{
		j = x % (mod-i);
		arr[i] = check[j];
		x /= (mod-i);
		
		for (; j < mod-1-i; ++j)
			check[j] = check[j+1];
	}
	
	for (i = 0; i < mod; ++i)
		ret |= arr[i] << (28-i*4);
	for (; i < 8; ++i)
		ret |= 0xF << (28-i*4);
	
	return ret;
}

uint32_t createInvPermutation(uint32_t p, uint8_t mod)
{
	uint8_t arr[8], x[8], i;
	uint32_t ret = 0;
	
	for (i = 0; i < mod; ++i) // get targets...
		x[i] = (p >> (28 - i*4)) & 0xF;
	
	for (i = 0; i < mod; ++i) // write inverse perm
		arr[x[i]] = i;
	
	for (i = 0; i < mod; ++i)
		ret |= arr[i] << (28-i*4);
	
	for (; i < 8; ++i)
		ret |= 0xF << (28-i*4);
	
	return ret;
}

uint32_t fBox(const uint32_t k, const uint32_t t)
{
	uint64_t k1, k2, p1, p2, tmp1, tmp2;
	uint32_t c1, c2;
	
	k1 = (k >> 16) + 1;
	k2 = (k & 0x0000FFFF) + 1;
	p1 = (t >> 16) + 1;
	p2 = t & 0x0000FFFF;
	
	tmp1 = ((k1 * p1) % PRIM) - 1;
	tmp2 = ((tmp1 + p2) & 0x0000FFFF) + 1;
	c2 = ((k2 * tmp2) % PRIM) - 1;
	c1 = (tmp1 + c2) & 0x0000FFFF;
	
	return (c1 << 16) | c2;
}

void eFunction(const uint32_t in, const uint32_t key1, const uint32_t key2, const uint32_t key3,
			   uint32_t &l, uint32_t &m, uint32_t &r)
{	
	// e-function mars
	m = ADD(in,key1);
	r = LROT(in,13)*key2;
	l = SUB(m,key3); // TODO
	r = LROT(r,5);
	m = LROT(m, r & 0x1F);
	l = XOR(l,r);
	r = LROT(r,5);
	l = XOR(l,r);
	l = LROT(l, r & 0x1F);
}

void feistel(uint32_t *text, const uint32_t srcIndex, const uint32_t val1, 
			 const uint32_t val2, const uint32_t val3,  const uint32_t fFunction)
{
	uint32_t i = 0;
	switch (fFunction) {
		case E_FUNCTION:
			if( i == srcIndex ) ++i;
			text[i] = ADD(text[i],val1);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = ADD(text[i],val2);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = ADD(text[i],val3);
			break;
			
		case F_BOX:
			if( i == srcIndex ) ++i;
			text[i] = XOR(text[i],val1);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = XOR(text[i],val2);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = XOR(text[i],val3);
			break;
			
		case MWC_RAND:
			if( i == srcIndex ) ++i;
			text[i] = SUB(text[i],val1);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = SUB(text[i],val2);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = SUB(text[i],val3);
			break;
		default:
			break;
	}
}

void feistelInv(uint32_t *text, const uint32_t srcIndex, const uint32_t val1, 
			 const uint32_t val2, const uint32_t val3,  const uint32_t fFunction)
{
	uint32_t i = 0;
	switch (fFunction) {
		case E_FUNCTION:
			if( i == srcIndex ) ++i;
			text[i] = SUB(text[i],val1);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = SUB(text[i],val2);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = SUB(text[i],val3);
			break;
			
		case F_BOX:
			if( i == srcIndex ) ++i;
			text[i] = XOR(text[i],val1);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = XOR(text[i],val2);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = XOR(text[i],val3);
			break;
			
		case MWC_RAND:
			if( i == srcIndex ) ++i;
			text[i] = ADD(text[i],val1);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = ADD(text[i],val2);
			++i;
			if( i == srcIndex ) ++i;
			text[i] = ADD(text[i],val3);
			break;
		default:
			break;
	}
}


void blockPerm(uint32_t &op1, uint32_t &op2, const uint32_t perm)
{
	uint8_t orig[8], res[8];
	
	for (size_t i = 0; i < 4; ++i) {
		orig[i] = (op1 >> (8*i)) & 0xFF;
		orig[i+4] = (op2 >> (8*i)) & 0xFF;
	}
	
	for (size_t i = 0; i < 8; ++i)
		res[i] = orig[permutateS8(perm, i)];
	
	op1 = op2 = 0;
	
	for (size_t i = 0; i < 4; ++i) {
		op1 |= res[i] << (8*i);
		op2 |= res[i+4] << (8*i);
	}
}



uint32_t hexToI(const char *h)
{
	uint32_t val = 0;
	char tmp[9];
	strncpy(tmp, h, 8);
	tmp[8] = '\0';
	
	sscanf(tmp, "%x", &val);
	return val;
}











