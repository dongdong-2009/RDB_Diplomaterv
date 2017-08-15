/* Pre-compiler coherency check */
#define PROJECT_CHK
#include "CrossCheck.h" 
#undef PROJECT_CHK

#include "MCTuningClass.h"
#include "MCInterfaceClass.h"

#include "MCTasks.h"
#include "Parameters conversion.h"

#include "Timebase.h"
#include "UITask.h"
#include "MCLibraryISRPriorityConf.h"

#include <stdio.h>



#include "ApplicationProgram/application.h"



void SysTick_Configuration(void);



//The version string of the firmware. 
#define FIRMWARE_VERS "STM32 FOC SDK\0Ver.4.3.0"
//The definition of the variable holding the version string. This will be sent to the monitor program.
const char s_fwVer[32] = FIRMWARE_VERS;  

//The definition of the MCInterface objects
CMCI oMCI[MC_NUM];
//The definition of the MCTuning objects
CMCT oMCT[MC_NUM];  

//The definition of the UI configuration array
uint32_t wConfig[MC_NUM] = {UI_CONFIG_M1,UI_CONFIG_M2};


//The main function of the firmware
int main(void)
{    
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */
  
  /*NVIC Priority group configuration.
    Default option is NVIC_PriorityGroup_3. 
  */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    
  /*MCInterface and MCTuning boot*/
  MCboot(oMCI,oMCT);

  /*Systick configuration.*/
  SysTick_Configuration();

  //Initializing the UI task
  UI_TaskInit(UI_INIT_CFG,wConfig,MC_NUM,oMCI,oMCT,s_fwVer); 
  
  //Initializing the Start/Stop button
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = START_STOP_GPIO_PIN;
  GPIO_Init(START_STOP_GPIO_PORT, &GPIO_InitStructure);
  
  //Initializing the application with the objects of the motor
  APPLICATION_Init((void *) oMCI[M1], (void *) oMCT[M1]);
  
  while(1)
  {        
    /* GUI, this section is present only if serial communication is enabled.*/
    if (UI_SerialCommunicationTimeOutHasElapsed())
    {
      // Send timeout message
      Exec_UI_IRQ_Handler(UI_IRQ_USART,3,0); // Flag 3 = Send timeout error*/
    }
    if (UI_SerialCommunicationATRTimeHasElapsed())
    {
      // Send ATR message
      Exec_UI_IRQ_Handler(UI_IRQ_USART,4,0); // Flag 4 = Send ATR message*/
    }

    //Periodically calling the application
    APPLICATION_Main();	 
  }
}

//Function to configure the SysTick
void SysTick_Configuration(void)
{
  /* Setup SysTick Timer for 500 usec interrupts  */
  if (SysTick_Config((SystemCoreClock) / SYS_TICK_FREQUENCY))
  { 
    /* Capture error */ 
    while (1);
  }
  
  //Setting up the priority of the SysTick interrupt
  NVIC_SetPriority(SysTick_IRQn, SYSTICK_PRIORITY);
  //Setting up the priority of the PendSV interrupt
  NVIC_SetPriority(PendSV_IRQn, PENDSV_PRIORITY);
}
