
#include "bbs.h"
#include "utilities.h"
#include "KOOS.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

#ifndef ASCII
#define ASCII 0
#endif

#ifndef TEXT_SIZE
#define TEXT_SIZE 4
#endif

#ifndef KEY_SIZE
#define KEY_SIZE 4
#endif

#ifndef KEY_AVALANCHE_ROUNDS // 24576
#define KEY_AVALANCHE_ROUNDS 24576
#endif

#ifndef PLAIN_CYPHER_CORR_BLOCKS // 8192
#define PLAIN_CYPHER_CORR_BLOCKS 8192
#endif

#ifndef PLAIN_CYPHER_CORR_ROUNDS // 128
#define PLAIN_CYPHER_CORR_ROUNDS 128
#endif

#ifndef CBC_MODE_ROUNDS // 300
#define CBC_MODE_ROUNDS 300
#endif

#ifndef CBC_MODE_BLOCKS // 8192
#define CBC_MODE_BLOCKS 8192
#endif

#ifndef RAND_PLAIN_KEY_BLOCKS // 8192
#define RAND_PLAIN_KEY_BLOCKS 8192
#endif

#ifndef RAND_PLAIN_KEY_ROUNDS // 128
#define RAND_PLAIN_KEY_ROUNDS 128
#endif

#ifndef LOW_HIGH_DENSITY_ROUNDS // 128
#define LOW_HIGH_DENSITY_ROUNDS 128
#endif

typedef void (*fp_data)(ostream & );

// PROTOTYPES
void dataSet(const string dir, const string name, fp_data f, size_t pos);
void status(uint32_t bits, uint32_t seq_len);
// write 0 & 1
template <typename T>
void writeZerosAndOnes(T x, ostream &out);
template <typename T>
void writeZerosAndOnesArray(T *x, size_t size, ostream &out);
// generate 0 & 1 streams
void genRandData(ostream &out);
void genKeyAva(ostream &out);
void genPlainAva(ostream &out);
void genPlainCypherCorrelation(ostream &out);
void genCBCMode(ostream &out);
void genRandPlainKey(ostream &out);
void genLowDensityKey(ostream &out);
void genLowDensityPlain(ostream &out);
void genHighDensityKey(ostream &out);
void genHighDensityPlain(ostream &out);

// write to files
void writeToFiles(const char* dir);

const char FILE_DELIMITER = '/';

int main (int argc, char *argv[])
{
	if(argc != 2) {
		cout << "Missing parameter! Use like this:" << endl;
		cout << argv[0] << " <outdir>" << endl;
		cout << "  With <outdir> being an existing directory, to whom the results are written." << endl;
		cout << "  (e.g. /home/user/Desktop/data)" << endl;
	}
	if(argv[1][strlen(argv[1]) - 1] == FILE_DELIMITER)
		argv[1][strlen(argv[1]) - 1] = '\0';

	writeToFiles(argv[1]);
	return 0;
}

void dataSet(const string dir, const string name, fp_data f, size_t pos)
{
	ofstream outfile;
	time_t start = time(NULL);
	string fileName = dir + FILE_DELIMITER + name + "Row.data";
	cout << "  " << pos <<". " << fileName << ":" << endl;
	outfile.open(fileName.c_str(), ios_base::out | ios_base::trunc);
	if (outfile.is_open()) {
		f(outfile);
		outfile.close();
	} else {
		cout << "ERROR: open " << fileName << " not possible!" << endl;
		exit(1);
	}
	time_t end = time(NULL);
	cout << "  Needed " << (end - start) << " sec." << endl << endl;
}

