#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include "controller_test_types.h"



#include "pi_controller.h"


static F32 Model(F32 f32HeatingInput);		


constexpr F32 f32AmbientTemperature				= 10;
constexpr U32 u32SamplingPeriodMs					= 100;
constexpr F32 f32SamplingPeriodS					= ((F32) u32SamplingPeriodMs) / 1000.0;
constexpr F32 f32HeatingInputCoefficient  = 0.1;	//It realizes the relation between the input heating and the temperature change of the model
constexpr F32 f32EnvironmentCoefficient   = 0.05;	//It realizes the relation between the effect of the ambient temperature and the temperature change of the model

I16  i16SetTemperatureValue		 = 4000;
I16  i16ControllerOutput			 = 0;
F32  f32ActualTemperatureValue = f32AmbientTemperature;
bool bEnableController				 = true;
I32  i16FeedbackValue;
F32  f32Kp = 2;
F32  f32Ti = 0;//0.2;

bool bExitProgram = false;

class Output
{
	std::ofstream file;	
	
 public:	
	std::stringstream data;
	
	Output() : data(std::ios_base::in | std::ios_base::out)
	{
		file.open("d:\\Programozas\\RDB_Diplomaterv\\Software\\NetbeansProject\\ControllerTest\\ControllerTest.txt", std::ofstream::trunc);		
	}
	~Output()
	{
		file.close();
	}
	
	void Print(void)
	{
		std::cout << data.str();
		file << data.str();
		file.flush();
		data.str(std::string());
	}	
};

int main(void)
{
	Output	myOutput;
		
	auto NextModelCallingTime = std::chrono::high_resolution_clock::now();
	
	PI_CONTROLLER_Init(u32SamplingPeriodMs);
	
	myOutput.data << "i32SetTemperatureValue, f32ActualTemperatureValue, f32AmbientTemperatureValue, i32ControllerOutput" << std::endl << std::flush;
	myOutput.Print();
	
	while(!bExitProgram)
	{    
		if( NextModelCallingTime <= std::chrono::high_resolution_clock::now())
		{					
      i16FeedbackValue = ((I32) f32ActualTemperatureValue);
		
			i16ControllerOutput = PI_CONTROLLER(bEnableController, i16SetTemperatureValue, i16FeedbackValue, f32Kp, f32Ti);
			f32ActualTemperatureValue = Model((F32) i16ControllerOutput);			
			
			NextModelCallingTime = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(u32SamplingPeriodMs);
						

			myOutput.data	<< i16SetTemperatureValue			<< ", "
										<< f32ActualTemperatureValue	<< ", "
										<< f32AmbientTemperature			<< ", "							
										<< i16ControllerOutput				<< ", "
										<< std::endl << std::flush;
			myOutput.Print();
		}
	}
	
	return 0;
}

static F32 Model(F32 f32HeatingInput)
{
	static F32 f32ModelTemperature = f32AmbientTemperature;
	
	f32ModelTemperature  += ((f32HeatingInputCoefficient * f32HeatingInput) / f32SamplingPeriodS);
	if(f32ModelTemperature < f32AmbientTemperature)
	{
		f32ModelTemperature += ((f32EnvironmentCoefficient * abs(f32ModelTemperature - f32AmbientTemperature)) / f32SamplingPeriodS);
	}
	else if(f32ModelTemperature > f32AmbientTemperature)
	{
		f32ModelTemperature -= ((f32EnvironmentCoefficient * abs(f32ModelTemperature - f32AmbientTemperature)) / f32SamplingPeriodS)	;	
	}
	else
	{
		//If the model temperature equals the ambient temperature then there is no effect on the model temperature from the environment
	}
	
	return f32ModelTemperature;
}
