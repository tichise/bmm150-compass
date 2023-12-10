#ifndef BMM150Compass_h
#define BMM150Compass_h

#include <Arduino.h>
#include "Preferences.h"
#include "M5_BMM150.h"
#include "M5_BMM150_DEFS.h"
#include <Wire.h>

class BMM150Compass
{
public:
	BMM150Compass(uint8_t sda, uint8_t scl);
	~BMM150Compass();

	int8_t initialize();
	void calibrate(uint32_t calibrate_time);
	float readHeading();
	void offset_load();

private:
	Preferences prefs;
	bmm150_dev dev;
	bmm150_mag_data mag_offset;
	bmm150_mag_data mag_max;
	bmm150_mag_data mag_min;

	uint8_t _sda, _scl;

	int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *read_data, uint16_t len);
	int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
	void offset_save();
};

#endif