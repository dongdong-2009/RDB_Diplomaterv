#include "stdio.h"
#include "string.h"

#include "ApplicationProgram\globals.h"

#include "UITask.h"
#include "MCTasks.h"
#include "Parameters conversion.h"

#include "ApplicationProgram\pi_controller.h"



//A tesztfolyamat allapotai
typedef enum
{ 
  E_TEST_STANDBY,             //Keszenleti allapot
  E_TEST_BUTTON_WAS_PRESSED,  //A nyomogomb vizsgalata
  E_TEST_CW,                  //Forgas CW iranyban
  E_TEST_CCW,                 //Forgas CCW iranyban
  E_TEST_STOP,                //Allo allapot
  E_TEST_FAILURE              //Hiba allapot
} E_TEST_STATES;

//A Scope funkciohoz tartozo adatsturktura deklaralasa
typedef struct
{
  bool                  bFunctionEnabled;
  bool                  bPeriodicSendingIsOn;
  U64                   u64msTimeOfTheLastTransmission;
  TEXT                  tHeadlineText;
  U8                    u8HeadlineTextLength;
} S_SCOPE_DATA;



void MOTOR_TestProcessStateMachineHandler(void);
void MOTOR_SpeedControlHandler(void);
void MOTOR_ScopeFunctionSendHeadline(void);
void MOTOR_ScopeFunctionStartPeriodicSending(void);
void MOTOR_ScopeFunctionStopPeriodicSending(void);
void MOTOR_ScopeFunctionPeriodicSendingHandler(void);
void MOTOR_SetCurrentReference(I16 i16CurrentSetValue);
bool MOTOR_TargetSpeedWasReached(void);  



//A tesztfolyamat soran hasznal abszolut +- celsebesseg ertek
#define MOTOR_TEST_PROCESS_ABS_TARGET_SPEED_RPM            ((I16) 2500) 
//A tesztfolyamat alatt hasznalt abszolut +- sebesseg beallasi limit
  //A celsebesseg +- ez az ertek koze kell beallnia a motornak a teszt soran es
  //a savon belul maradnia egy adott ideig ahhoz, hogy azt mondjuk,
  //hogy elertuk a celsebesseget
#define MOTOR_TARGET_SPEED_REACHED_ABS_LIMIT_RPM           ((I16) 30)
//A tesztfolyamat soran ennyi ms-ig kell a celsebesseg +- a limit erteken belul
  //maradni a motornak ahhoz, hogy elertuk a celsebesseget
#define MOTOR_TARGET_SPEED_REACHED_TIME_LIMIT_MS           ((U64) 500)
#warning "A meresek alapjan 1ms. Doskiban meg kell keresni..."
//A szabalyozo mintaveteli ideje ms-ban
#define MOTOR_SPEED_CONTROLLER_SAMPLING_PERIOD_MS          ((U64) 1)
//A motor sebesseg RPM-ben valo ertekenek kiszamolasahoz hasznalt konstans:
  //RPM = mert_ertek * MOTOR_SPEED_TO_RPM_CONSTANT
#define MOTOR_SPEED_TO_RPM_CONSTANT                        ((I16) 6)
//A Scope funkcio soran hasznalt adatbuffer merete byte-okban
#define SCOPE_FUNCTION_TRANSMIT_BUFFER_SIZE_IN_BYTES       ((U8)  100)
//A Scope funkcio periodikus adatkuldesenek periodusideje ms-ban
#define SCOPE_FUNCTION_PERIODIC_TRANSMIT_PERIOD_MS         ((U64)  10)



