#include <WiFi.h>               // WiFi接続用
#include <WiFiClientSecure.h>   // HTTPS用
#include "base64.hpp"           // Basic認証用
#include <esp_camera.h>         // カメラ制御用

#define DEBUG 0                   // ログ出力フラグ。運用時は0にしてください。
#define RUN_INTERVAL 600          // 送信間隔（10分）
#define LED_GPIO_NUM GPIO_NUM_14  // LED PIN NO

#define WIFI_SSID     ""  // Wi-Fi SSID。設定してください。
#define WIFI_PASSWORD ""   // Wi-Fi パスワード。設定してください。

#define DEVICE_ID "12345678"    // 計測器ID。ハウス環境モニタアプリの　設定画面の計測器IDを設定してください。
#define PASSCODE  "1234567890"  // 計測器パスコード。ハウス環境モニタアプリの　設定画面から上記の計測器を選択し表示される送信パスコードを設定してください。
#define AUTHORIZATION  DEVICE_ID ":" PASSCODE

// カメラ設定値
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 11
#define SIOD_GPIO_NUM 17
#define SIOC_GPIO_NUM 41
#define Y9_GPIO_NUM 13
#define Y8_GPIO_NUM 4
#define Y7_GPIO_NUM 10
#define Y6_GPIO_NUM 5
#define Y5_GPIO_NUM 7
#define Y4_GPIO_NUM 16
#define Y3_GPIO_NUM 15
#define Y2_GPIO_NUM 6
#define VSYNC_GPIO_NUM 42
#define HREF_GPIO_NUM 18
#define PCLK_GPIO_NUM 12

// ======================================
// サーバ接続情報
// ======================================
String serverName = "asia-northeast1-vhousem.cloudfunctions.net";
String serverPath = "/i";
const int serverPort = 443;

// ======================================
// HTTPS用ルート証明書
// ======================================
const char* ROOT_CA =
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

// ======================================
// ログ
// ======================================
void log(String text) {
#if DEBUG
  Serial.println(text);
  Serial.flush();
#else
  return;
#endif
}

// ======================================
// カメラ初期化
// ======================================
void cam_init() {
    log("cam initialize.");

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;

    config.frame_size = FRAMESIZE_UXGA;
    // config.frame_size = FRAMESIZE_VGA;

    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 10;
    config.fb_count = 2;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        log("failure: Camera uninitialized.");

        // Retry
        log("reboot..");
        delay(1000);
        esp_restart();
    }

    sensor_t* s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
    // s->set_brightness(s, 1);  // 少し明るく

    log("cam initialized.");
    delay(100);
}

// ======================================
// Wi-Fi接続
// ======================================
bool connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (retryCount >= 20) {
      log("failure: Wi-Fi unconnected.");
      return false;
    }
    retryCount++;

    delay(500);
    log(".");
  }
  log("WiFi Connected.");
  long rssi = WiFi.RSSI();
  log("RSSI: " + String(rssi) + " dBm");

  return true;
}

// ======================================
// 画像送信
// ======================================
void sendImage() {
  // カメラ画像キャプチャ
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    log("failure: Camera capture");
    esp_camera_fb_return(fb);
    return;
  }

  // 画像送信
  WiFiClientSecure *client = new WiFiClientSecure;
  if (!client) {
    log("failure: WiFiClientSecure.");
    return;
  }
  client->setCACert(ROOT_CA);

  if (!client->connect(serverName.c_str(), serverPort)) {
    log("failure: Cconnection to server.");
    esp_camera_fb_return(fb);

  } else {
    log("Connection successful!");

    // 認証情報
    unsigned char authorization[] = AUTHORIZATION;
    unsigned char base64[50];
    unsigned int base64_length = encode_base64(authorization, strlen((char *) authorization), base64);

    char authValue[56] = "Basic ";
    strcat(authValue, (char *) base64);
    String authHeader = authValue;

    // Content-Length
    String bodyStart = "--boundary\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String bodyEnd = "\r\n--boundary--\r\n";
    uint32_t imageLength = fb->len;
    int contentLength = bodyStart.length() + imageLength + bodyEnd.length();
    log("Content length: ");
    log(String(contentLength));

    // 送信
    client->println("POST " + serverPath + " HTTP/1.1");
    client->println("Host: " + serverName);
    client->println("Authorization: " + authHeader);
    client->println("Content-Length: " + String(contentLength));
    client->println("Content-Type: multipart/form-data; boundary=boundary");
    client->println();
    client->print(bodyStart);

    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0; n<fbLen; n=n+1024) {
      if (n+1024 < fbLen) {
        client->write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        client->write(fbBuf, remainder);
      }
    } 

    client->println(bodyEnd);

    // 受信
    log("Response:");
    String getAll;
    String getBody;
    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;
    while ((startTimer + timoutTimer) > millis()) {
      delay(100);      
      while (client->available()) {
        char c = client->read();
        if (c == '\n') {
          if (getAll.length()==0) { 
            state = true; 
          }
          getAll = "";
        } else if (c != '\r') { 
          getAll += String(c); 
        }
        if (state == true) { 
          getBody += String(c); 
        }
        startTimer = millis();
      }

      if (getBody.length() > 0) { 
        break; 
      }
    }
    log(getBody);

    client->clearWriteError();
    client->stop();

    // frame buffer解放
    esp_camera_fb_return(fb);
  }
}

void setup() {
#if DEBUG
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
#endif

  const unsigned long startTime = millis();

  // LED ON
  gpio_reset_pin(LED_GPIO_NUM);
  pinMode(LED_GPIO_NUM, OUTPUT);
  digitalWrite(LED_GPIO_NUM, 0);
  delay(1000);
  digitalWrite(LED_GPIO_NUM, 1);

  // カメラ初期化
  cam_init();

  // Wi-Fi接続
  if (connectWiFi()) {
    digitalWrite(LED_GPIO_NUM, 0);
    // 画像送信
    sendImage();

    digitalWrite(LED_GPIO_NUM, 1);
    delay(1000);
    digitalWrite(LED_GPIO_NUM, 0);
    delay(2000);
  }

  digitalWrite(LED_GPIO_NUM, 1);

  // ======================================
  // deep sleep
  // ======================================
  // Deep sleep時、周辺機器への電力供給を止める
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  // ディープスリープから復帰した場合に保持しておくメモリ領域への電源供給は不要
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF); 
  // RTC_DATA_ATTRに保存したデータの保持は不要
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  // Deep sleep時、水晶振動子への電力供給を止める
  esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
  delay(100);

  const unsigned int now = millis();
  const unsigned int runInterval = RUN_INTERVAL * 1000 * 1000 - (now - startTime) * 1000;

  esp_sleep_enable_timer_wakeup(runInterval);
  esp_deep_sleep_start();
}

void loop() { }
