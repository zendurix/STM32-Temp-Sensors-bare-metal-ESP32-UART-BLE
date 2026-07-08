#include "temp_analog_sensor.h"

#include <math.h>

#include "stm32u031xx.h"
#include "../clock.h"

// PC0 (A5 on board) -> ADC1_IN1

void TempAnalogSens_initialize(void) {

	// PC0

	// enable clock on GPIO C
	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
	// enable ADC clock
	RCC->APBENR2 |= RCC_APBENR2_ADCEN;

	// Set MODER to 0b11 - ANALOG mode.
	GPIOC->MODER &= ~(GPIO_MODER_MODE0);
	GPIOC->MODER |= (0b11 << GPIO_MODER_MODE0_Pos);

	// PUPDR 0b00 no pull up no pull down
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0);

	// should wait 10 micro sec
	Delay(10);

	// 1. disable ADC for configuration
	if (ADC1->CR & ADC_CR_ADEN) {
		ADC1->CR |= ADC_CR_ADDIS;
		while (ADC1->CR & ADC_CR_ADEN) {
			;
		}
	}

	// 2. ADC voltage regulator ADVREGEN enable - 1
	ADC1->CR |= ADC_CR_ADVREGEN;

	// 3. Software calibration procedure
	ADC1->CFGR1 &= ~(ADC_CFGR1_AUTOFF);
	ADC1->CFGR1 &= ~(ADC_CFGR1_DMAEN);

	// rm suggest calibrating 8 times and getting average from it

	uint16_t calibration_factors_sum = 0;
	for (int i = 0; i < 8; i++) {
		ADC1->CR |= ADC_CR_ADCAL;
		while (ADC1->CR & ADC_CR_ADCAL) {
			;
		}
		uint16_t calibration_factor = (ADC1->CALFACT & ADC_CALFACT_CALFACT_Msk)
				>> ADC_CALFACT_CALFACT_Pos;
		calibration_factor++;
		calibration_factors_sum += calibration_factor;
	}
	uint16_t calibration_factor_average = calibration_factors_sum / 8U;
	uint8_t calibration_factor =
			(calibration_factor_average > 0x7F) ?
					0x7F : calibration_factor_average;
	ADC1->CALFACT = (calibration_factor << ADC_CALFACT_CALFACT_Pos)
			& ADC_CALFACT_CALFACT_Msk;

//	ADC1->CR |= ADC_CR_ADCAL;
	//while (ADC1->CR & ADC_CR_ADCAL)
	//	;

	// 4. Enable ADC // use ADDIS to disable
	ADC1->CR |= ADC_CR_ADEN;
	while (ADC1->ISR & ADC_ISR_ADRDY) {
		;
	}

	// 5. Sampling config  0 - use SMP1 for channel 1
	ADC1->SMPR &= ~(ADC_SMPR_SMPSEL1);
	ADC1->SMPR &= ~(ADC_SMPR_SMP1);
	// 0b110 - 79.5 ADC clock cycles
	ADC1->SMPR |= (0b111 << ADC_SMPR_SMP1_Pos);

	// 6. Select input channel - 0 and turn off all other channels (for now)
	ADC1->CHSELR = ADC_CHSELR_CHSEL0;

	// 7. turn on ADC
	ADC1->CR |= ADC_CR_ADEN;
}

uint16_t TempAnalogSens_get_ADC_reading(void) {

	// start conversion
	ADC1->CR |= ADC_CR_ADSTART;

	// wait for conversion end
	while (!(ADC1->ISR & ADC_ISR_EOC)) {
		;
	}
	return ADC1->DR;
}

float TempAnalogSens_get_temperature(void) {
	uint16_t adc_value = TempAnalogSens_get_ADC_reading();

#define VCC 3.3f
#define R_FIXED 10000.0f
#define R0 10000.0f
#define T0 298.15f
#define BETA 3950.0f

	float Vadc = ((float) adc_value * 3.3f) / 4095.0f;
	float Rntc = R_FIXED * (Vadc / (VCC - Vadc));
	float tempK = 1.0f / ((1.0f / T0) + (1.0f / BETA) * logf(R0 / Rntc));

	// 4. Kelvin → Celsius
	float tempC = tempK - 273.15f;

	return (tempC);
}