void writeToFiles(const char* d)
{
	bbsSeed("10d6333cfac8e30e808d2192f7c0439480da79db9bbca1667d73be9a677ed31311f3b830937763837cb7b1b1dc75f14eea417f84d9625628750de99e7ef1e976", // seed
			"E65097BAEC92E70478CAF4ED0ED94E1C94B154466BFB9EC9BE37B2B0FF8526C222B76E0E915017535AE8B9207250257D0A0C87C0DACEF78E17D1EF9DC44FD91F", // p
			"E029AEFCF8EA2C29D99CB53DD5FA9BC1D0176F5DF8D9110FD16EE21F32E37BA86FF42F00531AD5B8A43073182CC2E15F5C86E8DA059E346777C9A985F7D8A867");//q
	srand(time(NULL));              // make some random
	uint randNum = rand() % 1000;   // numbers for 'seeding' bbs
	for(size_t i = 0; i < randNum; ++i)
		bbsRand();
	
	ofstream outfile;
	string dir = d, fileName;
	time_t start, end;
	fp_data fs[] = {genKeyAva, genPlainAva, genPlainCypherCorrelation,
					genCBCMode, genLowDensityKey,
					genLowDensityPlain, genHighDensityKey,
					genHighDensityPlain, genRandPlainKey, genRandData};
	string names[] = {"keyAva", "plainAva", "plainCyphCorr", "cbcMode",
					"lowDensKey", "lowDensPlain",
					"highDensKey", "highDensPlain", "randPlainKey","genRandData"};
	
	
	start = time(NULL);
	for(size_t pos = 0; pos < 8; ++pos) // 0-9
		dataSet(dir, names[pos], fs[pos], pos+1);
	
	end = time(NULL);
	cout << "All over needed time: " << (end-start) << endl;
}


template <typename T>
void writeZerosAndOnes(T x, ostream &out)
{
	if(ASCII) {
		for(size_t i = 0; i < sizeof(T)*8; ++i)
		{
			if((x >> (sizeof(T)*8-1 - i)) & 1) {
				out << "1";
			} else {
				out << "0";
			}
		}
		out << endl;
	} else {
		for(size_t i = 0; i < sizeof(T); ++i)
		{
			out << (uint8_t)((x >> ((sizeof(T) - 1 - i)*8)) & 0xFF);
			//printf("%2x", (int)(x >> ((sizeof(T) - 1 - i)*8)) &0xff);
		}
	}
}

template <typename T>
void writeZerosAndOnesArray(T *x, size_t size, ostream &out)
{
	for(size_t i = 0; i < size; ++i)
	{
		writeZerosAndOnes(x[i], out);
	}
}

void status(uint32_t bits, uint32_t seq_len)
{
	cout << "    Bits: " << bits << endl;
	cout << "    Sequence-length: "<< seq_len <<" => " << bits/seq_len << " sequences." << endl;
	cout << "    Key-Size: " << KEY_SIZE*32 << endl;
	cout << "    Text_Size: " << TEXT_SIZE*32 << endl;
	cout << "    Rounds: " << ROUNDS << endl;
}

void genRandData(ostream &out) 
{
	uint32_t data[32768]; // one sequence of data a 1048576 bit
	bbsSeed("10d6333cfac8e30e808d2192f7c0439480da79db9bbca1667d73be9a677ed31311f3b830937763837cb7b1b1dc75f14eea417f84d9625628750de99e7ef1e976", // seed
			"E65097BAEC92E70478CAF4ED0ED94E1C94B154466BFB9EC9BE37B2B0FF8526C222B76E0E915017535AE8B9207250257D0A0C87C0DACEF78E17D1EF9DC44FD91F", // p
			"E029AEFCF8EA2C29D99CB53DD5FA9BC1D0176F5DF8D9110FD16EE21F32E37BA86FF42F00531AD5B8A43073182CC2E15F5C86E8DA059E346777C9A985F7D8A867");//q
	time_t start, tmp1, tmp2;
	tmp1 = start = time(NULL);
	for (uint32_t i = 0; i < 300; ++i) {
		for (uint32_t j = 0; j < 32768; ++j) {
			data[j] = bbsRand();
		}
		tmp2 = time(NULL);
		cout << setw(3) << i << ". Sequence done! Needed " << tmp2 - tmp1 << " s. Overal: " << tmp2 - start << "s." << endl;
		tmp1 = tmp2;
		writeZerosAndOnesArray(data, 32768, out);
	}
}

