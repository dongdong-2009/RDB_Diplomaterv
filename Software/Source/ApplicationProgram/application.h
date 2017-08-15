#ifndef APPLICATION_H
#define APPLICATION_H



//Az alkalmazasprogram inicializalasa. Meg kell hivni az elso APPLICATION_Main hivas elott
void APPLICATION_Init(void *oMCI_Address,         //Az MCInterface objektum cime 
                      void *oMCT_Address);        //Az MCTuning objektum cimer
//Az alkalmazas main fuggvenye. Meg kell hivni a firmware minden programciklusaban
void APPLICATION_Main(void);
//Az alkalmazasprogram SysTick kezelo fuggvenye. Meg kell hivni minden SysTick ISR-ben
void APPLICATION_SystickHandler(void);



#endif
