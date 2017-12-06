/*
 * file: mp.c
 *
 * DESCRIPTION
 *
 * These functions comprise a multi-precision integer arithmetic
 * and discrete function package.
 */

#include	"bbs.h"

#define	MAXPLEN		384
BBS g_bbsSeed;

int performanceTest ()
{                         // 10000  ca 57 sec on cluster-linux-nehalem
	int i, start, end, rands=1000; // ca 21 sec on MacBook Pro 2,5 GHz i5
	bbsSeed("10d6333cfac8e30e808d2192f7c0439480da79db9bbca1667d73be9a677ed31311f3b830937763837cb7b1b1dc75f14eea417f84d9625628750de99e7ef1e976",
	"E65097BAEC92E70478CAF4ED0ED94E1C94B154466BFB9EC9BE37B2B0FF8526C222B76E0E915017535AE8B9207250257D0A0C87C0DACEF78E17D1EF9DC44FD91F",
	"E029AEFCF8EA2C29D99CB53DD5FA9BC1D0176F5DF8D9110FD16EE21F32E37BA86FF42F00531AD5B8A43073182CC2E15F5C86E8DA059E346777C9A985F7D8A867");
	
	start = time(NULL);
	for( i = 0; i < rands; ++i)
	{
		bbsRand();
	}
	end = time(NULL);
	printf("Time to calculate %d rand numbers: %d\n",rands, end-start );
	return 0;
}

void bbsSeed(const char* seed, const char* p, const char* q)
{
	uint8_t s[64];
#ifndef FASTER_RAND
	ahtopb(p, g_bbsSeed.p, 64); // zb "E65097BAEC92E70478CAF4ED0ED94E1C94B154466BFB9EC9BE37B2B0FF8526C222B76E0E915017535AE8B9207250257D0A0C87C0DACEF78E17D1EF9DC44FD91F"
	ahtopb(q, g_bbsSeed.q, 64); // zb "E029AEFCF8EA2C29D99CB53DD5FA9BC1D0176F5DF8D9110FD16EE21F32E37BA86FF42F00531AD5B8A43073182CC2E15F5C86E8DA059E346777C9A985F7D8A867"
	memset(g_bbsSeed.n, 0x00, 128);
	Mult(g_bbsSeed.n, g_bbsSeed.p, 64, g_bbsSeed.q, 64);
	memset(s, 0x00, 64);
	ahtopb(seed, s, 64);
	memset(g_bbsSeed.x, 0x00, 256);
	ModSqr(g_bbsSeed.x, s, 64, g_bbsSeed.n, 128);
#else
	srand(time(NULL));
#endif
}

uint32_t bbsRand()
{
	uint32_t ret = 0, i;
#ifndef FASTER_RAND
	// new square
	for(i = 0; i < 32; ++i)
	{
		ModSqr(g_bbsSeed.x, g_bbsSeed.x, 128, g_bbsSeed.n, 128);
		memcpy(g_bbsSeed.x, g_bbsSeed.x+128, 128);
		
		ret |= (g_bbsSeed.x[127] & 0x01) << i;
	}
#else
	ret = rand();
#endif
	return ret;
}


/*****************************************
** greater - Test if x > y               *
**                                       *
** Returns TRUE (1) if x greater than y, *
** otherwise FALSE (0).                  *
**                                       *
** Parameters:                           *
**                                       *
**  x      Address of array x            *
**  y      Address of array y            *
**  l      Length both x and y in bytes  *
**                                       *
******************************************/ 
int greater(uint8_t *x, uint8_t *y, int l)
{
	int		i;

	for ( i=0; i<l; i++ )
		if ( x[i] != y[i] )
			break;

	if ( i == l )
		return 0;

	if ( x[i] > y[i] )
		return 1;

	return 0;
}


/*****************************************
** less - Test if x < y                  *
**                                       *
** Returns TRUE (1) if x less than y,    *
** otherwise FALSE (0).                  *
**                                       *
** Parameters:                           *
**                                       *
**  x      Address of array x            *
**  y      Address of array y            *
**  l      Length both x and y in bytes  *
**                                       *
******************************************/ 
int less(uint8_t *x, uint8_t *y, int l)
{
	int		i;

	for ( i=0; i<l; i++ )
		if ( x[i] != y[i] )
			break;

	if ( i == l ) {
		return 0;
	}

	if ( x[i] < y[i] ) {
		return 1;
	}

	return 0;
}