void genKeyAva(ostream &out)
{
	uint32_t plainOrig[TEXT_SIZE], plainPert[TEXT_SIZE];
	uint32_t key[KEY_SIZE];
	KOOS k1, k2;
	
	status(KEY_AVALANCHE_ROUNDS*TEXT_SIZE*32*128, 1048576);
	
	for(size_t i = 0; i < KEY_AVALANCHE_ROUNDS; ++i)
	{
		for (size_t j = 0; j < KEY_SIZE; ++j) // new random key
			key[j] = bbsRand();
		
		k1 = KOOS(key,KEY_SIZE);
		
		for(size_t j = 0; j < TEXT_SIZE; ++j) // plaintext of zeros
			plainOrig[j] = 0;
		
		k1.encryptBlock(plainOrig); // create orig crypttext
		
		for(size_t j = 0; j < 128; ++j)
		{
			key[j / 32] ^= (1 << (j % 32)); // flip bit in key at pos i
			k2 = KOOS(key, KEY_SIZE);
			
			for(size_t k = 0; k < TEXT_SIZE; ++k) // plaintext of zeros
				plainPert[k] = 0;
			
			k2.encryptBlock(plainPert); // create flipped crypttext
			
			for(size_t k = 0; k < TEXT_SIZE; ++k) // xor orig and flipped crypttext
				plainPert[k] ^= plainOrig[k];
			
			writeZerosAndOnesArray(plainPert, TEXT_SIZE, out); // write
			
			key[j / 32] ^= (1 << (j % 32)); // flip ith bit back
		}
	}
}

void genPlainAva(ostream &out)
{
	uint32_t plain[TEXT_SIZE], plainOrig[TEXT_SIZE], plainPert[TEXT_SIZE];
	uint32_t key[KEY_SIZE];
	KOOS k1;
	
	status(KEY_AVALANCHE_ROUNDS*TEXT_SIZE*32*128, 1048576);
	
	for (size_t j = 0; j < KEY_SIZE; ++j) // key of zeros
		key[j] = 0;
	
	k1 = KOOS(key, KEY_SIZE);
	
	for(size_t i = 0; i < KEY_AVALANCHE_ROUNDS; ++i)
	{
		for(size_t j = 0; j < TEXT_SIZE; ++j) // new random plaintext
			plain[j] = plainOrig[j] = bbsRand();
		
		k1.encryptBlock(plainOrig); // create orig crypttext
		
		for(size_t j = 0; j < 128; ++j)
		{
			for(size_t k = 0; k < TEXT_SIZE; ++k) // copy orig plaintext
				plainPert[k] = plain[k];
			
			plainPert[j / 32] ^= (1 << (j % 32)); // flip bit in plain at pos i
			
			k1.encryptBlock(plainPert); // create flipped crypttext
			
			for(size_t k = 0; k < TEXT_SIZE; ++k) // xor orig and flipped crypttext
				plainPert[k] ^= plainOrig[k];
			
			writeZerosAndOnesArray(plainPert, TEXT_SIZE, out); // write
		}
	}
}

void genPlainCypherCorrelation(ostream &out)
{
	uint32_t plain[TEXT_SIZE * PLAIN_CYPHER_CORR_BLOCKS], cypher[TEXT_SIZE],
			 key[KEY_SIZE];
	KOOS k;
	
	status(PLAIN_CYPHER_CORR_ROUNDS*PLAIN_CYPHER_CORR_BLOCKS*TEXT_SIZE*32, 1048576);
	
	// get PLAIN_CYPHER_CORR_BLOCKS random 128bit plaintexts
	for (size_t i = 0; i < TEXT_SIZE * PLAIN_CYPHER_CORR_BLOCKS ; ++i)
		plain[i] = bbsRand();
	
	
	for (size_t i = 0; i < PLAIN_CYPHER_CORR_ROUNDS; ++i) {
		
		// new rand key
		for (size_t j = 0; j < KEY_SIZE; ++j)
			key[j] = bbsRand();
		
		// new koos from key
		k = KOOS(key, KEY_SIZE);
		
		// encrypt with this key PLAIN_CYPHER_CORR_BLOCKS random plaintexts
		for (size_t l = 0; l < PLAIN_CYPHER_CORR_BLOCKS; ++l) {
			// copy plain-text
			for (size_t j = 0; j < TEXT_SIZE; ++j)
				cypher[j] = plain[l*TEXT_SIZE + j];
			
			
			k.encryptBlock(cypher); // generate cypher
			
			for (size_t j = 0; j < TEXT_SIZE; ++j)
				cypher[j] ^= plain[l*TEXT_SIZE + j]; // XOR with plain
			
			
			writeZerosAndOnesArray(cypher, TEXT_SIZE, out);
		}
	}
}

