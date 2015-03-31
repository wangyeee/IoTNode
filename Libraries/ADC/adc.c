#include "adc.h"

void analog_init(ADC_CHL adc) {
    ADC_InitTypeDef ADC_InitStructure;

    ADC_DeInit(adc.ADCx);
    RCC_APB2PeriphClockCmd(adc.ADC_Clock, ENABLE);
    digital_init(adc.pin);

    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);

    /* Configure the ADC in continous mode withe a resolutuion equal to 12 bits */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
    ADC_Init(adc.ADCx, &ADC_InitStructure);

    ADC_ChannelConfig(adc.ADCx, ADC_Channel_4 , ADC_SampleTime_55_5Cycles);
    ADC_GetCalibrationFactor(adc.ADCx);

    /* Enable ADC */
    ADC_Cmd(adc.ADCx, ENABLE);

    while(ADC_GetFlagStatus(adc.ADCx, ADC_FLAG_ADEN) == RESET);
    ADC_StartOfConversion(adc.ADCx);
}

uint16_t analog_read(ADC_CHL adc) {
    //ADC_RegularChannelConfig(adc.ADCx, channel, 1, ADC_SampleTime_1_5Cycles);

    // Start the conversion
    // ADC_SoftwareStartConvCmd(adc.ADCx, ENABLE);
    //ADC_StartOfConversion(adc.ADCx);
    // Wait until conversion completion
    while(ADC_GetFlagStatus(adc.ADCx, ADC_FLAG_EOC) == RESET);
    // Get the conversion value
    return ADC_GetConversionValue(adc.ADCx);
}
