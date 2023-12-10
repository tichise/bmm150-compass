#include <Arduino.h>
#include "Preferences.h"
#include "math.h"
#include "M5_BMM150.h"
#include "M5_BMM150_DEFS.h"
#include <Wire.h>

// デフォルトピンから変更する場合
#define PIN_SDA 13
#define PIN_SCL 15

Preferences prefs;

struct bmm150_dev dev;
bmm150_mag_data mag_offset;
bmm150_mag_data mag_max;
bmm150_mag_data mag_min;

int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *read_data, uint16_t len)
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

int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
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

int8_t bmm150_initialization()
{
  int8_t rslt = BMM150_OK;

  dev.dev_id = 0x10;          // Device address setting.  设备地址设置
  dev.intf = BMM150_I2C_INTF; // SPI or I2C interface setup.  SPI或I2C接口设置
  dev.read = i2c_read;        // Read the bus pointer.  读总线指针
  dev.write = i2c_write;      // Write the bus pointer.  写总线指针
  dev.delay_ms = delay;

  // Set the maximum range range
  // 设置最大范围区间
  mag_max.x = -2000;
  mag_max.y = -2000;
  mag_max.z = -2000;

  // Set the minimum range
  // 设置最小范围区间
  mag_min.x = 2000;
  mag_min.y = 2000;
  mag_min.z = 2000;

  rslt = bmm150_init(&dev); // Memory chip ID.  存储芯片ID
  dev.settings.pwr_mode = BMM150_NORMAL_MODE;
  rslt |= bmm150_set_op_mode(&dev); // Set the sensor power mode.  设置传感器电源工作模式
  dev.settings.preset_mode = BMM150_PRESETMODE_ENHANCED;
  rslt |= bmm150_set_presetmode(&dev); // Set the preset mode of .  设置传感器的预置模式
  return rslt;
}

void bmm150_offset_save()
{ // Store the data.  存储bmm150的数据
  prefs.begin("bmm150", false);
  prefs.putBytes("offset", (uint8_t *)&mag_offset, sizeof(bmm150_mag_data));
  prefs.end();
}

void bmm150_offset_load()
{ // load the data.  加载bmm150的数据
  if (prefs.begin("bmm150", true))
  {
    prefs.getBytes("offset", (uint8_t *)&mag_offset, sizeof(bmm150_mag_data));
    prefs.end();
    USBSerial.println("bmm150 load offset finish....");
  }
  else
  {
    USBSerial.println("bmm150 load offset failed....");
  }
}

void bmm150_calibrate(uint32_t calibrate_time)
{ // bbm150 data calibrate.  bbm150数据校准
  uint32_t calibrate_timeout = 0;

  calibrate_timeout = millis() + calibrate_time;
  USBSerial.printf("Go calibrate, use %d ms \r\n", calibrate_time); // The serial port outputs formatting characters.  串口输出格式化字符
  USBSerial.printf("running ...");

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

  mag_offset.x = (mag_max.x + mag_min.x) / 2;
  mag_offset.y = (mag_max.y + mag_min.y) / 2;
  mag_offset.z = (mag_max.z + mag_min.z) / 2;
  bmm150_offset_save();

  USBSerial.printf("\n calibrate finish ... \r\n");
  USBSerial.printf("mag_max.x: %.2f x_min: %.2f \t", mag_max.x, mag_min.x);
  USBSerial.printf("y_max: %.2f y_min: %.2f \t", mag_max.y, mag_min.y);
  USBSerial.printf("z_max: %.2f z_min: %.2f \r\n", mag_max.z, mag_min.z);
}

void setup()
{
  Wire.begin(PIN_SDA, PIN_SCL); // SDAとSCLのピン番号と周波数を必要に応じて変更

  USBSerial.begin(115200); // シリアル通信の初期化

  USBSerial.println("BMM150 initialization...");

  if (bmm150_initialization() != BMM150_OK)
  {
    USBSerial.println("BMM150 initialization failed.");
    while (1)
    {
      USBSerial.println("BMM150 initialization failed.");
      delay(100); // エラーが発生した場合は無限ループ
    }
  }

  USBSerial.println("BMM150 initialization success.");

  bmm150_offset_load();
}

void loop()
{
  USBSerial.println("BMM150 read data...");

  static bool isCalibrated = false;

  if (!isCalibrated)
  {
    USBSerial.println("Starting calibration...");
    bmm150_calibrate(10000); // キャリブレーションを実行
    isCalibrated = true;
  }

  USBSerial.println("Reading data...");

  bmm150_read_mag_data(&dev);
  float head_dir = atan2(dev.data.x - mag_offset.x, dev.data.y - mag_offset.y) * 180.0 / M_PI;
  USBSerial.printf("Magnetometer data, heading %.2f\n", head_dir);
  USBSerial.printf("MAG X : %.2f \t MAG Y : %.2f \t MAG Z : %.2f \n", dev.data.x, dev.data.y, dev.data.z);
  USBSerial.printf("MID X : %.2f \t MID Y : %.2f \t MID Z : %.2f \n", mag_offset.x, mag_offset.y, mag_offset.z);

  delay(100); // ループの遅延
}