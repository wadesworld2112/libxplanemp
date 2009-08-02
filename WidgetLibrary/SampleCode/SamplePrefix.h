#if __MWERKS__ || macintosh
	#define APL 1
	#define IBM 0
	#define LIN 0
#else
	#define IBM 1
	#define APL 0
	#define LIN 0
#endif