void genCBCMode(ostream &out)
{
	uint32_t key[KEY_SIZE], iv[TEXT_SIZE], plain[TEXT_SIZE*CBC_MODE_BLOCKS];
	KOOS k;
	
	status(CBC_MODE_ROUNDS*TEXT_SIZE*CBC_MODE_BLOCKS*32, 1048576);
	
	for (size_t i = 0; i < TEXT_SIZE; ++i) // IV of all zeros
		iv[i] = 0;
	
	
	for (size_t i = 0; i < CBC_MODE_ROUNDS; ++i) {
		for (size_t j = 0; j < KEY_SIZE; ++j) // new random key
			key[j] = bbsRand();
		
		k = KOOS(key, KEY_SIZE);
		
		for (size_t j = 0; j < TEXT_SIZE*CBC_MODE_BLOCKS; ++j) // plain of all zeros
			plain[j] = 0;
		
		k.encryptCBC(plain, TEXT_SIZE*CBC_MODE_BLOCKS, iv); // encrypt cbc mode
		
		writeZerosAndOnesArray(plain, TEXT_SIZE*CBC_MODE_BLOCKS, out);
	}
	
}

void genRandPlainKey(ostream &out)
{
	uint32_t key[KEY_SIZE], plain[TEXT_SIZE*RAND_PLAIN_KEY_BLOCKS];
	KOOS k;
	
	status(RAND_PLAIN_KEY_ROUNDS*TEXT_SIZE*RAND_PLAIN_KEY_BLOCKS*32, 1048576);
	
	for (size_t i = 0; i < RAND_PLAIN_KEY_ROUNDS; ++i) {
		for (size_t j = 0; j < KEY_SIZE; ++j) // new random key
			key[j] = bbsRand();
		
		k = KOOS(key, KEY_SIZE);
		
		for (size_t j = 0; j < TEXT_SIZE*RAND_PLAIN_KEY_BLOCKS; ++j) // random plain
			plain[j] = bbsRand();
		
		k.encryptECB(plain, TEXT_SIZE*RAND_PLAIN_KEY_BLOCKS); // encrypt ecb mode
		
		writeZerosAndOnesArray(plain, TEXT_SIZE*RAND_PLAIN_KEY_BLOCKS, out);
		
	}
	
}



void genLowDensityKey(ostream &out)
{
	uint32_t key[KEY_SIZE], plain[TEXT_SIZE], cypher[TEXT_SIZE];
	KOOS k;
	
	status(LOW_HIGH_DENSITY_ROUNDS*(TEXT_SIZE*32+ // key all zeros
									TEXT_SIZE*32*KEY_SIZE*32+ // KEY_SIZE*32 keys with one one
									TEXT_SIZE*32*KEY_SIZE*32*(KEY_SIZE*32-1)/2) // 128*127/2 keys with two ones
									, (1+ // key all zero
									   KEY_SIZE*32+ // KEY_SIZE*32 keys with one ones
									   KEY_SIZE*32*(KEY_SIZE*32-1)/2)*TEXT_SIZE*32); // 128*127/2 keys with two ones
	
	for (size_t i = 0; i < LOW_HIGH_DENSITY_ROUNDS; ++i) {
		
		for (size_t j = 0; j < TEXT_SIZE; ++j) // new random plain
			cypher[j] = plain[j] = bbsRand();
		
		for (size_t j = 0; j < KEY_SIZE; ++j) // key of all zero
			key[j] = 0;
		
		k = KOOS(key, KEY_SIZE);
		
		k.encryptBlock(cypher); // encrypt with key all 0
		
		writeZerosAndOnesArray(cypher, TEXT_SIZE, out);
		
		for (size_t j = 0; j < KEY_SIZE*32; ++j) { // key with one one
			
			for (size_t l = 0; l < TEXT_SIZE; ++l) // copy plain
				cypher[l] = plain[l];
			
			key[j / 32] ^= (1 << (j % 32)); // flip bit in key at pos j
			
			k = KOOS(key, KEY_SIZE);
			
			k.encryptBlock(cypher); // encrypt with key with one one
			
			writeZerosAndOnesArray(cypher, TEXT_SIZE, out);
			
			key[j / 32] ^= (1 << (j % 32)); // flip j-th bit back
		}
		
		for (size_t j = 0; j < KEY_SIZE*32; ++j) { // key with two ones
			for (size_t l = j+1; l < KEY_SIZE*32; ++l) {
				
				for (size_t m = 0; m < TEXT_SIZE; ++m) // copy plain
					cypher[m] = plain[m];
				
				key[j / 32] ^= (1 << (j % 32)); // flip bit in key at pos j
				key[l / 32] ^= (1 << (l % 32)); // flip bit in key at pos l
				
				k = KOOS(key, KEY_SIZE);
				
				k.encryptBlock(cypher); // encrypt with key with two ones
				
				writeZerosAndOnesArray(cypher, TEXT_SIZE, out);
				
				key[j / 32] ^= (1 << (j % 32)); // flip bit in key at pos j
				key[l / 32] ^= (1 << (l % 32)); // flip bit in key at pos l
			}
		}
		
	}
}

