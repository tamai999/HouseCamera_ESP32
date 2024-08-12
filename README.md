# HouseCamera_ESP32
[ハウス環境モニタサービス](https://vhousem.web.app)のカメラ画像送信サンプルコードです。<br>
ハウスや作物の画像をハウス環境モニタサービスに定期的に送信します。

## 1. セットアップ

### 1-1. 開発環境
- Arduino IDE(Version 2.3)

### 1-2. マイコン/カメラ
サンプルプログラムは次のハードウェア上で動作します。

| パーツ | 製品名 |
|:---|:---|
|カメラ内蔵マイコン| [M5 Stack UNIT-CAMS3](https://docs.m5stack.com/ja/unit/Unit-CamS3) |

### 1-3. IDEの設定
Arduino IDEで次の設定をしてください。

1. OAdditional boards manager URLsの設定
   1. Arduino IDEメニュー > Preferences を選択し、OAdditional boards manager URLs欄に `https://static-cdn.m5stack.com/resource/arduino/package_m5stack_index.json` を指定する。<br><br>
      <img width="753" src="https://github.com/user-attachments/assets/809d42c4-54f6-4075-82f8-2088997ad5f4">
1. マイコンボードの設定
    1. マイコン選択ボックスの `Select Board` から `M5UnitCAMS3` を選択する。
　　　<br><table><tr><td><img width="367" src="https://github.com/user-attachments/assets/0f2ad9e6-bcd1-4a79-a391-11065d9d2759"></td><td><img width="663" src="https://github.com/user-attachments/assets/88a90cb8-3843-47dc-b358-2bb427612e01"></td></tr></table>
1. PSRAMの設定
    1. Arduino IDEメニュー > Tools を選択し、PSRAMの設定を `OPI PSRAM` に変更する。
       <img width="548" src="https://github.com/user-attachments/assets/a42f9aaf-585c-4e21-9c30-8f371ae268d7">

### 1-4. WiFi-SSID、WiFiパスワード、計測器ID、送信パスコードを設定
サンプルコードの次の変数の値を、ハウス環境モニタアプリの計測器画面に表示される値に変更します。

```cpp
#define WIFI_SSID     ""  // Wi-Fi SSID。設定してください。
#define WIFI_PASSWORD ""   // Wi-Fi パスワード。設定してください。

#define DEVICE_ID "12345678"    // 計測器ID。ハウス環境モニタアプリの　設定画面の計測器IDを設定してください。
#define PASSCODE  "1234567890"  // 計測器パスコード。ハウス環境モニタアプリの　設定画面から上記の計測器を選択し表示される送信パスコードを設定してください。
```

計測器ID及び送信パスコードは[ハウス環境モニタアプリ](https://apps.apple.com/jp/app/id6444917835)の　設定　＞　計測器一覧　から計測器を選択することで確認できます。

## 2. 使い方

- USB-Cに電源供給することで、10分に1回の頻度でカメラ画像をハウス環境モニタサービスに送信します。
