#ifndef GLOBALS_H
#define GLOBALS_H



#include "string.h"

#include "ApplicationProgram\types.h"
#include "ApplicationProgram\time.h"
#include "ApplicationProgram\local_io.h"



//A globalis adatokat tartalmazo struktura deklaracioja
typedef struct
{
  S_TIME_GLOBAL_DATA      sTime;      //A TIME modul globalis adatai
  S_LOCAL_IO_GLOBAL_DATA  sLocalIo;   //A LOCAL_IO modul globalis adatai
} S_GLOBAL_DATA;



//A globalis adatokat minden modulban elerhetove tesszuk ahol ez a header fajl inkludolva lesz.
extern S_GLOBAL_DATA sGlobal;



#endif
