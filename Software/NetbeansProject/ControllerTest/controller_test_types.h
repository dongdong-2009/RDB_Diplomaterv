#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint8_t 		U8;
typedef uint16_t                U16;
typedef uint32_t 		U32;
typedef uint64_t                U64;

typedef int8_t 			I8;
typedef int16_t 		I16;
typedef int32_t 		I32;
typedef int64_t 		I64;

typedef float 			F32;
typedef double 			F64;

#define I16MIN                  (INT16_MIN)
#define I16MAX                  (INT16_MAX)
#define I32MIN                  (INT32_MIN)
#define I32MAX                  (INT32_MAX)
#define TRUE                    true

#define APPLICATION_TYPES_CHECK() \
    if((1 != sizeof(U8))   ||     \
       (2 != sizeof(U16))  ||     \
       (4 != sizeof(U32))  ||     \
       (8 != sizeof(U64))  ||     \
       (1 != sizeof(I8))   ||     \
       (2 != sizeof(I16))  ||     \
       (4 != sizeof(I32))  ||     \
       (8 != sizeof(I64))  ||     \
       (4 != sizeof(F32))  ||     \
       (8 != sizeof(F64)))        \
    {                             \
        while(1);                 \
    }                                                             

#endif
