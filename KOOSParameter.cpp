/*
 *  KOOSParameter.cpp
 *  KOOS
 *
 *  Created by Tammo Ippen on 30.03.11.
 *  Copyright 2011 RWTH Aachen. All rights reserved.
 *
 */

#include "KOOSParameter.h"


KOOSParameter::KOOSParameter(uint64_t k)
{
	this->addrPerm = createPermutation(k, TEXT_SIZE);
	
	this->blockPerm = createPermutation(k, 8); // 8 byte per 2*32-bit words
	this->blockAddr1 = k % TEXT_SIZE;
	k /= TEXT_SIZE;
	this->blockAddr2 = k % (TEXT_SIZE-1);
	k /= (TEXT_SIZE-1);
	// must not be the same
	if(this->blockAddr2 >= this->blockAddr1) ++this->blockAddr2;
	
	this->ops = vector<uint32_t>(NUMBER_OPERATIONS+3);
	for (size_t i = 0; i < this->ops.size(); ++i) {
		this->ops[i] = k % NUMBER_OPERATIONS;
		k /= NUMBER_OPERATIONS;
	}
	
	this->fFunction = k % NUMBER_F_FUNCTIONS;
	k /= NUMBER_F_FUNCTIONS;
	
	this->feistelAddr = k % TEXT_SIZE;
	k /= TEXT_SIZE;
	
	this->opAddr1 = k % TEXT_SIZE;
	k /= TEXT_SIZE;
	this->opAddr2 = k % (TEXT_SIZE-1);
	k /= (TEXT_SIZE-1);
	// must not be the same
	if(this->opAddr2 >= this->opAddr1) ++this->opAddr2;
	
	this->opAddr3 = k % (TEXT_SIZE-1);
	k /= (TEXT_SIZE-1);
	// opAddr1 and opAddr3 become modified... must not be the same
	if(this->opAddr3 >= this->opAddr1) ++this->opAddr3;
	this->opAddr4 = k % (TEXT_SIZE-1);
	k /= (TEXT_SIZE-1);
	// must not be the same
	if(this->opAddr4 >= this->opAddr3) ++this->opAddr4;
	
	this->opOrder = createPermutation(k, NUMBER_ORDER);
}

ostream& operator<<(ostream &o, const KOOSParameter p)
{
	static string fs[] = {"E-fct", "f-box", "MWC"};
	static string ops[] = {"ADD", "SUB", "XOR", "LROT", "MULT"};
	o << "opOrder   = 0x" << hex << p.opOrder << dec << endl;
	o << "addrPerm  = 0x" << hex << p.addrPerm << dec << endl;
	o << "blockPerm = 0x" << hex << p.blockPerm << dec << endl;
	o << "  With addresses " << p.blockAddr1 << " and " << p.blockAddr2 << endl;
	o << "Feistel function " << fs[p.fFunction] << " and source " << p.feistelAddr << endl;
	o << "  Last op = " << ops[p.ops[4]] << endl;
	o << "Operations = [";
	for (size_t i = 0; i < TEXT_SIZE; ++i)
		o << (i != 0 ? ", " : "") << ops[p.ops[i]];
	o << "]" << endl;
	o << "Unkeyed mixing 1: " << ops[p.ops[5]] << " with " << p.opAddr1 << " and " << p.opAddr2 << endl;
	o << "Unkeyed mixing 2: " << ops[p.ops[6]] << " with " << p.opAddr3 << " and " << p.opAddr4 << endl;
	return o;
}