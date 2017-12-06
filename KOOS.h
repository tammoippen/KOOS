/*
 *  KOOS.h
 *  KOOS
 *
 *  Created by Tammo Ippen on 30.03.11.
 *  Copyright 2011 RWTH Aachen. All rights reserved.
 *
 */
#ifndef _KOOS_H
#define _KOOS_

#include <vector>
#include "utilities.h"
#include "KOOSParameter.h"
#include "sha2.h"

using namespace std;

class KOOS 
{
public:
	KOOS(const uint32_t *key = NULL, const uint32_t keySize = 0);
	~KOOS() { origKey.clear(); extendedKey.clear(); paras.clear(); }
	
	void encryptBlock(uint32_t *plain);
	void decryptBlock(uint32_t *cypher);
	
	void encryptECB(uint32_t *plain, const uint32_t plainSize);
	void decryptECB(uint32_t *cypher, const uint32_t cypherSize);
	
	void encryptCBC(uint32_t *plain, const uint32_t plainSize, const uint32_t *iv);
	void decryptCBC(uint32_t *cypher, const uint32_t cypherSize, const uint32_t *iv);
	
private:
	vector<uint32_t> origKey, extendedKey;
	vector<KOOSParameter> paras;
	uint32_t textSize, rounds;
};

#endif