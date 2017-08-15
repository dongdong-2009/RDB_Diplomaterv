#include "controller_test_types.h"

#include "pi_controller.h"



static I16 SaturateI32ToI16(I32 i32Input);
static I32 SaturateI64ToI32(I64 i64Input);
static I32 AdditionWithSaturationI32I32(I32 i32OperandA, I32 i32OperandB);
static I32 MultiplicationWithSaturationF32I32(F32 f32OperandA, I32 i32OperandB);
static I32 DivisionByTwoWithShiftingI32(I32 i32Operand);



typedef enum
{
  E_PI_CONTROLLER_IS_WITHIN_RANGE = 0,
  E_PI_CONTROLLER_IS_ON_HIGH_LIMIT,
  E_PI_CONTROLLER_IS_ON_LOW_LIMIT
} E_PI_CONTROLLER_STATUS;



static F32 PI_CONTROLLER_f32SamplingPeriodS;



void PI_CONTROLLER_Init(U32 u32SamplingPeriodMs)
{
  PI_CONTROLLER_f32SamplingPeriodS = (((F32) u32SamplingPeriodMs) / 1000);
}

I16 PI_CONTROLLER(bool bEnable, I16 i16SetValue, I16 i16FeedbackValue, F32 f32Kp, F32 f32Ti)
{
  I32 i32ErrorValue;
  static I32 i32PreviousErrorValue = 0;
  static I32 i32Integrator = 0;
  I32 i32IntegratorNewValue;
  I32 i32POutput;
  I32 i32IOutput;
  I32 i16ControllerOutput;
  static E_PI_CONTROLLER_STATUS eStatus = E_PI_CONTROLLER_IS_WITHIN_RANGE;
	F32 f32Temp;
	I32 i32Temp;
	
	
	
  //Is the controller enabled?
  if( TRUE == bEnable )
  {
    //Calculating the error value
    i32ErrorValue = i16SetValue - i16FeedbackValue;

    //Calculating the P part of the output
    i32POutput = MultiplicationWithSaturationF32I32(f32Kp, i32ErrorValue);		

    //Is the integrator turned on? (f32Ti == 0 means that the I part is OFF)
    if( 0 < f32Ti )
    {
      //Calculating the integration
      //Executing the following in separate steps: i32IntegratorNewValue = ( i32Integrator + ((i32ErrorValue + i32PreviousErrorValue) / 2 ));
			i32IntegratorNewValue = AdditionWithSaturationI32I32(i32ErrorValue, i32PreviousErrorValue);
			i32IntegratorNewValue = DivisionByTwoWithShiftingI32(i32IntegratorNewValue);
			i32IntegratorNewValue = AdditionWithSaturationI32I32(i32IntegratorNewValue, i32Integrator);

      //Integrator anti-windup handling
      if(( E_PI_CONTROLLER_IS_WITHIN_RANGE  == eStatus )                                                  ||
         (( E_PI_CONTROLLER_IS_ON_HIGH_LIMIT == eStatus ) && ( i32IntegratorNewValue < i32Integrator ))   ||
         (( E_PI_CONTROLLER_IS_ON_LOW_LIMIT  == eStatus ) && ( i32IntegratorNewValue > i32Integrator )))
      {
        //The integrator did not windup so we accept the new value
        i32Integrator = i32IntegratorNewValue;
      }			
			//Calculating the I part of the output
			//Executing the following in separate steps: i32IOutput = (( I32 ) (f32Kp * ( PI_CONTROLLER_f32SamplingPeriodS / f32Ti ) * ((F32) i32Integrator )));
			f32Temp = PI_CONTROLLER_f32SamplingPeriodS / f32Ti;
			f32Temp *= f32Kp;
			i32IOutput = MultiplicationWithSaturationF32I32(f32Temp, i32Integrator);			
    }
    else
    {
      //The controller works as a plain P controller, so the integrator value needs to be reseted
      i32Integrator = 0;
			//The integrator part of the output will be also reseted
			i32IOutput = 0;
    }

    //Calculating the final output
		//Executing the following in separate steps: i16ControllerOutput = ((I16) (i32POutput + i32IOutput));
    i32Temp = AdditionWithSaturationI32I32(i32POutput, i32IOutput);
		i16ControllerOutput = SaturateI32ToI16(i32Temp);

    //Saving the actual error value for the next cycle
    i32PreviousErrorValue = i32ErrorValue;   		
		
    //Limiting the output value and setting the status of the output, which is used for the integrator anti-windup handling
    if(( PI_CONTROLLER_OUTPUT_LIMIT_HIGH ) < i16ControllerOutput )
    {
      i16ControllerOutput = PI_CONTROLLER_OUTPUT_LIMIT_HIGH;
      eStatus = E_PI_CONTROLLER_IS_ON_HIGH_LIMIT;
    }
    else if(( PI_CONTROLLER_OUTPUT_LIMIT_LOW ) > i16ControllerOutput )
    {
      i16ControllerOutput = PI_CONTROLLER_OUTPUT_LIMIT_LOW;
      eStatus = E_PI_CONTROLLER_IS_ON_LOW_LIMIT;
    }
    else
    {    
      eStatus = E_PI_CONTROLLER_IS_WITHIN_RANGE;
    }     
  }
  else
  {
    //The controller is not enabled so the internal data has to be reseted
    i32PreviousErrorValue = 0;
    i32Integrator = 0;
    i32POutput = 0;
    i32IOutput = 0;
    i16ControllerOutput = 0;    
    eStatus = E_PI_CONTROLLER_IS_WITHIN_RANGE;
  }
  
  return i16ControllerOutput;
}

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
static I32 AdditionWithSaturationI32I32(I32 i32OperandA, I32 i32OperandB)
{
  I64 i64Temp;
  I32 i32Result;
  
  i64Temp = i32OperandA + i32OperandB;
  
	i32Result = SaturateI64ToI32(i64Temp);
	
	return i32Result;
}

static I32 MultiplicationWithSaturationF32I32(F32 f32OperandA, I32 i32OperandB)
{
  I64 i64Temp;
  I32 i32Result;	
	
	i64Temp = ((I64) (f32OperandA * ((F32) i32OperandB)));
	
	i32Result = SaturateI64ToI32(i64Temp);
	
	return i32Result;
}

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
