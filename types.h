typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

typedef unsigned char uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned int  uint32_t;
typedef unsigned long int  uint64_t;

#if __WORDSIZE == 64
typedef uint32_t uintptr_t;
#else
typedef uint64_t uintptr_t;
#endif
