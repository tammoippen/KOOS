#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include "utilities.h"
#include "KOOSParameter.h"
#include "KOOS.h"


using namespace std;

int main1 (int argc, char * const argv[]) {
	uint32_t plain[] = {0x11111111,0x22222222,0x33333333,0x44444444};
    cout << "Test ADD(3,4) = 7 ? " << (ADD(3,4) == 7) << endl;
	cout << "Test SUB(ADD(3,4), 4) = 3 ? " << (SUB(ADD(3,4),4) == 3) << endl;
	cout << "Test XOR(3,5) = 6 ? " << (XOR(3,5) == 6 ) << endl;
	cout << "Test LROT(3,5) = 96 ? " << (LROT(3,5) == 96)  << endl;
	cout << "Test RROT(96,5) = 3 ? " << (RROT(96,5) == 3)  << endl;
	cout << "Test MWC(0x12345678,0x87654321) = 0x1BDC87B5 ? " << (MWC(0x12345678,0x87654321) == 0x1BDC87B5)  << endl;
//	cout << "Test multInv(0x12345678,mult(0x12345678,0x87654321)) = 0x87654321 ? " << (multInv(0x12345678,mult(0x12345678,0x87654321)) == 0x87654321) << endl;
	cout << "Test fBox(0x12345678,0x87654321) = 0xE923BEA7 ? " << (fBox(0x12345678,0x87654321) == 0xE923BEA7) << endl;
	feistel(plain, 0, 0x12345678, 0x87654321, 0x9abcdef0, E_FUNCTION);
	feistelInv(plain, 0, 0x12345678, 0x87654321, 0x9abcdef0, E_FUNCTION);
	cout << "Test feistel with E_Function ? " << (plain[0] == 0x11111111 && plain[1] == 0x22222222 && plain[2] == 0x33333333 && plain[3] == 0x44444444) << endl;
	feistel(plain, 0, 0x12345678, 0x87654321, 0x9abcdef0, F_BOX);
	feistelInv(plain, 0, 0x12345678, 0x87654321, 0x9abcdef0, F_BOX);
	cout << "Test feistel with fBox ? " << (plain[0] == 0x11111111 && plain[1] == 0x22222222 && plain[2] == 0x33333333 && plain[3] == 0x44444444) << endl;
    
	feistel(plain, 0, 0x12345678, 0x87654321, 0x9abcdef0, MWC_RAND);
	feistelInv(plain, 0, 0x12345678, 0x87654321, 0x9abcdef0, MWC_RAND);
	cout << "Test feistel with MWC ? " << (plain[0] == 0x11111111 && plain[1] == 0x22222222 && plain[2] == 0x33333333 && plain[3] == 0x44444444) << endl;
	
	srand(time(NULL));
	for (size_t i = 0; i < 2; ++i) {
		uint64_t parameter = rand() | ((uint64_t)rand() << 32);
		cout << i+1 << ". KOOSParameter: 0x" << hex << setw(16) << parameter << endl;
		cout << KOOSParameter(parameter) << endl << endl;
	}
	
	uint32_t op1 = 0x33221100, op2 = 0x77665544, p = 0x12345670;
	blockPerm(op1, op2, p);
	cout << "Test blockPerm(0x33221100, 0x77665544, 0x12345670) ? " << (op1 == 0x44332211 && op2 == 0x00776655) << endl;
	blockPerm(op1, op2, createInvPermutation(p, 8));
	cout << "Test blockPerm(0x33221100, 0x77665544, createInvPermutation(0x12345670,8)) ? " << (op1 == 0x33221100 && op2 == 0x77665544) << endl;
	
	uint32_t key[] = {0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777, 0x88888888};
	KOOS k(key, 8);
	k.encryptBlock(plain);
	k.decryptBlock(plain);
	cout << "Test encrypt / decrypt ? " << (plain[0] == 0x11111111 && plain[1] == 0x22222222 && plain[2] == 0x33333333 && plain[3] == 0x44444444) << endl;
	
	k.encryptECB(key, 8);
	k.decryptECB(key, 8);
	cout << "Test encrypt / decrypt ECB ? " << (key[0] == 0x11111111 && key[1] == 0x22222222 && key[2] == 0x33333333 && key[3] == 0x44444444 &&
												key[4] == 0x55555555 && key[5] == 0x66666666 && key[6] == 0x77777777 && key[7] == 0x88888888) << endl;
	
	uint32_t iv[] = {0xaaaaaaaa, 0xbbbbbbbb, 0xcccccccc, 0xdddddddd};
	k.encryptCBC(key, 8, iv);
	k.decryptCBC(key, 8, iv);
	cout << "Test encrypt / decrypt CBC ? " << (key[0] == 0x11111111 && key[1] == 0x22222222 && key[2] == 0x33333333 && key[3] == 0x44444444 &&
												key[4] == 0x55555555 && key[5] == 0x66666666 && key[6] == 0x77777777 && key[7] == 0x88888888) << endl;
	return 0;
}