/*****************************************
** bshl - shifts array left              *
**                  by one bit.          *
**                                       *	
** x = x * 2                             *
**                                       *
** Parameters:                           *	
**                                       *
**  x      Address of array x            *
**  l      Length array x in bytes       *
**                                       *
******************************************/ 
uint8_t bshl(uint8_t *x, int l)
{
	uint8_t	*p;
	int		c1, c2;

	p = x + l - 1;
	c1 = 0;
	c2 = 0;
	while ( p != x ) {
		if ( *p & 0x80 )
			c2 = 1;
		*p <<= 1;  /* shift the word left once (ls bit = 0) */
		if ( c1 )
			*p |= 1;
		c1 = c2;
		c2 = 0;
		p--;
	}

	if ( *p & 0x80 )
		c2 = 1;
	*p <<= 1;  /* shift the word left once (ls bit = 0) */
	if ( c1 )
		*p |= (uint16_t)1;

	return (uint8_t)c2;
}


/*****************************************
** bshr - shifts array right             *
**                   by one bit.         *
**                                       *	
** x = x / 2                             *
**                                       *
** Parameters:                           *	
**                                       *
**  x      Address of array x            *
**  l      Length array x in bytes       *	
**                                       *
******************************************/
void bshr(uint8_t *x, int l)	
{
	uint8_t	*p;
	int		c1,c2;

	p = x;
	c1 = 0;
	c2 = 0;
	while ( p != x+l-1 ) {
		if ( *p & 0x01 )
			c2 = 1;
		*p >>= 1;  /* shift the word right once (ms bit = 0) */
		if ( c1 )
			*p |= 0x80;
		c1 = c2;
		c2 = 0;
		p++;
	}

	*p >>= 1;  /* shift the word right once (ms bit = 0) */
	if ( c1 )
		*p |= 0x80;
}


/*****************************************
** Mult - Multiply two integers          *
**                                       *
** A = B * C                             *
**                                       *
** Parameters:                           *	
**                                       *
**  A      Address of the result         *
**  B      Address of the multiplier     *
**  C      Address of the multiplicand   *
**  LB      Length of B in bytes         *
**  LC      Length of C in bytes         *
**                                       *
**  NOTE:  A MUST be LB+LC in length     *
**                                       *
******************************************/
int Mult(uint8_t *A, uint8_t *B, int LB, uint8_t *C, int LC)
{
	int		i, j, k = 0, LA;
	uint16_t	result;

	LA = LB + LC;

	for ( i=LB-1; i>=0; i-- ) {
		result = 0;
		for ( j=LC-1; j>=0; j-- ) {
			k = i+j+1;
			result = (uint16_t)A[k] + ((uint16_t)(B[i] * C[j])) + (result >> 8);
			A[k] = (uint8_t)result;
			}
		A[--k] = (uint8_t)(result >> 8);
	}

	return 0;
}


void ModSqr(uint8_t *A, uint8_t *B, int LB, uint8_t *M, int LM)
{

	Square(A, B, LB);
	Mod(A, 2*LB, M, LM);
}

void ModMult(uint8_t *A, uint8_t *B, int LB, uint8_t *C, int LC, uint8_t *M, int LM)
{
	Mult(A, B, LB, C, LC);
	Mod(A, (LB+LC), M, LM);
}


/*****************************************
** smult - Multiply array by a scalar.   *
**                                       *
** A = b * C                             *
**                                       *
** Parameters:                           *	
**                                       *
**  A      Address of the result         *
**  b      Scalar (1 uint8_t)               *
**  C      Address of the multiplicand   *
**  L      Length of C in bytes          *
**                                       *
**  NOTE:  A MUST be L+1 in length       *
**                                       *
******************************************/
void smult(uint8_t *A, uint8_t b, uint8_t *C, int L)
{
	int		i;
	uint16_t	result;

	result = 0;
	for ( i=L-1; i>0; i-- ) {
		result = A[i] + ((uint16_t)b * C[i]) + (result >> 8);
		A[i] = (uint8_t)(result & 0xff);
		A[i-1] = (uint8_t)(result >> 8);
	}
}

