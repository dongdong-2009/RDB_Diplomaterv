#include "ApplicationProgram\globals.h"

#include "ApplicationProgram\time.h"



//Az 500 µs alapu Systick megszakitasszamlalo
static U32 TIME_u32SystickCounter500Us  = 0;
//Az 1 ms alapu Systick megszakitasszamlalo
static U32 TIME_u32SysticCounterMs      = 0;



//A TIME modul SysTick kezeloje. Meg kell hivni minden SysTick interrupt alkalmaval.
  //A modul csak 500µs periodusu SysTick megszakitassal kompatibilis.
void TIME_SystickHandler(void)
{
  //Szamlaljuk az 500 µs-mos es az 1 ms-mos megszakitasokat
  TIME_u32SystickCounter500Us++;
  if(2 <= TIME_u32SystickCounter500Us)
  {
    TIME_u32SystickCounter500Us = 0;
    TIME_u32SysticCounterMs++;
  }
}

//A TIME modul kezelofuggvenye. Meg kell hivni az alkalmazasprogram minden ciklusaban.
void TIME_Handler(void)
{
  U32 u32AccumulatedSystickCountMs;
  
  //Atvesszuk az 1 ms-mos megszakitasszamlalo erteket.
    //A SysTick megszakitasnak itt nem kell letilva lennie, hiszen a µC
    //ezt a muveletet egy orajel alatt hajtja vegre
  u32AccumulatedSystickCountMs = TIME_u32SysticCounterMs;
  TIME_u32SysticCounterMs = 0;
  
  //Hozzaadjuk a globalis idohoz az elelt ms-mok szamat
  sGlobal.sTime.u64UpTimeMs += u32AccumulatedSystickCountMs;  
}

//Segedfuggveny idozitesek megvalositasahoz
bool TIME_DidTimeElapse(U64 u64TimerWasStartedAtMs,  //Az idozites inditasanak idopontja
                        U64 u64TimerSetValueMs)      //Az idozito beallitasa
{
  bool bResult = FALSE;
  
  //Ha elertuk a u64TimerSetValueMs erteket az idozito inditasa ota
  if((sGlobal.sTime.u64UpTimeMs - u64TimerWasStartedAtMs ) >= ( u64TimerSetValueMs ))
  {
    bResult = TRUE;
  }
  
  return bResult;
}