void genLowDensityPlain(ostream &out)
{
	uint32_t key[KEY_SIZE], plain[TEXT_SIZE];
	KOOS k;
	
	status(LOW_HIGH_DENSITY_ROUNDS*(TEXT_SIZE*32+ // plain all zero
									TEXT_SIZE*32*TEXT_SIZE*32+ // TEXT_SIZE*32 plains with one ones
									TEXT_SIZE*32*TEXT_SIZE*32*(TEXT_SIZE*32-1)/2) // 128*127/2 plains with two ones
		   , (1+ // plain all zero
			  TEXT_SIZE*32+ // TEXT_SIZE*32 plains with one ones
			  TEXT_SIZE*32*(TEXT_SIZE*32-1)/2)*TEXT_SIZE*32); // 128*127/2 plains with two ones
	
	for (size_t i = 0; i < LOW_HIGH_DENSITY_ROUNDS; ++i) {
		
		for (size_t j = 0; j < KEY_SIZE; ++j) // new random key
			key[j] = bbsRand();
		
		for (size_t j = 0; j < TEXT_SIZE; ++j) // plain with all zero
			plain[j] = 0;
		
		k = KOOS(key, KEY_SIZE);
		
		k.encryptBlock(plain); // encrypt with plain all zero
		
		writeZerosAndOnesArray(plain, TEXT_SIZE, out);
		
		for (size_t j = 0; j < TEXT_SIZE*32; ++j) { // plain with one one
			
			for (size_t l = 0; l < TEXT_SIZE; ++l) // plain with all zero
				plain[l] = 0;
			
			plain[j / 32] ^= (1 << (j % 32)); // flip bit in plain at pos j
			
			k.encryptBlock(plain); // encrypt with plain with one one
			
			writeZerosAndOnesArray(plain, TEXT_SIZE, out);
		}
		
		for (size_t j = 0; j < TEXT_SIZE*32; ++j) { // plain with two ones
			for (size_t l = j+1; l < TEXT_SIZE*32; ++l) {
				
				for (size_t m = 0; m < TEXT_SIZE; ++m) // plain with all zero
					plain[m] = 0;
				
				plain[j / 32] ^= (1 << (j % 32)); // flip bit in plain at pos j
				plain[l / 32] ^= (1 << (l % 32)); // flip bit in plain at pos j
				
				k.encryptBlock(plain); // encrypt with plain with two ones
				
				writeZerosAndOnesArray(plain, TEXT_SIZE, out);
			}
		}
	}
}


