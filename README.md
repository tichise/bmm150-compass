# m5stamp-s3-bmm150

## インストール方法

プライベートリポジトリ https://github.com/tichise/m5stamp-s3-bmm150 を使用するための platformio.ini ファイルは以下のように設定します。

platformio.ini ファイルの例
```
platform = espressif32
board = m5stack-core-esp32
framework = arduino
lib_deps =
    https://github.com/tichise/m5stamp-s3-bmm150.git
```

この設定では、以下の点を考慮しています：

- プラットフォーム: espressif32 を指定しています。これはESP32ベースのボードを想定しています。
- ボード: board = m5stack-core-esp32 と設定しています。これはM5StackのESP32ボードに対応していますが、お使いのボードに応じて変更してください。
- フレームワーク: Arduinoフレームワークを使用します。
- ライブラリ依存関係: プライベートリポジトリのURL https://github.com/tichise/m5stamp-s3-bmm150.git を直接指定しています。
- プライベートリポジトリを使用する場合、アクセス権限が必要です。PlatformIOはGitのクローンプロセスを使うので、あなたのマシンでそのリポジトリにアクセスできる権限（たとえばSSHキーなど）が設定されている必要があります。
