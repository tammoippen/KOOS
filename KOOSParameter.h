/*
 *  KOOSParameter.h
 *  KOOS
 *
 *  Created by Tammo Ippen on 30.03.11.
 *  Copyright 2011 RWTH Aachen. All rights reserved.
 *
 */

#ifndef _KOOS_PARAMETER_H
#define _KOOS_PARAMETER_H

#include <vector>
#include "utilities.h"

using namespace std;

class KOOS;

class KOOSParameter 
{
public:
	KOOSParameter(uint64_t=0);
	~KOOSParameter(){};
	
	friend class KOOS;
	
	friend ostream& operator<<(ostream &, const KOOSParameter);
	
private:
	//KOOSParameter();
	uint32_t addrPerm, opOrder, blockPerm;
	uint32_t blockAddr1, blockAddr2, fFunction, feistelAddr;
	// 0-3 : key whitening
	// 4 : in feistel
	// 5-6: unkeyed mixing
	vector<uint32_t> ops;
	uint32_t opAddr1, opAddr2, opAddr3, opAddr4;
	
};

#endif