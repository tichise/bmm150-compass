#include "BMM150Compass.h"

// コンストラクタ
BMM150Compass::BMM150Compass()
{
}

// デストラクタ
BMM150Compass::~BMM150Compass()
{
	// 必要に応じてリソース解放などの処理をここに追加
}

// I2C読み込み関数
int8_t BMM150Compass::i2c_read_static(uint8_t dev_id, uint8_t reg_addr, uint8_t *read_data, uint16_t len)
{
	// データを読み込む
	Wire.beginTransmission(dev_id);

	// レジスタアドレスを送信
	Wire.write(reg_addr);

	// 送信結果を返す
	if (Wire.endTransmission() != 0)
	{
		// 送信に失敗した場合はエラーを返す
		return BMM150_E_DEV_NOT_FOUND;
	}

	// データを読み込む
	Wire.requestFrom((int)dev_id, (int)len);
	uint8_t i = 0;

	while (Wire.available())
	{
		read_data[i++] = Wire.read();
	}

	// 送信に成功した場合は成功を返す
	return BMM150_OK;
}

// I2C書き込み関数
int8_t BMM150Compass::i2c_write_static(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	// データを書き込む
	Wire.beginTransmission(dev_id);
	Wire.write(reg_addr);
	Wire.write(data, len);

	// 送信結果を返す
	if (Wire.endTransmission() != 0)
	{
		// 送信に失敗した場合はエラーを返す
		return BMM150_E_DEV_NOT_FOUND;
	}

	// 送信に成功した場合は成功を返す
	return BMM150_OK;
}

// BMM150の初期化
int8_t BMM150Compass::initialize()
{
	// BMM150センサの初期化
	int8_t rslt = BMM150_OK;

	// BMM150センサのI2Cアドレスを設定
	// https://wiki.seeedstudio.com/Grove-3-Axis_Digitial_Compass_v2.0/
	dev.dev_id = 0x13;

	// インターフェイスとしてI2Cを使用する設定
	dev.intf = BMM150_I2C_INTF;

	// I2C通信でデータを読み込むための関数を指定
	dev.read = i2c_read_static;

	// I2C通信でデータを書き込むための関数を指定
	dev.write = i2c_write_static;

	// センサとの通信に使用する遅延時間を設定するための関数を指定
	dev.delay_ms = delay;

	// センサが測定できる磁場の最大値を設定
	mag_max.x = -2000;
	mag_max.y = -2000;
	mag_max.z = -2000;

	// センサが測定できる磁場の最小値を設定
	mag_min.x = 2000;
	mag_min.y = 2000;
	mag_min.z = 2000;

	// BMM150センサの初期化を行い、チップIDをメモリに保存
	rslt = bmm150_init(&dev);

	// センサの電源モードを通常モードに設定
	dev.settings.pwr_mode = BMM150_NORMAL_MODE;
	rslt |= bmm150_set_op_mode(&dev);

	// センサのプリセットモードを拡張モードに設定
	dev.settings.preset_mode = BMM150_PRESETMODE_ENHANCED;
	rslt |= bmm150_set_presetmode(&dev);

	// rsltの結果をUSBシリアルに出力
	USBSerial.printf("bmm150_init result: %d\n", rslt);

	// 初期化の結果を返す（BMM150_OKは成功、それ以外はエラー）
	return rslt;
}

// オフセットの保存
void BMM150Compass::offset_save()
{
	prefs.begin("bmm150", false);

	// オフセットの保存に成功した場合は、mag_offsetを保存
	prefs.putBytes("offset", (uint8_t *)&mag_offset, sizeof(bmm150_mag_data));
	prefs.end();
}

// オフセットの読み込み
void BMM150Compass::offset_load()
{
	// オフセットの読み込み
	if (prefs.begin("bmm150", true))
	{
		// オフセットの読み込みに成功した場合は、mag_offsetに読み込んだ値を設定
		prefs.getBytes("offset", (uint8_t *)&mag_offset, sizeof(bmm150_mag_data));
		prefs.end();
	}
}

// キャリブレーション
void BMM150Compass::calibrate(uint32_t calibrate_time)
{
	// BMM150のデータをキャリブレーションする
	uint32_t calibrate_timeout = 0;

	// キャリブレーションのタイムアウト時間を設定
	calibrate_timeout = millis() + calibrate_time;

	// キャリブレーションの開始をUSBシリアルに出力
	USBSerial.printf("Go calibrate, use %d ms \r\n", calibrate_time);
	USBSerial.printf("running ...");

	// キャリブレーションの開始時間からタイムアウト時間まで繰り返す
	while (calibrate_timeout > millis())
	{
		bmm150_read_mag_data(&dev); // read the magnetometer data from registers.  从寄存器读取磁力计数据
		if (dev.data.x)
		{
			mag_min.x = (dev.data.x < mag_min.x) ? dev.data.x : mag_min.x;
			mag_max.x = (dev.data.x > mag_max.x) ? dev.data.x : mag_max.x;
		}
		if (dev.data.y)
		{
			mag_max.y = (dev.data.y > mag_max.y) ? dev.data.y : mag_max.y;
			mag_min.y = (dev.data.y < mag_min.y) ? dev.data.y : mag_min.y;
		}
		if (dev.data.z)
		{
			mag_min.z = (dev.data.z < mag_min.z) ? dev.data.z : mag_min.z;
			mag_max.z = (dev.data.z > mag_max.z) ? dev.data.z : mag_max.z;
		}
		delay(100);
	}

	// mag_offsetを計算。mag_maxとmag_minの中間値を使用
	mag_offset.x = (mag_max.x + mag_min.x) / 2;
	mag_offset.y = (mag_max.y + mag_min.y) / 2;
	mag_offset.z = (mag_max.z + mag_min.z) / 2;

	// オフセットの保存
	offset_save();

	// キャリブレーションの終了をUSBシリアルに出力
	USBSerial.printf("\n calibrate finish ... \r\n");
	USBSerial.printf("mag_max.x: %.2f x_min: %.2f \t", mag_max.x, mag_min.x);
	USBSerial.printf("y_max: %.2f y_min: %.2f \t", mag_max.y, mag_min.y);
	USBSerial.printf("z_max: %.2f z_min: %.2f \r\n", mag_max.z, mag_min.z);
}

// XYZの値を取得
void BMM150Compass::getXYZ(int16_t output[3])
{
	bmm150_read_mag_data(&dev);
	output[0] = dev.data.x - mag_offset.x;
	output[1] = dev.data.y - mag_offset.y;
	output[2] = dev.data.z - mag_offset.z;
}

// 方位角をラジアンで取得
double BMM150Compass::getHeadingRadians()
{
	bmm150_read_mag_data(&dev);
	int16_t output[3];
	output[0] = dev.data.x - mag_offset.x;
	output[1] = dev.data.y - mag_offset.y;
	output[2] = dev.data.z - mag_offset.z;

	return atan2(output[1], output[0]);
}

// 方位角を度数で取得（-180から180度の範囲）
double BMM150Compass::getHeadingDegrees180()
{
	double headingRadians = getHeadingRadians();
	double headingDegrees = headingRadians * RAD_TO_DEG;

	if (headingDegrees > 180)
	{
		headingDegrees -= 360;
	}
	else if (headingDegrees < -180)
	{
		headingDegrees += 360;
	}

	return headingDegrees;
}

// 方位角を度数で取得（0から360度の範囲）
double BMM150Compass::getHeadingDegrees360()
{
	double headingDegrees = getHeadingDegrees180();
	if (headingDegrees < 0)
	{
		headingDegrees += 360;
	}

	return headingDegrees;
}