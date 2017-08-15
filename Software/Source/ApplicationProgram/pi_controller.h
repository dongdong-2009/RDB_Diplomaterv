#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H



typedef enum
{
  E_PI_CONTROLLER_IS_WITHIN_RANGE = 0,
  E_PI_CONTROLLER_IS_ON_HIGH_LIMIT,
  E_PI_CONTROLLER_IS_ON_LOW_LIMIT
} E_PI_CONTROLLER_STATUS;

typedef struct
{
  F32 f32SamplingPeriodS;  
  I32 i32PreviousErrorValue;
  I32 i32Integrator;
  E_PI_CONTROLLER_STATUS eStatus;  
} S_PI_CONTROLLER_DATA;



//A PI szabalyozo peldanyt inicializalo fuggveny.
  //Meg kell hivni az adott PI szabalyozo peldany elso PI_CONTROLLER_Handler
  //fuggvenyhivasa elott
void PI_CONTROLLER_Init(S_PI_CONTROLLER_DATA *psData,    //A PI szabalyozo peldany adatai
                        U32 u32SamplingPeriodMs);     //A kivant mintavetelezesi ido ms-ban

//Az adott PI szabalyozo peldanyt megvalosito fuggveny. Meg kell hivni az aditt
  //peldanynal beallitasra kerult mintavetelezesi idonek megfelelo periodusonkent
I16 PI_CONTROLLER_Handler(S_PI_CONTROLLER_DATA *psData,       //A PI szabalyozo peldany adatai
                          bool bEnable,                       //Engedelyezes
                          I16 i16SetValue,                    //Az alapjel
                          I16 i16FeedbackValue,               //A visszacsatolo jel
                          F32 f32Kp,                          //Az aranyos tag erositese
                          F32 f32Ti,                          //Az integralasi ido
                          I16 i16AbsControllerOutputLimit);   //A kimenet szimmetrikus +-limit erteke



#endif