void genHighDensityKey(ostream &out)
{
	uint32_t key[KEY_SIZE], plain[TEXT_SIZE], cypher[TEXT_SIZE];
	KOOS k;
	
	status(LOW_HIGH_DENSITY_ROUNDS*(TEXT_SIZE*32+ // key all ones
									TEXT_SIZE*32*KEY_SIZE*32+ // KEY_SIZE*32 keys with one zero
									TEXT_SIZE*32*KEY_SIZE*32*(KEY_SIZE*32-1)/2) // 128*127/2 keys with two zeros
		   , (1+ // key all one
			  KEY_SIZE*32+ // KEY_SIZE*32 keys with one zero
			  KEY_SIZE*32*(KEY_SIZE*32-1)/2)*TEXT_SIZE*32); // 128*127/2 keys with two zeros
	
	for (size_t i = 0; i < LOW_HIGH_DENSITY_ROUNDS; ++i) {
		
		for (size_t j = 0; j < TEXT_SIZE; ++j) // new random plain
			cypher[j] = plain[j] = bbsRand();
		
		for (size_t j = 0; j < KEY_SIZE; ++j) // key of all ones
			key[j] = 0xFFFFFFFF;
		
		k = KOOS(key, KEY_SIZE);
		
		k.encryptBlock(cypher); // encrypt with key all ones
		
		writeZerosAndOnesArray(cypher, TEXT_SIZE, out);
		
		for (size_t j = 0; j < KEY_SIZE*32; ++j) { // key with one zero
			
			for (size_t l = 0; l < TEXT_SIZE; ++l) // copy plain
				cypher[l] = plain[l];
			
			key[j / 32] ^= (1 << (j % 32)); // flip bit in key at pos j
			
			k = KOOS(key, KEY_SIZE);
			
			k.encryptBlock(cypher); // encrypt with key with one zero
			
			writeZerosAndOnesArray(cypher, TEXT_SIZE, out);
			
			key[j / 32] ^= (1 << (j % 32)); // flip j-th bit back
		}
		
		for (size_t j = 0; j < KEY_SIZE*32; ++j) { // key with two zeros
			for (size_t l = j+1; l < KEY_SIZE*32; ++l) {
				
				for (size_t m = 0; m < TEXT_SIZE; ++m) // copy plain
					cypher[m] = plain[m];
				
				key[j / 32] ^= (1 << (j % 32)); // flip bit in key at pos j
				key[l / 32] ^= (1 << (l % 32)); // flip bit in key at pos l
				
				k = KOOS(key, KEY_SIZE);
				
				k.encryptBlock(cypher); // encrypt with key with two zeros
				
				writeZerosAndOnesArray(cypher, TEXT_SIZE, out);
				
				key[j / 32] ^= (1 << (j % 32)); // flip bit in key at pos j
				key[l / 32] ^= (1 << (l % 32)); // flip bit in key at pos l
			}
		}
		
	}
}


void genHighDensityPlain(ostream &out)
{
	uint32_t key[KEY_SIZE], plain[TEXT_SIZE];
	KOOS k;
	
	status(LOW_HIGH_DENSITY_ROUNDS*(TEXT_SIZE*32+ // plain all ones
									TEXT_SIZE*32*TEXT_SIZE*32+ // TEXT_SIZE*32 plains with one zero
									TEXT_SIZE*32*TEXT_SIZE*32*(TEXT_SIZE*32-1)/2) // 128*127/2 plains with two zeros
		   , (1+ // plain all ones
			  TEXT_SIZE*32+ // TEXT_SIZE*32 plains with one zero
			  TEXT_SIZE*32*(TEXT_SIZE*32-1)/2)*TEXT_SIZE*32); // 128*127/2 plains with two zeros
	
	
	for (size_t i = 0; i < LOW_HIGH_DENSITY_ROUNDS; ++i) {
		
		for (size_t j = 0; j < KEY_SIZE; ++j) // new random key
			key[j] = bbsRand();
		
		for (size_t j = 0; j < TEXT_SIZE; ++j) // plain with all ones
			plain[j] = 0xFFFFFFFF;
		
		k = KOOS(key, KEY_SIZE);
		
		k.encryptBlock(plain); // encrypt with plain all ones
		
		writeZerosAndOnesArray(plain, TEXT_SIZE, out);
		
		for (size_t j = 0; j < TEXT_SIZE*32; ++j) { // plain with one zero
			
			for (size_t l = 0; l < TEXT_SIZE; ++l) // plain with all ones
				plain[l] = 0xFFFFFFFF;
			
			plain[j / 32] ^= (1 << (j % 32)); // flip bit in plain at pos j
			
			k.encryptBlock(plain); // encrypt with plain with one zero
			
			writeZerosAndOnesArray(plain, TEXT_SIZE, out);
		}
		
		for (size_t j = 0; j < TEXT_SIZE*32; ++j) { // plain with two zeros
			for (size_t l = j+1; l < TEXT_SIZE*32; ++l) {
				
				for (size_t m = 0; m < TEXT_SIZE; ++m) // plain with all ones
					plain[m] = 0xFFFFFFFF;
				
				plain[j / 32] ^= (1 << (j % 32)); // flip bit in plain at pos j
				plain[l / 32] ^= (1 << (l % 32)); // flip bit in plain at pos j
				
				k.encryptBlock(plain); // encrypt with plain with two zeros
				
				writeZerosAndOnesArray(plain, TEXT_SIZE, out);
			}
		}
	}
}








