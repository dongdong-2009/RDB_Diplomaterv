#include "stdio.h"
#include "string.h"

#include "UITask.h"

#include "ApplicationProgram\globals.h"

#include "ApplicationProgram\motor.h"



void APPLICATION_SendInitText(void);



//A globalis adatokat tartalmazo struktura definicioja
S_GLOBAL_DATA sGlobal;
//A program inditasakor az USART-on kikuldott szoveg
static TEXT   tInitText = "\n\r\n\r\n\r\n\r.:/ Erdos Gabor Diplomaterv, Gamma Demo \\:.\n\r\n\r\n\r\n\r";
        
        
        
//Az alkalmazasprogram inicializalasa. Meg kell hivni az elso APPLICATION_Main hivas elott
void APPLICATION_Init(void *oMCI_Address,         //Az MCInterface objektum cime 
                      void *oMCT_Address)         //Az MCTuning objektum cimer
{
  bool bCustomUSARTModeRequested;
  
  //Az alkalmazas tipusdefinicioinak ellenorzese
  APPLICATION_TYPES_CHECK();                      
    
  //A globalis adatok torlese
  memset(&sGlobal, 0x00, sizeof(S_GLOBAL_DATA));  
  
  //Az almodulok inicializalasa   
  LOCAL_IO_Init();      
  //Az egyedi USART modot a programindulaskori UserButton nyomvatartassal tudjuk kezdemenyezni
  bCustomUSARTModeRequested = sGlobal.sLocalIo.bUserButtonRaw;
  //Ha az egyedi USART modban vagyunk
  if(TRUE == bCustomUSARTModeRequested)
  {
    //Az udvozloszoveg kikuldese az USAR-on
    APPLICATION_SendInitText();
  }
  MOTOR_Init(oMCI_Address, oMCT_Address, bCustomUSARTModeRequested);
}

//Az alkalmazas main fuggvenye. Meg kell hivni a firmware minden programciklusaban
void APPLICATION_Main(void)
{
  //Az almodulok futtatasa
  TIME_Handler();
  LOCAL_IO_Handler();
  MOTOR_Handler();
}

//Az alkalmazasprogram SysTick kezelo fuggvenye. Meg kell hivni minden SysTick ISR-ben
void APPLICATION_SystickHandler(void)
{
  //A TIME modul SysTick kezelojenek meghivasa
  TIME_SystickHandler();
}

//A fuggveny meghivasakor kikuldi az udvozloszoveget az USART-on
void APPLICATION_SendInitText(void)
{ 
  U8 u8InitTextLength;   
  
  u8InitTextLength = strlen(tInitText);
  UITask_SendBufferContentOnUsart((uint8_t *) tInitText, u8InitTextLength);    
}
