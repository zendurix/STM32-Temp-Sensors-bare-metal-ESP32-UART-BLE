
#ifndef TEMPDIGITALSENSOR_TEMP_DIGITAL_SENSOR_H_
#define TEMPDIGITALSENSOR_TEMP_DIGITAL_SENSOR_H_

#include <stdint.h>
#include <stdbool.h>

// temp sensor: DS18B20
// DQ <-> PA0 (A0 on board)


void TempDigitalSens_initialize(void);


float TempDigitalSens_get_temperature(uint8_t* byte);



#endif /* TEMPDIGITALSENSOR_TEMP_DIGITAL_SENSOR_H_ */
