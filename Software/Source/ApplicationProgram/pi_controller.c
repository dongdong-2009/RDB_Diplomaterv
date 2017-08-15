#include "ApplicationProgram\types.h"

#include "ApplicationProgram\pi_controller.h"



static I16 SaturateI32ToI16(I32 i32Input);
static I32 SaturateI64ToI32(I64 i64Input);
static I32 AdditionWithSaturationI32I32(I32 i32OperandA, I32 i32OperandB);
static I32 MultiplicationWithSaturationF32I32(F32 f32OperandA, I32 i32OperandB);
static I32 DivisionByTwoWithShiftingI32(I32 i32Operand);



//A PI szabalyozot inicializalo fuggveny
void PI_CONTROLLER_Init(S_PI_CONTROLLER_DATA *psData,  //A PI szabalyozo peldany adatai
                        U32 u32SamplingPeriodMs)       //A kivant mintavetelezesi ido ms-ban
{
  //Ellenorizzuk, hogy megfelelo-e a kapott mintavetelezesi ido erteke
  if(0 < u32SamplingPeriodMs)
  {
    //A szabalyzo peldany inicializalasa
    psData->f32SamplingPeriodS = (((F32) u32SamplingPeriodMs) / 1000);
    psData->i32PreviousErrorValue = 0;
    psData->i32Integrator = 0;
    psData->eStatus = E_PI_CONTROLLER_IS_WITHIN_RANGE;    
  }
  else
  {
    //Hiba, nem megfelelo u32SamplingPeriodMs beallitas.
    while(1);
  }
}

//A PI szabalyozo funkciot megvalosito fuggveny. Meg kell hivni a beallitasra
  //kerult mintavetelezesi idonek megfelelo idokozonkent
I16 PI_CONTROLLER_Handler(S_PI_CONTROLLER_DATA *psData,       //A PI szabalyozo peldany adatai
                          bool bEnable,                       //Engedelyezes
                          I16 i16SetValue,                    //Az alapjel
                          I16 i16FeedbackValue,               //A visszacsatolo jel
                          F32 f32Kp,                          //Az aranyos tag erositese
                          F32 f32Ti,                          //Az integralasi ido
                          I16 i16AbsControllerOutputLimit)    //A kimenet szimmetrikus +-limit erteke
{
  I32 i32ErrorValue;
  I32 i32IntegratorNewValue;
  I32 i32POutput;
  I32 i32IOutput;
  I32 i16ControllerOutput;
  F32 f32Temp;
  I32 i32Temp;
	
	
	
  //Engedelyezve van a szabalyozo?
  if(TRUE == bEnable)
  {
    //A hibajel kiszamitasa
    i32ErrorValue = i16SetValue - i16FeedbackValue;

    //A kimenet P osszetevojenek kiszamitasa
    i32POutput = MultiplicationWithSaturationF32I32(f32Kp, i32ErrorValue);		

    //Be van kapcsolva az integralas? (f32Ti == 0 ertekkel az integralas kikapcsolt)
    if(0 < f32Ti)
    {
      //Az integralas elvegzese
      //A muvelet osszevonva: i32IntegratorNewValue = ( i32Integrator + ((i32ErrorValue + psData->i32PreviousErrorValue) / 2 ));
      i32IntegratorNewValue = AdditionWithSaturationI32I32(i32ErrorValue, psData->i32PreviousErrorValue);
      i32IntegratorNewValue = DivisionByTwoWithShiftingI32(i32IntegratorNewValue);
      i32IntegratorNewValue = AdditionWithSaturationI32I32(i32IntegratorNewValue, psData->i32Integrator);

      //Az integrator anti-windup funkcio megvalositasa
      if(( E_PI_CONTROLLER_IS_WITHIN_RANGE   == psData->eStatus )                                                        ||
         (( E_PI_CONTROLLER_IS_ON_HIGH_LIMIT == psData->eStatus ) && ( i32IntegratorNewValue < psData->i32Integrator ))  ||
         (( E_PI_CONTROLLER_IS_ON_LOW_LIMIT  == psData->eStatus ) && ( i32IntegratorNewValue > psData->i32Integrator )))
      {
        //Az integrator nincs wind-up allapotban ezert az uj erteket elfogadjuk
        psData->i32Integrator = i32IntegratorNewValue;
      }			
      //A kimenet I osszetevojenek kiszamitasa
      //A muvelet osszevonva:  i32IOutput = (( I32 ) (f32Kp * ( PI_CONTROLLER_f32SamplingPeriodS / f32Ti ) * ((F32) i32Integrator )));
      f32Temp = psData->f32SamplingPeriodS / f32Ti;
      f32Temp *= f32Kp;
      i32IOutput = MultiplicationWithSaturationF32I32(f32Temp, psData->i32Integrator);			
    }
    else
    {
      //A szabalyozo sima P uzemmodban mukodik, ezert az integratort alaphelyzetbe allitjuk
      psData->i32Integrator = 0;
      //A kimenet I osszetevojet szinten alaphelyzetbe allitjuk
      i32IOutput = 0;
    }

    //A kimeneti ertek kiszamitasa
    //A muvelet osszevonva: i16ControllerOutput = ((I16) (i32POutput + i32IOutput));
    i32Temp = AdditionWithSaturationI32I32(i32POutput, i32IOutput);
    i16ControllerOutput = SaturateI32ToI16(i32Temp);

    //Elmentjuk a hibajel erteket a kovetkezo ciklusra
    psData->i32PreviousErrorValue = i32ErrorValue;   		
		
    //A kimenet korlatozasa es az adatok elmentesi az anti wind-up funkcio megvalositasahoz
    if(( i16AbsControllerOutputLimit ) < i16ControllerOutput )
    {
      i16ControllerOutput = i16AbsControllerOutputLimit;
      psData->eStatus = E_PI_CONTROLLER_IS_ON_HIGH_LIMIT;
    }
    else if(( -1 * i16AbsControllerOutputLimit ) > i16ControllerOutput )
    {
      i16ControllerOutput = -1 * i16AbsControllerOutputLimit;
      psData->eStatus = E_PI_CONTROLLER_IS_ON_LOW_LIMIT;
    }
    else
    {    
      psData->eStatus = E_PI_CONTROLLER_IS_WITHIN_RANGE;
    }     
  }
  else
  {
    //A szabalyozo tiltott allapotban van, igy az adatokat torolnunk kell
    psData->eStatus = E_PI_CONTROLLER_IS_WITHIN_RANGE;    
    psData->i32PreviousErrorValue = 0;
    psData->i32Integrator = 0;
    i32POutput = 0;
    i32IOutput = 0;
    i16ControllerOutput = 0;    
  }
  
  return i16ControllerOutput;
}

