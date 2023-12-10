#include <Arduino.h>
#include "BMM150Compass.h"

// SDAとSCLのピン番号を定義（あなたのハードウェア設定に合わせて変更してください）
#define PIN_SDA 13
#define PIN_SCL 15

// BMM150Compassのインスタンスを作成
BMM150Compass compass(PIN_SDA, PIN_SCL);

void setup()
{
  // シリアル通信の初期化
  Serial.begin(115200);
  while (!Serial)
    ; // シリアルポートが利用可能になるまで待機

  // センサーの初期化
  if (compass.initialize() != BMM150_OK)
  {
    Serial.println("BMM150 initialization failed!");
    while (1)
      ; // 初期化に失敗した場合は無限ループ
  }

  Serial.println("BMM150 initialized successfully.");

  // オフセットの読み込み
  compass.loadOffset();

  // キャリブレーションの実行（10秒間）
  Serial.println("Starting calibration...");
  compass.calibrate(10000);
  Serial.println("Calibration complete.");
}

void loop()
{
  // 磁場データを読み取り、方位角を計算
  float heading = compass.readHeading();

  // 結果をシリアルポートに出力
  Serial.print("Heading: ");
  Serial.print(heading);
  Serial.println(" degrees");

  // ループの遅延（1秒ごとに更新）
  delay(1000);
}