#include "BMM150Compass.h"

// コンストラクタ
BMM150Compass::BMM150Compass(uint8_t sda, uint8_t scl) : _sda(sda), _scl(scl)
{
	Wire.begin(_sda, _scl);
}

// デストラクタ
BMM150Compass::~BMM150Compass()
{
	// 必要に応じてリソース解放などの処理をここに追加
}

// I2C読み込み関数
int8_t BMM150Compass::i2c_read_static(uint8_t dev_id, uint8_t reg_addr, uint8_t *read_data, uint16_t len)
{
	Wire.beginTransmission(dev_id);
	Wire.write(reg_addr);
	if (Wire.endTransmission() != 0)
	{
		return BMM150_E_DEV_NOT_FOUND;
	}

	Wire.requestFrom((int)dev_id, (int)len);
	uint8_t i = 0;
	while (Wire.available())
	{
		read_data[i++] = Wire.read();
	}

	return BMM150_OK;
}

// I2C書き込み関数
int8_t BMM150Compass::i2c_write_static(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	Wire.beginTransmission(dev_id);
	Wire.write(reg_addr);
	Wire.write(data, len);
	if (Wire.endTransmission() != 0)
	{
		return BMM150_E_DEV_NOT_FOUND;
	}

	return BMM150_OK;
}

// BMM150の初期化
int8_t BMM150Compass::initialize()
{
	dev.dev_id = 0x13;
	dev.intf = BMM150_I2C_INTF;

	// I2Cの読み書き関数を設定。BMM150Compassクラスのメンバ関数を指定
	dev.read = i2c_read_static;
	dev.write = i2c_write_static;
	dev.delay_ms = delay;

	mag_max.x = mag_max.y = mag_max.z = -2000;
	mag_min.x = mag_min.y = mag_min.z = 2000;

	int8_t rslt = bmm150_init(&dev);
	dev.settings.pwr_mode = BMM150_NORMAL_MODE;
	rslt |= bmm150_set_op_mode(&dev);
	dev.settings.preset_mode = BMM150_PRESETMODE_ENHANCED;
	rslt |= bmm150_set_presetmode(&dev);

	return rslt;
}

// オフセットの保存
void BMM150Compass::offset_save()
{
	prefs.begin("bmm150", false);
	prefs.putBytes("offset", (uint8_t *)&mag_offset, sizeof(bmm150_mag_data));
	prefs.end();
}

// オフセットの読み込み
void BMM150Compass::offset_load()
{
	if (prefs.begin("bmm150", true))
	{
		prefs.getBytes("offset", (uint8_t *)&mag_offset, sizeof(bmm150_mag_data));
		prefs.end();
	}
}

// キャリブレーション
void BMM150Compass::calibrate(uint32_t calibrate_time)
{
	uint32_t calibrate_timeout = millis() + calibrate_time;
	while (calibrate_timeout > millis())
	{
		bmm150_read_mag_data(&dev);
		if (dev.data.x)
		{
			mag_min.x = min(dev.data.x, mag_min.x);
			mag_max.x = max(dev.data.x, mag_max.x);
		}
		if (dev.data.y)
		{
			mag_min.y = min(dev.data.y, mag_min.y);
			mag_max.y = max(dev.data.y, mag_max.y);
		}
		if (dev.data.z)
		{
			mag_min.z = min(dev.data.z, mag_min.z);
			mag_max.z = max(dev.data.z, mag_max.z);
		}
		delay(100);
	}

	mag_offset.x = (mag_max.x + mag_min.x) / 2;
	mag_offset.y = (mag_max.y + mag_min.y) / 2;
	mag_offset.z = (mag_max.z + mag_min.z) / 2;
	offset_save();
}

// 方位角の読み取り
float BMM150Compass::readHeading()
{
	bmm150_read_mag_data(&dev);
	return atan2(dev.data.x - mag_offset.x, dev.data.y - mag_offset.y) * 180.0 / M_PI;
}