//A motorhoz tartozo MCInterface objektum cime 
static CMCI                 poMCI;
//A motorhoz tartozo MCTuning objektum cime 
static CMCT                 poMCT;
//A fordulatszamszabalyozo adatai
static S_PI_CONTROLLER_DATA sPIControllerObject;
//A szabalyozo utolso hivasanak idopontja
static U64                  u64TimeOfTheLastControllerCallMs    = 0;
//A szabalyozo engedelyezojele
static bool                 bControllerEnable                   = FALSE;
//Az aramszabalyozo alapjele
static I16                  i16CurrentSetValue                  = 0;
//A fordulatszamszabalyozo alapjele
static I16                  i16SpeedSetValueRpm                 = 0;
//Az aktualis mert fordulatszam ertek
static I16                  i16SpeedActualValueRpm              = 0;
//A fordulatszamszabalyozo P tagjanak erositese
static F32                  f32KpSetting                        = 2.0;        //Tapasztalati uton nyert mukodo ertek
//A fordulatszamszabalyozo I tagjanak integralasi ideje
static F32                  f32TiSetting                        = 0.4;        //Tapasztalati uton nyert mukodo ertek
//A fordulatszamszabalyozo abszolut +- maximum kimeneti erteke
static I16                  i16AbsControllerOutputLimitSetValue = 5000;       //Tapasztalati uton nyert mukodo ertek
//A tesztfolyamat allapotvaltozoja
static E_TEST_STATES        eTestProcessState                   = E_TEST_STANDBY;
//A Scope funkcio adatai
static S_SCOPE_DATA         sScopeData = 
{
  .bFunctionEnabled               = FALSE,
  .bPeriodicSendingIsOn           = FALSE,
  .u64msTimeOfTheLastTransmission = 0, 
  .tHeadlineText                  = "\n\r\n\r\n\r\n\ru64TimeOfTheLastControllerCallMs, eTestProcessState, i16SpeedSetValueRpm, i16SpeedActualValueRpm, i16CurrentSetValue\n\r",
  .u8HeadlineTextLength           = 0       
};


//A motor modul inicializalasat vegzo fuggveny.
  //Meg kell hivni a MOTOR_Handler fuggveny elso hivasa elott
void MOTOR_Init(void *oMCI_Address,         //A motorhoz tartozo MCInterface objektum cime 
                void *oMCT_Address,         //A motorhoz tartozo MCTuning objektum cime 
                bool bEnableScopeFunction)  //A scope funkcio engedelyezese
{
  //Ellenorizzuk a kapott parametereket
  if((NULL != oMCI_Address) && (NULL != oMCT_Address))
  {
    poMCI = (CMCI) oMCI_Address;
    poMCT = (CMCT) oMCT_Address;
  }
  else
  {
    //Hiba: Nem megfelelo cimeket kaptunk.
    while(1);
  }
  
  //A scope funkcio inicializalasa
  sScopeData.bFunctionEnabled     = bEnableScopeFunction;
  sScopeData.u8HeadlineTextLength = strlen((const char *) sScopeData.tHeadlineText);
  
  //A fordulatszamszabalyozo inicializalasa
  PI_CONTROLLER_Init(&sPIControllerObject, MOTOR_SPEED_CONTROLLER_SAMPLING_PERIOD_MS);  
  
  //Az aram alapjelek inicializalasa
  MOTOR_SetCurrentReference(0);
  
  //Az aramszabalyozo ellenorzese
  if(STC_TORQUE_MODE != MCI_GetControlMode(poMCI))
  {
    //Hiba: Az alapprogram nem aramszabalyozo uzemmodban van
    while(1);
  }
  //Az aramszabalyozo ellenorzese
  if(IDLE != MCI_GetSTMState(poMCI))  
  {
    //Hiba: Az alapprogram nem IDLE allapotban van
    while(1);
  }
}

//A MOTOR modul kezelofuggvenye. Meg kell hivni minden alkalmazasprogramciklusban.
void MOTOR_Handler(void)
{
  //Az egyes funkciok kezelofuggvenyeinek meghivasa
  MOTOR_TestProcessStateMachineHandler();
  MOTOR_SpeedControlHandler();
  MOTOR_ScopeFunctionPeriodicSendingHandler();
}

