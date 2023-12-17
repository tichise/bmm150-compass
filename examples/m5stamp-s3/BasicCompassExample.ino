#include <Arduino.h>
// #include "BMM150Compass/BMM150Compass.h"
#include <BMM150Compass.h>

// SDAとSCLのピン番号を定義（あなたのハードウェア設定に合わせて変更してください）
#define PIN_SDA 13
#define PIN_SCL 15

// BMM150Compassのインスタンスを作成
BMM150Compass compass;

void setup()
{
  Wire.begin(PIN_SDA, PIN_SCL);

  delay(3000);
  USBSerial.println("Starting BMM150 Compass example...");

  // シリアル通信の初期化
  USBSerial.begin(115200);

  compass = BMM150Compass();

  // センサーの初期化
  if (compass.initialize() != BMM150_OK)
  {
    USBSerial.println("BMM150 initialization failed!");
    while (1)
      ; // 初期化に失敗した場合は無限ループ
  }

  USBSerial.println("BMM150 initialized successfully.");

  // オフセットの読み込み
  compass.offset_load();

  // キャリブレーションの実行（10秒間）
  USBSerial.println("Starting calibration...");
  compass.calibrate(10000);
  USBSerial.println("Calibration complete.");
}

void loop()
{
  int16_t raw_data[3];

  // 磁気センサーの値を取得するメソッド
  compass.getXYZ(raw_data);

  // 度数での方位角を取得するメソッド（0から360度の範囲）
  float headingDegrees360 = compass.getHeadingDegrees360();

  double headingDegrees360 = compass.getHeadingDegrees360();

  // Grove Degital Compass v2はv1と比べて180度ずれてるので、180度回転させる
  headingDegrees360 += 180; // 角度を180°時計回りに回転
  if (headingDegrees360 < 0)
  {
    headingDegrees360 += 360; // 負の値を修正
  }
  headingDegrees360 = fmod(headingDegrees360, 360); // 360°を超える値を修正

  // 結果をシリアルポートに出力
  USBSerial.print("Heading: ");
  USBSerial.print(headingDegrees360);
  USBSerial.println(" degrees");

  // ループの遅延（1秒ごとに更新）
  delay(1000);
}
