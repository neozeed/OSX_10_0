/* For the Cylink bignum stuff */
#define LITTLE_ENDIAN	1234
#define BYTE_ORDER		LITTLE_ENDIAN

/* This makes cylink compile the following listed for us.
   without this, we get unresolved externals on them:
	lbnMulAdd1_32()
	lbnMulN1_32()
	lbnMulN1_32()
	lbnDiv21_32()
	lbnMulSub1_32()
	lbnModQ_32()
*/
typedef _int64 bnword64;
#define BNWORD64 bnword64

/* If you care to implement the abovementioned functions, use this, since it forward-declares them for you */
/* #define BNINCLUDE lbn80386.h */
