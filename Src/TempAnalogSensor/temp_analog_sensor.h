#ifndef TEMPANALOGSENSOR_TEMP_ANALOG_SENSOR_H
#define TEMPANALOGSENSOR_TEMP_ANALOG_SENSOR_H

#include <stdint.h>
#include <stdbool.h>

// PC0 (A5 on board) -> ADC1_IN1


void TempAnalogSens_initialize(void);

uint16_t TempAnalogSens_get_ADC_reading(void);

float TempAnalogSens_get_temperature(void);

float GetInternalTemp(void);
float GetInternalTemp2(void);


#endif /* TEMPANALOGSENSOR_TEMP_ANALOG_SENSOR_H */