//Segeduggveny, ami egy I32 valtozot I16 ertekre korlatoz
static I16 SaturateI32ToI16(I32 i32Input)
{
  I16 i16Result;

  if((I16MAX) <= i32Input)
  {
    i16Result = I16MAX;
  }
  else if(i32Input <= (I16MIN))
  {
    i16Result = I16MIN;
  }
  else
  {
    i16Result = ((I16) i32Input);
  }	
	
  return i16Result;
}

//Segeduggveny, ami egy I64 valtozot I32 ertekre korlatoz
static I32 SaturateI64ToI32(I64 i64Input)
{
  I32 i32Result;
	
  if((I32MAX) <= i64Input)
  {
    i32Result = I32MAX;
  }
  else if(i64Input <= (I32MIN))
  {
    i32Result = I32MIN;
  }
  else
  {
    i32Result = ((I32) i64Input);
  }	
	
  return i32Result;
}

//Segeduggveny, ami osszead ket I32 valtozot es az eredmenyt I32 ertekre korlatozza
static I32 AdditionWithSaturationI32I32(I32 i32OperandA, I32 i32OperandB)
{
  I64 i64Temp;
  I32 i32Result;
  
  i64Temp = i32OperandA + i32OperandB;
  
  i32Result = SaturateI64ToI32(i64Temp);

  return i32Result;
}

//Segeduggveny, ami osszeszoroz egy F32 es egy I32 valtozot es az eredmenyt I32 ertekre korlatozza
static I32 MultiplicationWithSaturationF32I32(F32 f32OperandA, I32 i32OperandB)
{
  I64 i64Temp;
  I32 i32Result;	
	
  i64Temp = ((I64) (f32OperandA * ((F32) i32OperandB)));

  i32Result = SaturateI64ToI32(i64Temp);

  return i32Result;
}


//Segeduggveny, ami shiftelessel eloszt egy I32-t kettovel.
static I32 DivisionByTwoWithShiftingI32(I32 i32Operand)
{
  I32 i32Result;

  if(i32Operand < 0)
  {
    i32Result = 0 - i32Operand;
    i32Result >>= 1;
    i32Result = 0 - i32Result;
  }
  else
  {
    i32Result = i32Operand >> 1;		
  }

  return i32Result;
}