/*****************************************
** Square() - Square an integer          *
**                                       *
** A = B^2                               *
**                                       *
** Parameters:                           *
**                                       *
**  A      Address of the result         *
**  B      Address of the operand        *
**  L      Length of B in bytes          *
**                                       *
**  NOTE:  A MUST be 2*L in length       *
**                                       *
******************************************/
void Square(uint8_t *A, uint8_t *B, int L)
{
	Mult(A, B, L, B, L);
}




/* DivMod:
 *
 *   computes:
 *         quot = x / n
 *         rem = x % n
 *   returns:
 *         length of "quot"
 *
 *  len of rem is lenx+1
 */
int DivMod(uint8_t *x, int lenx, uint8_t *n, int lenn, uint8_t *quot, uint8_t *rem)
{
	uint8_t	*tx, *tn, *ttx, *ts, bmult[1];
	int		i, shift, lgth_x, lgth_n, t_len, lenq;
	uint16_t	tMSn, mult;
	uint64_t	tMSx;
	int		underflow;

	tx = x;
	tn = n;
	
	/* point to the MSD of n  */
	for ( i=0, lgth_n=lenn; i<lenn; i++, lgth_n-- ) {
		if ( *tn )
			break;
		tn++;
	}
	if ( !lgth_n )
		return 0;
	
	/* point to the MSD of x  */
	for ( i=0, lgth_x=lenx; i<lenx; i++, lgth_x-- ) {
		if ( *tx )
			break;
		tx++;
	}
	if ( !lgth_x )
		return 0;

	if ( lgth_x < lgth_n )
		lenq = 1;
	else
		lenq = lgth_x - lgth_n + 1;
	memset(quot, 0x00, lenq);
	
	/* Loop while x > n,  WATCH OUT if lgth_x == lgth_n */
	while ( (lgth_x > lgth_n) || ((lgth_x == lgth_n) && !less(tx, tn, lgth_n)) ) {
		shift = 1;
		if ( lgth_n == 1 ) {
			if ( *tx < *tn ) {
				tMSx = (uint16_t) (((*tx) << 8) | *(tx+1));
				tMSn = *tn;
				shift = 0;
			}
			else {
				tMSx = *tx;
				tMSn = *tn;
			}
		}
		else if ( lgth_n > 1 ) {
			tMSx = (uint16_t) (((*tx) << 8) | *(tx+1));
			tMSn = (uint16_t) (((*tn) << 8) | *(tn+1));
			if ( (tMSx < tMSn) || ((tMSx == tMSn) && less(tx, tn, lgth_n)) ) {
				tMSx = (tMSx << 8) | *(tx+2);
				shift = 0;
			}
		}
		else {
			tMSx = (uint16_t) (((*tx) << 8) | *(tx+1));
			tMSn = *tn;
			shift = 0;
		}

		mult = (uint16_t) (tMSx / tMSn);
		if ( mult > 0xff )
			mult = 0xff;
		bmult[0] = mult & 0xff;

		ts = rem;
		do {
			memset(ts, 0x00, lgth_x+1);
			Mult(ts, tn, lgth_n, bmult, 1);

			underflow = 0;
			if ( shift ) {
				if ( ts[0] != 0 )
					underflow = 1;
				else {
					for ( i=0; i<lgth_x; i++ )
						ts[i] = ts[i+1];
					ts[lgth_x] = 0x00;
				}
			}
			if ( greater(ts, tx, lgth_x) || underflow ) {
				bmult[0]--;
				underflow = 1;
			}
			else
				underflow = 0;
		} while ( underflow );
		sub(tx, lgth_x, ts, lgth_x);
		if ( shift )
			quot[lenq - (lgth_x - lgth_n) - 1] = bmult[0];
		else
			quot[lenq - (lgth_x - lgth_n)] = bmult[0];
		
		ttx = tx;
		t_len = lgth_x;
		for ( i=0, lgth_x=t_len; i<t_len; i++, lgth_x-- ) {
			if ( *ttx )
				break;
			ttx++;
		}
		tx = ttx;
	}
	memset(rem, 0x00, lenn);
	if ( lgth_x )
		memcpy(rem+lenn-lgth_x, tx, lgth_x);

	return lenq;
}