//A tesztfolyamat kezelofuggvenye
void MOTOR_TestProcessStateMachineHandler(void)
{
  //A tesztfolyamat allapotgepenek megvalositasa
  //Keszenleti allapot
  if(E_TEST_STANDBY == eTestProcessState)
  {
    //Ha a nyomogombnak felfuto ele van
    if(TRUE == sGlobal.sLocalIo.bUserButtonRising)
    {
      //Akkor atlepunk a nyomogombvizsgalati allapotba
      eTestProcessState = E_TEST_BUTTON_WAS_PRESSED;
    }     
  }
  //A nyomogomb vizsgalata  
  else if(E_TEST_BUTTON_WAS_PRESSED == eTestProcessState)
  {
    //Ha a nyomogombnak lefuto ele van
    if(TRUE == sGlobal.sLocalIo.bUserButtonFalling)
    {
      //Kiküldjük a scope funkcio fejlecet
      MOTOR_ScopeFunctionSendHeadline();
      //Engedelyezzuk a periodikus adatkuldest
      MOTOR_ScopeFunctionStartPeriodicSending();
      //Engedelyezzuk a  szabalyozokat es atlepunk a kovetkezo allapotba
      bControllerEnable = TRUE;
      MCI_StartMotor(poMCI);
      eTestProcessState = E_TEST_CW;
    }     
    else
    {
      //Ha az ujrainditas alatt törteno egyedi USART funkcio bekapcsolasahoz nyomtak meg a gombot,
        //akkor nem fogunk lefuto elt kapni, hanem a kontrollert a reset gombbal ujrainditjak.
    }
  }
  //Forgas CW iranyba allapot
  else if(E_TEST_CW == eTestProcessState)
  {
    //Beallitjuk a celsebesseget, ha elertuk akkor atlepunk a kovetkezo allapotba
    i16SpeedSetValueRpm = MOTOR_TEST_PROCESS_ABS_TARGET_SPEED_RPM;
    if(TRUE == MOTOR_TargetSpeedWasReached())
    {
      eTestProcessState = E_TEST_CCW;
    }
  }
  //Forgas CCW iranyba allapot
  else if(E_TEST_CCW == eTestProcessState)
  {
    //Beallitjuk a celsebesseget, ha elertuk akkor atlepunk a kovetkezo allapotba
    i16SpeedSetValueRpm = -1 * MOTOR_TEST_PROCESS_ABS_TARGET_SPEED_RPM;    
    if(TRUE == MOTOR_TargetSpeedWasReached())
    {
      eTestProcessState = E_TEST_STOP;
    }
  }
  //Allo allapot
  else if(E_TEST_STOP == eTestProcessState)
  {
    //Beallitjuk a velsebesseget, ha elertuk akkor letiltjuk a szabalyozokat es
      //atlepunk a kovetkezo allapotba
    i16SpeedSetValueRpm = 0;
    if(TRUE == MOTOR_TargetSpeedWasReached())
    {
      //Letiltjuk a periodikus adatkuldest
      MOTOR_ScopeFunctionStopPeriodicSending();      
      //Letiltjuk a szabalyozokat es atlepunk a kovetkezo allapotba      
      bControllerEnable = FALSE;      
      MCI_StopMotor(poMCI);
      eTestProcessState = E_TEST_STANDBY;
    }    
  }
  //Hiba allapot es a nem definialt allapotok
  else
  {
    //Letiltjuk a periodikus adatkuldest
    MOTOR_ScopeFunctionStopPeriodicSending();     
    //Letiltjuk a szabalyozokat, ebbol az allapotbol nem lepunk ki tobbet
    bControllerEnable = FALSE;
    MCI_StopMotor(poMCI);
  }  
}

//A sebessegszabalyozas kezelofuggvenye
void MOTOR_SpeedControlHandler(void)
{
  //A fordulatszamszabalyozo futtatasa a MOTOR_SPEED_CONTROLLER_SAMPLING_PERIOD_MS gyakorisaggal
  if(TIME_DidTimeElapse(u64TimeOfTheLastControllerCallMs ,MOTOR_SPEED_CONTROLLER_SAMPLING_PERIOD_MS))
  {
    //Az aktualis mert sebessegertek kiolvasasa es atszamitasa RPM-be
    i16SpeedActualValueRpm = MOTOR_SPEED_TO_RPM_CONSTANT * MCI_GetAvrgMecSpeed01Hz(poMCI);
    //A szabalyozo meghivasa es az aram alapjel ertek tarolasa
    i16CurrentSetValue = PI_CONTROLLER_Handler(&sPIControllerObject, 
                                               bControllerEnable,
                                               i16SpeedSetValueRpm,
                                               i16SpeedActualValueRpm,
                                               f32KpSetting,
                                               f32TiSetting,
                                               i16AbsControllerOutputLimitSetValue);
    //Az aram alapjel tovabbitasa az aramszabalyozonak
    MOTOR_SetCurrentReference(i16CurrentSetValue);
    //Frissitjuk az utolso szabalyozohivas idejet a mostani idore
    u64TimeOfTheLastControllerCallMs = sGlobal.sTime.u64UpTimeMs;
  }  
}

//A Scope funkcio fejlecenek kikuldesehez hasznalt fuggveny
void MOTOR_ScopeFunctionSendHeadline(void)
{ 
  //Ha a Scope funkcio be van kapcsolva
  if(TRUE == sScopeData.bFunctionEnabled)
  {
    //Akkor kiküldjük a mert adatok feljecet
    UITask_SendBufferContentOnUsart((uint8_t *) sScopeData.tHeadlineText, sScopeData.u8HeadlineTextLength);    
    //Beallitjuk, hogy most törtent az utolso küldes, igy a periodikus kuldes idozitese mostantol indul majd
    sScopeData.u64msTimeOfTheLastTransmission = sGlobal.sTime.u64UpTimeMs;
  }
}

