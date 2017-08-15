#include "ApplicationProgram\globals.h"

#include "MCTasks.h"
#include "Parameters conversion.h"

#include "ApplicationProgram\local_io.h"



//A gomb elozo nyers erteke
static bool bPreviousUserButtonRaw = FALSE;
//A gomb elozo prellmentesitett erteke
static bool bPreviousUserButonDePrell = FALSE;
//Az idopont amikor a nyomogomb prellegni kezdett.
U64  LOCAL_IO_u64DePrellTimerStartTimeMs;
  


//A LOCAL_IO modul inicializalasa. Meg kell hivni az elso LOCAL_IO_Handler hivas elott.
void LOCAL_IO_Init(void)
{
  bool bInitialUserButtonState;
  
  //Beolvassuk a kezdeti nyomogomb erteket
  bInitialUserButtonState = LOCAL_IO_ReadRawUserButton();
  
  //Inicializaljuk a nyers jelhez tartzozo valtozokat a kezdeti ertekkel
  sGlobal.sLocalIo.bUserButtonRaw = bInitialUserButtonState;
  bPreviousUserButtonRaw = bInitialUserButtonState;

  //Inicializaljuk a prellmentesitett jelhez tartzozo valtozokat a kezdeti ertekkel  
  sGlobal.sLocalIo.bUserButtonDePrell = bInitialUserButtonState;
  bPreviousUserButonDePrell = bInitialUserButtonState;
  
  //Inicializaljuk az elfigyeleshez tartzozo valtozokat a kezdeti ertekkel  
  sGlobal.sLocalIo.bUserButtonRising = FALSE;
  sGlobal.sLocalIo.bUserButtonFalling = FALSE;
}

//A LOCAL_IO modul kezelofuggvenye. Meg kell hivni az alkalmazasprogram minden ciklusaban.
void LOCAL_IO_Handler(void)
{
  //A nyomogomb nyers jelenek feldolgozasa
    //Beolvassuk a nyomogomb allapotat majd taroljuk azt a globalis adatokban
  sGlobal.sLocalIo.bUserButtonRaw = LOCAL_IO_ReadRawUserButton();

  //A nyomogomb nyers jelenek prellmentesitese
    //Elmenjuk az idopontot a nyers jel utolso valtozasanak idopontjat
    //Ha eltellt a megadott prellmentesitesi ido a valtozas ota akkor az 
    //uj prellmentesitett jel a nyers jel aktualis erteke lesz
  if( sGlobal.sLocalIo.bUserButtonRaw != bPreviousUserButtonRaw )
  {
    LOCAL_IO_u64DePrellTimerStartTimeMs = sGlobal.sTime.u64UpTimeMs;
  }
  else
  {
    if(TRUE == TIME_DidTimeElapse(LOCAL_IO_u64DePrellTimerStartTimeMs, LOCAL_IO_PRELL_LIMIT_MS))
    {
      sGlobal.sLocalIo.bUserButtonDePrell = sGlobal.sLocalIo.bUserButtonRaw;
    }
  }

  //A nyomogomb prellmentesitett jelenek felfuto el detektalasa
  if(( sGlobal.sLocalIo.bUserButtonDePrell != bPreviousUserButonDePrell ) && 
     ( TRUE == sGlobal.sLocalIo.bUserButtonDePrell ))
  {
    sGlobal.sLocalIo.bUserButtonRising = TRUE;
  }	
  else
  {
    sGlobal.sLocalIo.bUserButtonRising = FALSE;
  }

  //A nyomogomb prellmentesitett jelenek lefuto el detektalasa
  if(( sGlobal.sLocalIo.bUserButtonDePrell != bPreviousUserButonDePrell ) && 
     ( FALSE == sGlobal.sLocalIo.bUserButtonDePrell ))
  {
    sGlobal.sLocalIo.bUserButtonFalling = TRUE;
  }	
  else
  {
    sGlobal.sLocalIo.bUserButtonFalling = FALSE;
  }

  //Elmentjuk az adatokat a kovetkezo programciklusra
  bPreviousUserButtonRaw = sGlobal.sLocalIo.bUserButtonRaw;
  bPreviousUserButonDePrell = sGlobal.sLocalIo.bUserButtonDePrell;
}

bool LOCAL_IO_ReadRawUserButton(void)
{
  bool bResult = FALSE; //Alapbol azt mondjuk, hogy nincs megnyomva
  
  //Beolvassuk a nyomogomb allapotat majd taroljuk azt
  if(START_STOP_POLARITY == GPIO_ReadInputDataBit(START_STOP_GPIO_PORT, START_STOP_GPIO_PIN))
  {
    bResult = TRUE;
  }

  return bResult;
}
