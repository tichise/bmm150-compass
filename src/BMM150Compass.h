#ifndef BMM150Compass_h
#define BMM150Compass_h

#include <Arduino.h>
#include "Preferences.h"
#include "M5_BMM150.h"
#include "M5_BMM150_DEFS.h"
#include <Wire.h>

// このラッパーは、m5stack/M5_BMM150ライブラリを簡単に使用できるように設計されています。
// BMM150は、Boschのデジタル地磁気センサーで、M5Stackモジュールに対応しています。
// このラッパークラスは、地磁気センサーの初期化、データの読み取り、およびキャリブレーションプロセスを簡素化し、Arduinoスケッチでの使用を容易にします。
class BMM150Compass
{
public:
	BMM150Compass();
	~BMM150Compass();

	int8_t initialize();
	void calibrate(uint32_t calibrate_time);
	void offset_load();
	void offset_save();

	void getXYZ(int16_t output[3]);
	double getHeadingRadians();
	double getHeadingDegrees180();
	double getHeadingDegrees360();

	static int8_t i2c_read_static(uint8_t dev_id, uint8_t reg_addr, uint8_t *read_data, uint16_t len);
	static int8_t i2c_write_static(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);

private:
	Preferences prefs;
	bmm150_dev dev;
	bmm150_mag_data mag_offset;
	bmm150_mag_data mag_max;
	bmm150_mag_data mag_min;

	uint8_t _sda, _scl;
};

#endif
