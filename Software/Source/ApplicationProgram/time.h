#ifndef TIME_H
#define TIME_H



//A TIME modul globalis adatai
typedef struct
{
  U64 u64UpTimeMs;
} S_TIME_GLOBAL_DATA;



//A TIME modul SysTick kezeloje. Meg kell hivni minden SysTick interrupt alkalmaval.
  //A modul csak 500µs periodusu SysTick megszakitassal kompatibilis.
void TIME_SystickHandler(void);
//A TIME modul kezelofuggvenye. Meg kell hivni az alkalmazasprogram minden ciklusaban.
void TIME_Handler(void);
//Segedfuggveny idozitesek megvalositasahoz
bool TIME_DidTimeElapse(U64 u64TimerWasStartedAtMs,  //Az idozites inditasanak idopontja
                        U64 u64TimerSetValueMs);     //Az idozito beallitasa




#endif