/* 
 * Mod - Computes an integer modulo another integer
 *
 * x = x (mod n)
 *
 */
void Mod(uint8_t *x, int lenx, uint8_t *n, int lenn)
{
	uint8_t	quot[MAXPLEN+1], rem[2*MAXPLEN+1];

	memset(quot, 0x00, sizeof(quot));
	memset(rem, 0x00, sizeof(rem));
	if ( DivMod(x, lenx, n, lenn, quot, rem) ) {
		memset(x, 0x00, lenx);
		memcpy(x+lenx-lenn, rem, lenn);
	}
}


/*****************************************
** sub - Subtract two integers           *
**                                       *
** A = A - B                             *
**                                       *
**                                       *
** Parameters:                           *	
**                                       *
**  A      Address of subtrahend integer *
**  B      Address of subtractor integer *
**  L      Length of A and B in bytes    *
**                                       *
**  NOTE: In order to save RAM, B is     *
**        two's complemented twice,      *
**        rather than using a copy of B  *
**                                       *
******************************************/
void sub(uint8_t *A, int LA, uint8_t *B, int LB)
{
	uint8_t	*tb;

	tb = (uint8_t *)calloc(LA, 1);
	memcpy(tb, B, LB);
	negate(tb, LB);
	add(A, LA, tb, LA);

	FREE(tb);
}


/*****************************************
** negate - Negate an integer            *
**                                       *
** A = -A                                *
**                                       *
**                                       *
** Parameters:                           *	
**                                       *
**  A      Address of integer to negate  *
**  L      Length of A in bytes          *
**                                       *
******************************************/
int negate(uint8_t *A, int L)
{
	int		i, tL;
	uint16_t	accum;

	/* Take one's complement of A */
	for ( i=0; i<L; i++ )
		A[i] = ~(A[i]);

	/* Add one to get two's complement of A */
	accum = 1;
	tL = L-1;
	while ( accum && (tL >= 0) ) {
		accum += A[tL];
		A[tL--] = (uint8_t)(accum & 0xff);
		accum = accum >> 8;
	}

	return accum;
}


/*
 * add()
 *
 * A = A + B
 *
 * LB must be <= LA
 *
 */
uint8_t add(uint8_t *A, int LA, uint8_t *B, int LB)
{
	int		i, indexA, indexB;
	uint16_t	accum;

	indexA = LA - 1; 	/* LSD of result */
	indexB = LB - 1; 	/* LSD of B */

	accum = 0;
	for ( i = 0; i < LB; i++ ) {
		accum += A[indexA];
		accum += B[indexB--];
		A[indexA--] = (uint8_t)(accum & 0xff);
		accum = accum >> 8;
	}

	if ( LA > LB )
		while ( accum  && (indexA >= 0) ) {
			accum += A[indexA];
			A[indexA--] = (uint8_t)(accum & 0xff);
			accum = accum >> 8;
		}

	return (uint8_t)accum;
}

/**********************************************************************/
/*  Performs byte reverse for PC based implementation (little endian) */
/**********************************************************************/
void byteReverse(uint64_t *buffer, int byteCount)
{
	uint64_t value;
	int count;

	byteCount /= sizeof( uint64_t );
	for( count = 0; count < byteCount; count++ ) {
		value = ( buffer[ count ] << 16 ) | ( buffer[ count ] >> 16 );
		buffer[ count ] = ( ( value & 0xFF00FF00L ) >> 8 ) | ( ( value & 0x00FF00FFL ) << 8 );
	}
}

void
ahtopb (const char *ascii_hex, uint8_t *p_binary, int bin_len)
{
	uint8_t    nibble;
	int     i; 
	
	for ( i=0; i<bin_len; i++ ) {
        nibble = ascii_hex[i * 2];
	    if ( nibble > 'F' )
	        nibble -= 0x20;   
	    if ( nibble > '9' )
	        nibble -= 7;      
	    nibble -= '0';   
	    p_binary[i] = nibble << 4;
		
	    nibble = ascii_hex[i * 2 + 1];
	    if ( nibble > 'F' )
			nibble -= 0x20;
        if ( nibble > '9' )
            nibble -= 7;   
        nibble -= '0';
		p_binary[i] += nibble;
	}
}