//A Scope funkcio periodikus kuldesenek elinditasahoz hasznalt fuggveny
void MOTOR_ScopeFunctionStartPeriodicSending(void)
{
  sScopeData.bPeriodicSendingIsOn = TRUE;
}

//A Scope funkcio periodikus kuldesenek leallitasahoz hasznalt fuggveny
void MOTOR_ScopeFunctionStopPeriodicSending(void)
{
  sScopeData.bPeriodicSendingIsOn = FALSE;
}

//A Scope funkcio periodikus kuldesenek kezelofuggvenye
void MOTOR_ScopeFunctionPeriodicSendingHandler(void)
{
  //A kuldeshez tartozo buffer. Az egyes hivasok kozott itt tarolodnak az adatok es az USART programresz innen vegzi az olvasast
  static U8 au8TransmitBuffer[SCOPE_FUNCTION_TRANSMIT_BUFFER_SIZE_IN_BYTES];
  //A buffer kezdetetol szamitva ennyi byte-ot tartalmaz az aktualis kuldes
  static U8 u8TransmitLength;
  
  //Ha a Scope funkcio be van kapcsolva  
  if(TRUE == sScopeData.bFunctionEnabled)
  {
    //Ha a periodikus kuldes be van kapcsolva
    if(TRUE == sScopeData.bPeriodicSendingIsOn)
    {
      //A periodikus kuldes futtatasa a SCOPE_FUNCTION_PERIODIC_TRANSMIT_PERIOD_MS gyakorisaggal
      if(TIME_DidTimeElapse(sScopeData.u64msTimeOfTheLastTransmission ,SCOPE_FUNCTION_PERIODIC_TRANSMIT_PERIOD_MS))
      {          
        //A kuldesre kerulo string osszeallitasa es a hosszanak mentese
        u8TransmitLength = sprintf((char *) au8TransmitBuffer, "%llu, %d, %d, %d, %d\n\r",  u64TimeOfTheLastControllerCallMs,
                                                                                            eTestProcessState,
                                                                                            i16SpeedSetValueRpm,
                                                                                            i16SpeedActualValueRpm,
                                                                                            i16CurrentSetValue);
        //Az sprintf funkcio mukodesenek ellenorzese
        if(0 < u8TransmitLength)
        {
          //Az osszeallitott string kuldese
          UITask_SendBufferContentOnUsart(au8TransmitBuffer, u8TransmitLength);    
          //Elmentjuk a mostani kuldes idejet
          sScopeData.u64msTimeOfTheLastTransmission = sGlobal.sTime.u64UpTimeMs;
        }
      }
    }
  }
}

//Az aram alapjel ertek beallitasat elvegzo fuggveny
void MOTOR_SetCurrentReference(I16 i16CurrentSetValue)
{
  Curr_Components sSetValueComponents = {i16CurrentSetValue, i16CurrentSetValue};
  
  MCI_SetCurrentReferences(poMCI, sSetValueComponents);
}

//A celsebesseg elereset detektalo fuggveny
bool MOTOR_TargetSpeedWasReached(void)
{
  bool bResult = FALSE;
  static U64 u64TheTargetSpeedWasReachedAtMs = 0;
  I16 i16AcceptanceLimitHigh = i16SpeedSetValueRpm + MOTOR_TARGET_SPEED_REACHED_ABS_LIMIT_RPM;
  I16 i16AcceptanceLimitLow  = i16SpeedSetValueRpm - MOTOR_TARGET_SPEED_REACHED_ABS_LIMIT_RPM;

  //Ha a kiszamitott savon belul van a mert sebessegertek
  if((i16AcceptanceLimitLow <= i16SpeedActualValueRpm) && (i16SpeedActualValueRpm <= i16AcceptanceLimitHigh))
  {
    //Es ez egy meghatarozott idon keresztul teljesul, akkor elertuk a celsebesseget
    bResult = TIME_DidTimeElapse(u64TheTargetSpeedWasReachedAtMs ,MOTOR_TARGET_SPEED_REACHED_TIME_LIMIT_MS);
  }
  else
  {
    //Kulonben az idozitot a mostani idopontra allitjuk be, 
      //es visszaadjuk, hogy nem ertuk el a celsebesseget
    u64TheTargetSpeedWasReachedAtMs = sGlobal.sTime.u64UpTimeMs;
  }
       
  return bResult;
}
