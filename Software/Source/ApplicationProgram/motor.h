#ifndef MOTOR_H
#define MOTOR_H



//A MOTOR modul inicializalasat vegzo fuggveny.
  //Meg kell hivni a MOTOR_Handler fuggveny elso hivasa elott
void MOTOR_Init(void *oMCI_Address,         //A motorhoz tartozo MCInterface objektum cime 
                void *oMCT_Address,         //A motorhoz tartozo MCTuning objektum cime 
                bool bEnableScopeFunction); //A scope funkcio engedelyezese
//A MOTOR modul kezelofuggvenye. Meg kell hivni minden alkalmazasprogramciklusban.
void MOTOR_Handler(void);



#endif
