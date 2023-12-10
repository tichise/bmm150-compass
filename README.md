# bmm150-compass
このラッパークラスは、[m5stack/M5\_BMM150](https://github.com/m5stack/M5_BMM150)ライブラリを簡単に使用できるように設計されています。BMM150は、Boschのデジタル地磁気センサーで、M5Stackモジュールに対応しています。このラッパークラスは、地磁気センサーの初期化、データの読み取り、およびキャリブレーションプロセスを簡素化し、Arduinoスケッチでの使用を容易にします。

## 主な機能：
- 初期化と設定:I2C通信を介してBMM150センサーの初期化と設定を行います。
センサーの動作モードや設定をカスタマイズするオプションを提供します。
- データ読み取り:センサーからの地磁気データ（X、Y、Z軸）を読み取ります。読み取ったデータを利用して、方位角を計算します。BMM150磁気センサーモジュールは地磁気の強さと方向を検出し、そのデータを基に磁北の方向を算出します。したがって、このライブラリを使用すると、磁北の方向を知ることが可能です。
- キャリブレーション:センサーデータの精度を向上させるためのキャリブレーション機能を提供します。キャリブレーションデータの保存と読み込みをサポートします。

## インストール方法

プライベートリポジトリ https://github.com/tichise/bmm150-compass を使用するための platformio.ini ファイルは以下のように設定します。

platformio.ini ファイルの例
```
platform = espressif32
board = m5stack-core-esp32
framework = arduino
lib_deps =
    https://github.com/tichise/bmm150-compass.git
```

この設定では、以下の点を考慮しています：

- プラットフォーム: espressif32 を指定しています。これはESP32ベースのボードを想定しています。
- ボード: board = m5stack-core-esp32 と設定しています。これはM5StackのESP32ボードに対応していますが、お使いのボードに応じて変更してください。
- フレームワーク: Arduinoフレームワークを使用します。
- ライブラリ依存関係: URL https://github.com/tichise/bmm150-compass.git を直接指定しています。

## サンプル
`https://github.com/tichise/bmm150-compass/tree/main/examples/m5stamp-s3`を参考にしてください。
