#ifndef _BBS_H
#define _BBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef FASTER_RAND
//#define FASTER_RAND
#endif
#define	FREE(A)	if ( (A) ) { free((A)); (A) = NULL; }
	
	
typedef struct _BBS_stats {
	uint8_t p[64], q[64], n[128], x[256];
	} BBS;

void bbsSeed(const char* seed, const char* p, const char* q);
uint32_t bbsRand();


/*
 * Function Declarations
 */
int		greater(uint8_t *x, uint8_t *y, int l);
int		less(uint8_t *x, uint8_t *y, int l);
uint8_t	bshl(uint8_t *x, int l);
void	bshr(uint8_t *x, int l);
int		Mult(uint8_t *A, uint8_t *B, int LB, uint8_t *C, int LC);
void	ModSqr(uint8_t *A, uint8_t *B, int LB, uint8_t *M, int LM);
void	ModMult(uint8_t *A, uint8_t *B, int LB, uint8_t *C, int LC, uint8_t *M, int LM);
void	smult(uint8_t *A, uint8_t b, uint8_t *C, int L);
void	Square(uint8_t *A, uint8_t *B, int L);
//void	ModExp(uint8_t *A, uint8_t *B, int LB, uint8_t *C, int LC, uint8_t *M, int LM);
int		DivMod(uint8_t *x, int lenx, uint8_t *n, int lenn, uint8_t *quot, uint8_t *rem);
void	Mod(uint8_t *x, int lenx, uint8_t *n, int lenn);
//void	Div(uint8_t *x, int lenx, uint8_t *n, int lenn);
void	sub(uint8_t *A, int LA, uint8_t *B, int LB);
int		negate(uint8_t *A, int L);
uint8_t	add(uint8_t *A, int LA, uint8_t *B, int LB);
//void	prettyprintBstr(char *S, uint8_t *A, int L);
void	byteReverse(uint64_t *buffer, int byteCount);
void	ahtopb (const char *ascii_hex, uint8_t *p_binary, int bin_len);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: _BBS_H */
