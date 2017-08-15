#ifndef TYPES_H
#define TYPES_H



#include <stdint.h>
#include "MC_type.h"



//Az elojel nelkuli tipusok definialasa
typedef uint8_t                 U8;
typedef uint16_t                U16;
typedef uint32_t                U32;
typedef uint64_t                U64;

//Az elojeles tipusok definialasa
typedef int8_t                  I8;
typedef int16_t                 I16;
typedef int32_t                 I32;
typedef int64_t                 I64;

//A lebegopontos tipusok definialasa
typedef float                   F32;
typedef double                  F64;

//A szoveges tipusok definialasa
typedef  const char *           TEXT;



//Az elojel nelkuli tipusok szelsoertekeinek definialasa
#define U8MIN                   (UINT8_MIN)
#define U8MAX                   (UINT8_MAX)
#define U16MIN                  (UINT16_MIN)
#define U16MAX                  (UINT16_MAX)
#define U32MIN                  (UINT32_MIN)
#define U32MAX                  (UINT32_MAX)
#define U64MIN                  (UINT64_MIN)
#define U64MAX                  (UINT64_MAX)

//Az elojeles tipusok szelsoertekeinek definialasa
#define I8MIN                   (INT8_MIN)
#define I8MAX                   (INT8_MAX)
#define I16MIN                  (INT16_MIN)
#define I16MAX                  (INT16_MAX)
#define I32MIN                  (INT32_MIN)
#define I32MAX                  (INT32_MAX)
#define I64MIN                  (INT64_MIN)
#define I64MAX                  (INT64_MAX)



//Az alkalmazas tipusdefinicioinak ellenorzeset vegzo definicio
  //Amennyiben valamekyik tipusdefinicio nem a vart merettel rendelkezik,
  //leallitjuk az firmware futasat. 
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
