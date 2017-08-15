#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H



#define PI_CONTROLLER_OUTPUT_LIMIT_HIGH (( I16 )  5000 )
#define PI_CONTROLLER_OUTPUT_LIMIT_LOW  (( I16 ) -5000 )



void PI_CONTROLLER_Init(U32 u32SamplingPeriodMs);
I16 PI_CONTROLLER(bool bEnable, I16 i16SetValue, I16 i16FeedbackValue, F32 f32Kp, F32 f32Ti);
        
        
        
#endif
        