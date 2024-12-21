#define LGFX_USE_V1
//#define LGFX_AUTODETECT // Autodetect board

#include <Arduino.h>
#include "SPIFFS.h"
#include "main.h"
#include <lvgl.h>
#include "ui.h"
#include <LovyanGFX.hpp>
#include <ESP32Time.h>
#include <ArduinoNvs.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "SD.h"
#include "FS.h"
#include "Audio.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <esp_log.h>
#include <Preferences.h>
#include <Ticker.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <stdarg.h>  // Thư viện cho va_list, va_start, va_end

LiquidCrystal_I2C lcd(0x27,16,2);
WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char* TAG = "";
const char* mqtt_server = "113.177.27.162";  // Hoặc broker của bạn
const int mqtt_port = 8889;
const char* mqtt_topic_config = "lumi/qr/config";
const char* mqtt_topic_status = "lumi/qr/status";
const char* mqtt_topic_log = "lumi/qr/log"; // Thay thế bằng topic log của bạn

const char* ssid = "LUMI_TEST";
const char* password = "lumivn274!";

// Khởi tạo UDP
WiFiUDP ntpUDP;

// Khởi tạo NTPClient
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Cập nhật mỗi 60 giây

struct QRCode {
    int id;
    String name;
    String account;
    String bank;
    String qrText;
};
Preferences preferences;
std::vector<QRCode> qrList;
int activeQRId = -1;
static int nextQRId = 1;
bool g_is_reverting = false;
bool mqttLoggingEnabled = false;
class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_ST7796 _panel_instance;

  lgfx::Bus_Parallel8 _bus_instance;

  lgfx::Light_PWM _light_instance;

  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.port = 0;
      cfg.freq_write = 20000000;
      cfg.pin_wr = 47; // pin number connecting WR
      cfg.pin_rd = -1; // pin number connecting RD
      cfg.pin_rs = 0;  // Pin number connecting RS(D/C)
      cfg.pin_d0 = 9;  // pin number connecting D0
      cfg.pin_d1 = 46; // pin number connecting D1
      cfg.pin_d2 = 3;  // pin number connecting D2
      cfg.pin_d3 = 8;  // pin number connecting D3
      cfg.pin_d4 = 18; // pin number connecting D4
      cfg.pin_d5 = 17; // pin number connecting D5
      cfg.pin_d6 = 16; // pin number connecting D6
      cfg.pin_d7 = 15; // pin number connecting D7

      _bus_instance.config(cfg);              // Apply the settings to the bus.
      _panel_instance.setBus(&_bus_instance); // Sets the bus to the panel.
    }

    {                                      // Set display panel control.
      auto cfg = _panel_instance.config(); // Get the structure for display panel settings.

      cfg.pin_cs = -1;   // Pin number to which CS is connected (-1 = disable)
      cfg.pin_rst = 4;   // pin number where RST is connected (-1 = disable)
      cfg.pin_busy = -1; // pin number to which BUSY is connected (-1 = disable)

      // * The following setting values ​​are set to general default values ​​for each panel, and the pin number (-1 = disable) to which BUSY is connected, so please try commenting out any unknown items.

      cfg.memory_width = 320;  // Maximum width supported by driver IC
      cfg.memory_height = 480; // Maximum height supported by driver IC
      cfg.panel_width = 320;   // actual displayable width
      cfg.panel_height = 480;  // actual displayable height
      cfg.offset_x = 0;        // Panel offset in X direction
      cfg.offset_y = 0;        // Panel offset in Y direction
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;

      _panel_instance.config(cfg);
    }

    {                                      // Set backlight control. (delete if not necessary)
      auto cfg = _light_instance.config(); // Get the structure for backlight configuration.

      cfg.pin_bl = 45;     // pin number to which the backlight is connected
      cfg.invert = false;  // true to invert backlight brightness
      cfg.freq = 44100;    // backlight PWM frequency
      cfg.pwm_channel = 0; // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance); // Sets the backlight to the panel.
    }

    { // Configure settings for touch screen control. (delete if not necessary)
      auto cfg = _touch_instance.config();

      cfg.x_min = 0;   // Minimum X value (raw value) obtained from the touchscreen
      cfg.x_max = 319; // Maximum X value (raw value) obtained from the touchscreen
      cfg.y_min = 0;   // Minimum Y value obtained from touchscreen (raw value)
      cfg.y_max = 479; // Maximum Y value (raw value) obtained from the touchscreen
      cfg.pin_int = 7; // pin number to which INT is connected
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;

      // For I2C connection
      cfg.i2c_port = 1;    // Select I2C to use (0 or 1)
      cfg.i2c_addr = 0x38; // I2C device address number
      cfg.pin_sda = 6;     // pin number where SDA is connected
      cfg.pin_scl = 5;     // pin number to which SCL is connected
      cfg.freq = 400000;   // set I2C clock

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance); // Set the touchscreen to the panel.
    }

    setPanel(&_panel_instance); // Sets the panel to use.
  }
};

// Create an instance of the prepared class.
LGFX tft;

static lv_color_t disp_draw_buf[screenWidth * SCR];
static lv_color_t disp_draw_buf2[screenWidth * SCR];

ESP32Time rtc(3 * 3600);
HTTPClient http;
#ifdef MUSIC_PLAYER
Audio audio;
#endif
bool playing;


uint8_t ext[] PROGMEM = {
    // APP DATA
    0xA0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x54, 0x65, 0x73, 0x74, 0x20, 0x41, 0x70, 0x70, 0x00,

    // UI COMPONENTS
    // d, type, comp id   , parent id ,    x pos  ,    y pos  ,   width  ,   height   , text ..... terminator
    0xAA, 0x01, 0x10, 0x01, 0xAB, 0x01, 0x00, 0x14, 0x00, 0x14, 0x00, 0x19, 0x00, 0x19, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x00,
    0xAA, 0x02, 0x10, 0x02, 0xAB, 0x01, 0x00, 0x14, 0x00, 0x46, 0x00, 0x64, 0x00, 0x00, 0x43, 0x6c, 0x69, 0x63, 0x6b, 0x00,
    0xAA, 0x05, 0x10, 0x03, 0xAB, 0x01, 0x00, 0x14, 0x00, 0x96, 0x00, 0xFA, 0x00, 0x05, 0x00,
    0xAA, 0x07, 0x10, 0x04, 0xAB, 0x01, 0x00, 0x14, 0x00, 0xFA, 0x00, 0x32, 0x00, 0x14, 0x00,
    0xAA, 0x01, 0x10, 0x05, 0xAB, 0x01, 0x00, 0x14, 0x01, 0x2C, 0x00, 0x19, 0x00, 0x19, 0x4c, 0x6f, 0x72, 0x65, 0x6d, 0x20, 0x69, 0x70, 0x73, 0x75, 0x6d, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x0a, 0x73, 0x69, 0x74, 0x20, 0x61, 0x6d, 0x65, 0x74, 0x2c, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x65, 0x63, 0x74, 0x65, 0x74, 0x75, 0x72, 0x20, 0x0a, 0x61, 0x64, 0x69, 0x70, 0x69, 0x73, 0x63, 0x69, 0x6e, 0x67, 0x20, 0x65, 0x6c, 0x69, 0x74, 0x2e, 0x00
    };

AppComponent testApp[10];

void logMessage(const char* format, ...) {
    char buffer[1024];  // Bộ đệm để chứa thông điệp log

    // Lấy các tham số biến
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Cập nhật thời gian từ NTP server
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();

    // Tạo chuỗi log với thời gian
    String logEntry = "[" + String(epochTime) + "] " + buffer;

    // Log ra màn hình
    Serial.println(logEntry);

    // Gửi log qua MQTT
    if (mqttClient.connected()) {
        mqttClient.publish(mqtt_topic_log, logEntry.c_str());
    } else {
        Serial.println("MQTT not connected, unable to send log");
    }
}

void extractApp(uint8_t *data, int size)
{

  int a = 0, i = 0, j, z = size;
  uint8_t buf[100];
  while (1)
  {
    // copy component data to buffer
    for (j = 0; j < 14; j++)
    {
      if (i >= z)
      {
        break; // not enough data for component
      }
      buf[j] = data[i];
      i++;
    }
    while (1)
    {
      buf[j] = data[i];
      j++;
      i++;
      if (buf[j - 1] == 0x00)
      {
        break; // component terminator found
      }
      if (i >= z)
      {
        break; // component terminator not found & end of component data
      }
    }
    if (buf[j - 1] != 0x00)
    {
      break; // component incomplete
    }

    // component info was loaded to buffer
    if (buf[0] == 0xAA)
    {
      testApp[a].type = (Type)buf[1];
      testApp[a].id = buf[2] << 8 | buf[3];
      testApp[a].parent = buf[4] << 8 | buf[5];
      testApp[a].xPos = buf[6] << 8 | buf[7];
      testApp[a].yPos = buf[8] << 8 | buf[9];
      testApp[a].width = buf[10] << 8 | buf[11];
      testApp[a].height = buf[12] << 8 | buf[13];
      strncpy(testApp[a].text, (char *)buf + 14, j - 14);
      printf("App UI Component > 0x%X\n", uuid(testApp[a].parent, testApp[a].id));
      a++;
    }
    else if (buf[0] == 0xA0)
    {
      char appName[20];
      strncpy(appName, (char *)buf + 14, j - 14);
      printf("App Name %s\n", appName);
    }
  }
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }

  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);

  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
// #ifdef PLUS
  uint16_t touchX, touchY;

  bool touched = tft.getTouch(&touchX, &touchY);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;
    currentMillis = millis();
  }
// #else
//   data->point.x = touch_data.xpos;
//   data->point.y = touch_data.ypos;

//   if (touch_data.event == 1)
//   {
//     data->state = LV_INDEV_STATE_PR;
//     currentMillis = millis();
//   }
//   else
//   {
//     data->state = LV_INDEV_STATE_REL;
//   }
// #endif
}

void requestResult(int requestCode, int statusCode, String payload, long time)
{

  ESP_LOGI(TAG, "Request %d received, time %dms, code: %d\n", requestCode, time, statusCode);

  if (statusCode == HTTP_CODE_OK)
  {
    ESP_LOGI(TAG, "%s", payload.c_str());
  }

  switch (requestCode)
  {
  case TIME_REQUEST:
    if (statusCode == HTTP_CODE_OK)
    {
      DynamicJsonDocument json(100);
      deserializeJson(json, payload);
      long t = json["timestamp"].as<long>();
      rtc.setTime(t);
    }
    break;
  case APPS_REQUEST:
    if (statusCode == HTTP_CODE_OK)
    {
      DynamicJsonDocument json(2048);
      deserializeJson(json, payload);
      JsonArray array = json.as<JsonArray>();
      int x = 0;

      for (JsonVariant j : array)
      {
        JsonObject v = j.as<JsonObject>();
        String name = v["name"].as<String>();
        strncpy(appList[x].name, name.c_str(), name.length() + 1);
        appList[x].id = v["id"].as<uint32_t>();
        appList[x].version = v["version"].as<uint32_t>();
        appList[x].size = v["size"].as<uint32_t>();
        appList[x].state = true;
        x++;

        if (x >= MAX_APPS)
        {
          break;
        }
      }
    }
    break;
#ifdef PLUS
  case BLOG_REQUEST:
    if (statusCode == HTTP_CODE_OK)
    {
      DynamicJsonDocument json(2048);
      deserializeJson(json, payload);
      if (json["success"])
      {
        JsonArray array = json["article"].as<JsonArray>();
        for (JsonVariant j : array)
        {
          JsonObject v = j.as<JsonObject>();
          String text = v["text"].as<String>();
          uint16_t id = v["id"].as<uint16_t>();
          String link = v["image"].as<String>();
          add_blog_item(blogList, text.c_str(), id);
        }
      } else {
        blogText(ui_blogPanel, 50, "Failed to load the blog list");
      }
    } else {
      blogText(ui_blogPanel, 50, "Failed to load the blog list");
    }
    lv_obj_add_flag(ui_blogSpinner, LV_OBJ_FLAG_HIDDEN);
    break;
  case BLOG_ITEM_REQUEST:
    blog_panel(ui_appPanel);
    lv_obj_clear_flag(ui_blogSpinner, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(blogList, LV_OBJ_FLAG_HIDDEN);
    if (statusCode == HTTP_CODE_OK)
    {

      DynamicJsonDocument json(4096);
      deserializeJson(json, payload);
      if (json["success"])
      {

        String title = json["title"].as<String>();
        lv_obj_t *t = blogTitle(ui_blogPanel, (char *)title.c_str());
        lv_obj_set_style_text_font(t, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_update_layout(t);
        int16_t y = lv_obj_get_height(t);
        y += 10;

        JsonArray array = json["text"].as<JsonArray>(); // text
        for (JsonVariant j : array)
        {
          String text = j.as<String>();
          lv_obj_t *p = blogText(ui_blogPanel, y, (char *)text.c_str());
          lv_obj_update_layout(p);
          y += 10;
          y += lv_obj_get_height(p);
        }

        JsonArray images = json["image"].as<JsonArray>(); // images
        for (JsonVariant j : images)
        {
          String link = j.as<String>();
        }
      }
      else
      {
        blogText(ui_blogPanel, 50, "Failed to load the blog");
      }
    }
    else
    {
      blogText(ui_blogPanel, 50, "Failed to load the blog");
    }
    lv_obj_add_flag(ui_blogSpinner, LV_OBJ_FLAG_HIDDEN);
    lv_obj_scroll_by(ui_blogPanel, 0, 2, LV_ANIM_ON);
    break;
    #endif
  }
}

void sendRequest(void *parameter)
{
  long t;

  for (int r = 0; r < MAX_REQUEST; r++)
  {
    if (request[r].active)
    {
      t = millis();
      http.begin(request[r].url);
      int httpCode;
      if (request[r].method)
      {
        httpCode = http.POST(request[r].data);
      }
      else
      {
        httpCode = http.GET();
      }

      String payload = http.getString();

      // http.end();
      t = millis() - t;
      requestResult(request[r].code, httpCode, payload, t);
      request[r].active = false;
    }
  }
  http.end();
  activeRequest = false;
  // When you're done, call vTaskDelete. Don't forget this!
  vTaskDelete(NULL);
}


bool runRequest()
{
  // returns true if the task was created
  // returns false if the previous task has not completed, new one cannot be created
  if (!activeRequest)
  {
    activeRequest = true;
    // xTaskCreatePinnedToCore(
    xTaskCreate(
        sendRequest,     // Function that should be called
        "HTTP Requests", // Name of the task (for debugging)
        8192,            // Stack size (bytes)
        NULL,            // Parameter to pass
        1,               // Task priority
        NULL
        // NULL,               // Task handle
        // 1
    );

    return true;
  }
  else
  {
    return false;
  }
}

#ifdef MUSIC_PLAYER
void musicPlayer(void *parameter)
{
	logMessage(TAG, "Music Player started");
	while (1)
	{
		audio.loop();
    	vTaskDelay(10 / portTICK_PERIOD_MS);

	}
	vTaskDelete(NULL);
}

void runMusic()
{
	xTaskCreatePinnedToCore(
		musicPlayer,    // Function that should be called
		"Music Player", // Name of the task (for debugging)
		16384,          // Stack size (bytes)
		NULL,           // Parameter to pass
		1,              // Task priority
		NULL, // Task handle
		1);
}

void playFile(const char *track)
{
	audio.connecttoFS(SD, track);
	logMessage(TAG, "Playing file: %s", track);
}

void playPause()
{
  audio.pauseResume();
  bool isPlay = audio.isRunning();
}

void changeMusic(bool forward)
{

  int r = random(MAX_MUSIC);
  playFile(music[r].path);
}

void listMusic(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  int x = 0;
  while (file)
  {
    if (!file.isDirectory())
    {

      String fileName = String(file.name());
      if (!fileName.startsWith(".") && fileName.endsWith(".mp3") && x < MAX_MUSIC)
      {
        Serial.print("  MP3 ");

        fileName = "/" + fileName;
        strncpy(music[x].path, fileName.c_str(), fileName.length() + 1);
        music[x].id = x;
        music[x].size = (uint32_t)file.size();
        x++;
      }

      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void getMusicList()
{
  int v = audio.getVolume();
  lv_slider_set_value(ui_musicVolume, v, LV_ANIM_ON);

  listMusic(SD, "/", 0);
}
#endif

#ifdef PLUS
void getFileList(lv_obj_t *parent, const char *path)
{

  File root = SD.open(path);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    String fileName = String(file.name());
    // char n[50];
    // strncpy(n, fileName.c_str(), fileName.length() + 1);
    if (!fileName.startsWith("."))
    {
      add_file_item(parent, (char *)fileName.c_str(), file.size(), !file.isDirectory());
    }
    file = root.openNextFile();
  }
}
#endif

#ifdef LVGL_LOOP
void lvgl_loop(void *parameter)
{

  while (true)
  {
    lv_timer_handler();
    delay(5);
  }
  vTaskDelete(NULL);
}

void guiHandler()
{

  xTaskCreatePinnedToCore(
      // xTaskCreate(
      lvgl_loop,   // Function that should be called
      "LVGL Loop", // Name of the task (for debugging)
      16384,       // Stack size (bytes)
      NULL,        // Parameter to pass
      1,           // Task priority
      // NULL
      NULL, // Task handle
      1);
}
#endif

void getNVSData()
{
  String val;
  val = NVS.getString("ssid1");
  strncpy(ssid1, val.c_str(), val.length() + 1);
  val = NVS.getString("ssid2");
  strncpy(ssid2, val.c_str(), val.length() + 1);
  val = NVS.getString("ssid3");
  strncpy(ssid3, val.c_str(), val.length() + 1);
  val = NVS.getString("ssid4");
  strncpy(ssid4, val.c_str(), val.length() + 1);
  val = NVS.getString("ssid5");
  strncpy(ssid5, val.c_str(), val.length() + 1);
  val = NVS.getString("pass1");
  strncpy(pass1, val.c_str(), val.length() + 1);
  val = NVS.getString("pass2");
  strncpy(pass2, val.c_str(), val.length() + 1);
  val = NVS.getString("pass3");
  strncpy(pass3, val.c_str(), val.length() + 1);
  val = NVS.getString("pass4");
  strncpy(pass4, val.c_str(), val.length() + 1);
  val = NVS.getString("pass5");
  strncpy(pass5, val.c_str(), val.length() + 1);
  brightness = NVS.getInt("brightness", brightness);
  themeColor = NVS.getInt("theme", themeColor);
  screenTime = (long)NVS.getInt("screentime", (uint32_t)screenTime);

  NVS.getBlob("passCode", passcode.code, 4);
  passcode.set = NVS.getInt("passSet", 0) != 0;

  printf("loaded screentime %d\n", screenTime);

  Serial.printf("Passcode loaded %s - %d%d%d%d\n", passcode.set ? "ON" : "OFF", passcode.code[0], passcode.code[1], passcode.code[2], passcode.code[3]);
}



void saveSettings()
{
  NVS.setInt("brightness", brightness);
  NVS.setInt("theme", themeColor);
  NVS.setBlob("passCode", passcode.code, 4);
  NVS.setInt("passSet", passcode.set ? 1 : 0);
  NVS.setInt("screentime", (uint32_t)screenTime);
  Serial.printf("Passcode set %s - %d%d%d%d\n", passcode.set ? "ON" : "OFF", passcode.code[0], passcode.code[1], passcode.code[2], passcode.code[3]);
  printf("saved screentime %d\n", screenTime);
}


void deep_sleep()
{

  vibrate(100);

  tft.setBrightness(0);

  esp_sleep_enable_ext0_wakeup(WAKE_PIN, 0); // 1 = High, 0 = Low

  // Go to sleep now
  Serial.println("Going to deep sleep now");
  esp_deep_sleep_start();
}

void wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    rtc.setTime(1663491331); // Sunday, September 18, 2022 8:55:31 AM
    break;
  }
}

void light_sleep()
{
	// digitalWrite(MOTOR, LOW);
	tft.setBrightness(0);
	openLock();

	esp_sleep_enable_ext0_wakeup(WAKE_PIN, 0); // 1 = High, 0 = Low

	// Go to sleep now
	Serial.println("Going to light sleep now");
	esp_light_sleep_start();
}


//HieuNM Add

#ifdef LV_USE_QRCODE
lv_obj_t * ui_startScreen;
lv_obj_t * qr;
lv_obj_t * bank_label;
lv_obj_t * name_label;
lv_obj_t * account_label;

lv_obj_t *ui_Panel2;
lv_obj_t *ui_startButton;
lv_obj_t *ui_backButton;
lv_obj_t *ui_searchButton;
lv_obj_t *ui_BankName;

void ui_start_screen_init(void) {
	ui_startScreen = lv_obj_create(NULL);

	lv_obj_clear_flag(ui_startScreen, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_set_style_bg_color(ui_startScreen, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_bg_opa(ui_startScreen, LV_OPA_COVER, LV_PART_MAIN);
	lv_scr_load(ui_startScreen);

	lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
	lv_color_t fg_color = lv_palette_darken(LV_PALETTE_NONE, 4);
	qr = lv_qrcode_create(ui_startScreen, 250, fg_color, bg_color);

	lv_obj_set_style_border_color(qr, bg_color, 0);
	lv_obj_set_style_border_width(qr, 5, 0);

	// bank_label = lv_label_create(ui_startScreen);
	name_label = lv_label_create(ui_startScreen);
	account_label = lv_label_create(ui_startScreen);

	ui_BankName = lv_imgbtn_create(ui_startScreen);

}

void lv_print_qrcode(const char* qr_text, const char* bank_name, const char* name, const char* account)
{
	/*Set data*/
	lv_qrcode_update(qr, qr_text, strlen(qr_text));
	lv_obj_center(qr);
	lv_obj_set_align(qr, LV_ALIGN_CENTER);

    if (strcmp(bank_name, "Techcombank") == 0) {
        lv_imgbtn_set_src(ui_BankName, LV_IMGBTN_STATE_RELEASED, NULL, &techcombank, NULL);
		lv_obj_set_height(ui_BankName, techcombank.header.h);
		lv_obj_set_width(ui_BankName, techcombank.header.w);
    } else if (strcmp(bank_name, "Vietinbank") == 0) {
        lv_imgbtn_set_src(ui_BankName, LV_IMGBTN_STATE_RELEASED, NULL, &viettinbank, NULL);
		lv_obj_set_height(ui_BankName, viettinbank.header.h);
		lv_obj_set_width(ui_BankName, viettinbank.header.w);
    } else if (strcmp(bank_name, "MBBank") == 0) {
        lv_imgbtn_set_src(ui_BankName, LV_IMGBTN_STATE_RELEASED, NULL, &mbbank, NULL);
		lv_obj_set_height(ui_BankName, mbbank.header.h);
		lv_obj_set_width(ui_BankName, mbbank.header.w);
    } else if (strcmp(bank_name, "TPBank") == 0) {
        lv_imgbtn_set_src(ui_BankName, LV_IMGBTN_STATE_RELEASED, NULL, &tpbank, NULL);
		lv_obj_set_height(ui_BankName, tpbank.header.h);
		lv_obj_set_width(ui_BankName, tpbank.header.w);
    } else {
        // Default or unknown bank
        lv_imgbtn_set_src(ui_BankName, LV_IMGBTN_STATE_RELEASED, NULL, &viettinbank, NULL);
    }


    lv_obj_set_x(ui_BankName, 0);
    lv_obj_set_y(ui_BankName, 0);
    lv_obj_align_to(ui_BankName, qr, LV_ALIGN_OUT_TOP_MID, 0, -15);

	/*Add a border with bg_color*/

	// // Tạo các label trên màn hình mới
	// printf("lv_label_set_text bank\n");
	// lv_label_set_text(bank_label, bank_name);
	// lv_obj_set_style_text_font(bank_label, &lv_font_montserrat_32, 0);
	// lv_obj_set_style_text_color(bank_label, lv_color_black(), 0);  
	// lv_obj_align_to(bank_label, qr, LV_ALIGN_OUT_TOP_MID, 0, -10);

	lv_label_set_text(name_label, name);
	lv_obj_set_style_text_font(name_label, &lv_font_montserrat_24, 0);
	lv_obj_set_style_text_color(name_label, lv_color_black(), 0);
	lv_obj_align_to(name_label, qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

	lv_label_set_text(account_label, account);
	lv_obj_set_style_text_font(account_label, &lv_font_montserrat_20, 0);
	lv_obj_set_style_text_color(account_label, lv_color_black(), 0);  
	lv_obj_align_to(account_label, name_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 1);
}
#endif

void lcd_1602_init() {
	Wire.begin(11, 10, 400000); 
	lcd.init();
	lcd.clear();

	// Print a message to the LCD.
	lcd.backlight();
	lcd.setCursor(3, 0);  
	lcd.print("Xin Chao !!");
}

void lcd_1602_print_amount(double amount) {
	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Amount (VND): ");
	lcd.setCursor(0, 1);
	lcd.print(amount);
}

void saveQRListToFlash() {
    preferences.begin("qr-storage", false);
    preferences.putInt("qr_count", qrList.size());
    
    // Lưu từng QR
    for (size_t i = 0; i < qrList.size(); i++) {
        String prefix = "qr_" + String(i) + "_";
        preferences.putInt((prefix + "id").c_str(), qrList[i].id);
        preferences.putString((prefix + "name").c_str(), qrList[i].name);
        preferences.putString((prefix + "account").c_str(), qrList[i].account);
        preferences.putString((prefix + "bank").c_str(), qrList[i].bank);
        preferences.putString((prefix + "text").c_str(), qrList[i].qrText);
    }
    
    // Lưu active QR ID
    preferences.putInt("active_qr", activeQRId);
    preferences.end();
}

void loadQRListFromFlash() {
    preferences.begin("qr-storage", true);
    
    // Đọc số lượng QR
    int qrCount = preferences.getInt("qr_count", 0);
    
    // Đọc từng QR
    qrList.clear();
    for (int i = 0; i < qrCount; i++) {
        String prefix = "qr_" + String(i) + "_";
        QRCode qr;
        qr.id = preferences.getInt((prefix + "id").c_str(), 0);
        qr.name = preferences.getString((prefix + "name").c_str(), "");
        qr.account = preferences.getString((prefix + "account").c_str(), "");
        qr.bank = preferences.getString((prefix + "bank").c_str(), "");
        qr.qrText = preferences.getString((prefix + "text").c_str(), "");
        qrList.push_back(qr);
    }
    
    // Đọc active QR ID
    activeQRId = preferences.getInt("active_qr", -1);
    
    // Cập nhật nextQRId
    nextQRId = 1;
    for(const auto& qr : qrList) {
        if(qr.id >= nextQRId) {
            nextQRId = qr.id + 1;
        }
    }
    
    Serial.print("Next QR ID will be: ");
    Serial.println(nextQRId);

    preferences.end();
}

void publishQRList() {
    DynamicJsonDocument doc(8192);
    doc["activeQRId"] = activeQRId;
    
    JsonArray array = doc.createNestedArray("qrList");
    for(const auto& qr : qrList) {
        JsonObject obj = array.createNestedObject();
        obj["id"] = qr.id;
        obj["name"] = qr.name;
        obj["account"] = qr.account;
        obj["bank"] = qr.bank;
        obj["qrText"] = qr.qrText;
    }
    
    String response;
    serializeJson(doc, response);
    Serial.println("publishQRList: " + response);
    mqttClient.publish(mqtt_topic_status, response.c_str());
}

Ticker revertTicker;

void revertToOriginalQR() {	
	g_is_reverting = true;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    if (String(topic) == mqtt_topic_config) {
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, payload, length);
        
        String command = doc["command"].as<String>();
        String type = doc["type"].as<String>();
        
        if (command == "activate") {
            if (type == "static") {
                int id = doc["id"];
                auto it = std::find_if(qrList.begin(), qrList.end(),
                                      [id](const QRCode& qr) { return qr.id == id; });
                
                if (it != qrList.end()) {
                    activeQRId = id;
                    saveQRListToFlash();
                    
                    // Phản hồi thành công
                    DynamicJsonDocument responseDoc(256);
                    responseDoc["status"] = "success";
                    responseDoc["message"] = "Static QR code activated";
                    String response;
                    serializeJson(responseDoc, response);
                    mqttClient.publish(mqtt_topic_status, response.c_str());

                    lv_print_qrcode(it->qrText.c_str(), it->bank.c_str(), 
                                    it->name.c_str(), it->account.c_str());
                } else {
                    // Phản hồi lỗi
                    DynamicJsonDocument responseDoc(256);
                    responseDoc["status"] = "error";
                    responseDoc["message"] = "QR code ID not found";
                    String response;
                    serializeJson(responseDoc, response);
                    mqttClient.publish(mqtt_topic_status, response.c_str());
                }
            } else if (type == "dynamic") {
                String name = doc["name"].as<String>();
                String account = doc["account"].as<String>();
                String bank = doc["bank"].as<String>();
                String qrText = doc["qrText"].as<String>();
                double amount = doc["amount"].as<double>();
                int time = doc["time"].as<int>();
				if(time == 0) {
					time = 10;
				}
                // Xử lý mã QR động
                lv_print_qrcode(qrText.c_str(), bank.c_str(), name.c_str(), account.c_str());
				lcd_1602_print_amount(amount);	
                ESP_LOGI(TAG, "Dynamic QR code activated for %d seconds", time);

                // Thiết lập bộ đếm thời gian để quay về mã QR gốc
                revertTicker.once(time, revertToOriginalQR);

                // Phản hồi thành công
                DynamicJsonDocument responseDoc(256);
                responseDoc["status"] = "success";
                responseDoc["message"] = "Dynamic QR code activated";
                String response;
                serializeJson(responseDoc, response);
                mqttClient.publish(mqtt_topic_status, response.c_str());
            }
        }
        else if (command == "add") {
			printf("add command\n");
            QRCode qr;
            qr.id = nextQRId++;
            qr.name = doc["name"].as<String>();
            qr.account = doc["account"].as<String>();
            qr.bank = doc["bank"].as<String>();
            qr.qrText = doc["qrText"].as<String>();
            
            qrList.push_back(qr);
            saveQRListToFlash();
            
            // Publish updated list
            publishQRList();
            
            // Phản hồi thành công
            DynamicJsonDocument responseDoc(256);
            responseDoc["status"] = "success";
            responseDoc["message"] = "QR code added";
            String response;
            serializeJson(responseDoc, response);
            mqttClient.publish(mqtt_topic_status, response.c_str());
        }
        else if (command == "delete") {
            int idToDelete = doc["id"];
            auto it = std::find_if(qrList.begin(), qrList.end(),
                                  [idToDelete](const QRCode& qr) { return qr.id == idToDelete; });
            
            if (it != qrList.end()) {
                if (idToDelete == activeQRId) {
                    activeQRId = -1;
                    lv_obj_clean(lv_scr_act());
                }
                qrList.erase(it);
                saveQRListToFlash();
                publishQRList();
                
                // Phản hồi thành công
                DynamicJsonDocument responseDoc(256);
                responseDoc["status"] = "success";
                responseDoc["message"] = "QR code deleted";
                String response;
                serializeJson(responseDoc, response);
                mqttClient.publish(mqtt_topic_status, response.c_str());
            } else {
                // Phản hồi lỗi
                DynamicJsonDocument responseDoc(256);
                responseDoc["status"] = "error";
                responseDoc["message"] = "QR code ID not found";
                String response;
                serializeJson(responseDoc, response);
                mqttClient.publish(mqtt_topic_status, response.c_str());
            }
        }
        else if (command == "get_list") {
            ESP_LOGI(TAG, "Received get_list command");
            // Publish the current QR list
            publishQRList();
        }
        else if (command == "amount_received") {
            double receivedAmount = doc["amount"];
            
            // Xử lý số tiền đã nhận
            ESP_LOGI(TAG, "Amount received: %.2f", receivedAmount);
			if(receivedAmount == 50000) {
				playFile("/50000.mp3");
			} else if(receivedAmount == 200000) {
				playFile("/200000.mp3");
			} else if(receivedAmount == 10000) {
				playFile("/10000.mp3");
			} else if(receivedAmount == 1000000) {
				playFile("/1000000.mp3");
			} 
            // Phản hồi thành công
            DynamicJsonDocument responseDoc(256);
            responseDoc["status"] = "success";
            responseDoc["message"] = "Amount received processed";
            String response;
            serializeJson(responseDoc, response);
            mqttClient.publish(mqtt_topic_status, response.c_str());
        }
		else if (command == "setlog") {
            bool enable = doc["enable"].as<bool>();
            mqttLoggingEnabled = enable;
            Serial.printf("MQTT logging %s\n", enable ? "enabled" : "disabled");

			// Tạo phản hồi JSON
            DynamicJsonDocument responseDoc(256);
            responseDoc["status"] = "success";
            responseDoc["message"] = "Logging status updated";
            responseDoc["loggingEnabled"] = enable;

            // Chuyển đổi JSON thành chuỗi
            String response;
            serializeJson(responseDoc, response);

            // Gửi phản hồi qua MQTT
            mqttClient.publish(mqtt_topic_status, response.c_str());
        }
    }
}

void mqtt_start(void) {
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(mqtt_callback);
}

void reconnect_mqtt() {
    while (!mqttClient.connected()) {
        printf(TAG, "Attempting MQTT connection to ");
        printf(TAG, mqtt_server);
        printf(TAG, "...");
        
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        
        if (mqttClient.connect(clientId.c_str())) {
            ESP_LOGI(TAG, "mqtt connected");
            mqttClient.subscribe(mqtt_topic_config);  // Subscribe to the config topic
        } else {
            printf("mqtt failed, rc= %d\n", mqttClient.state());
            printf(" try again in 5 seconds\n");
            delay(5000);
        }
    }
}
void start_ap() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
}

void start_sta() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    printf("Connecting to WiFi\n");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        printf(".");
    }
    ESP_LOGI(TAG, "Connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());
}
//HieuNM End
Ticker logTicker;

void sendSystemInfo() {
    DynamicJsonDocument doc(1024);

    doc["chipModel"] = ESP.getChipModel();
    doc["chipCores"] = ESP.getChipCores();
    doc["chipFrequency"] = ESP.getCpuFreqMHz();
    doc["chipVersion"] = ESP.getChipRevision();
    doc["ramSizeKB"] = ESP.getHeapSize() / 1024.0;
    doc["psramSizeMB"] = ESP.getPsramSize() / (1024.0 * 1024.0);
    doc["flashSizeMB"] = ESP.getFlashChipSize() / (1024.0 * 1024.0);
    doc["flashSpeedMHz"] = ESP.getFlashChipSpeed() / 1000000.0;
    doc["sdkVersion"] = ESP.getSdkVersion();
    doc["firmwareSizeMB"] = ESP.getSketchSize() / (1024.0 * 1024.0);
    doc["storageSpaceMB"] = SPIFFS.totalBytes() / (1024.0 * 1024.0);
    doc["macAddress"] = WiFi.macAddress();

    // Chuyển đổi JSON thành chuỗi
    String jsonString;
    serializeJson(doc, jsonString);

    // Log ra màn hình
    Serial.println(jsonString);

    // Gửi log qua MQTT
    if (mqttClient.connected()) {
        // mqttClient.publish(mqtt_topic_log, jsonString.c_str());
		logMessage(jsonString.c_str());
    } else {
        Serial.println("MQTT not connected, unable to send log");
    }
}

void setup()
{
	// Serial.begin(115200);
	esp_log_level_set("*", ESP_LOG_VERBOSE);
	ESP_LOGI(TAG, "Starting...");

	loadQRListFromFlash();
	start_sta();
	mqtt_start();
#ifdef MUSIC_PLAYER
	audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
#endif
	tft.init();
	tft.initDMA();
	tft.startWrite();

#ifdef PLUS

	pinMode(SD_CS, OUTPUT);
	digitalWrite(SD_CS, HIGH);
	SPI.begin(SDMMC_CLK, SDMMC_D0, SDMMC_CMD);
	SD.begin(SD_CS);

#endif

	NVS.begin();

	getNVSData();

	if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
	{
		ESP_LOGI(TAG, "SPIFFS Mount Failed");
		tft.setBrightness(brightness);

		delay(2000);
		ESP.restart();
	}

	wakeup_reason();

	lv_init();

  	if (!disp_draw_buf)
	{
		ESP_LOGI(TAG, "LVGL disp_draw_buf allocate failed!");
	}
	else
	{

		ESP_LOGI(TAG, "Display buffer size: ");

		lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, disp_draw_buf2, screenWidth * SCR);

		/* Initialize the display */
		lv_disp_drv_init(&disp_drv);
		/* Change the following line to your display resolution */
		disp_drv.hor_res = screenWidth;
		disp_drv.ver_res = screenHeight;
		disp_drv.flush_cb = my_disp_flush;
		disp_drv.draw_buf = &draw_buf;
		lv_disp_drv_register(&disp_drv);

		/* Initialize the input device driver */
		static lv_indev_drv_t indev_drv;
		lv_indev_drv_init(&indev_drv);
		indev_drv.type = LV_INDEV_TYPE_POINTER;
		indev_drv.read_cb = my_touchpad_read;
		lv_indev_drv_register(&indev_drv);
	}
	tft.setBrightness(brightness);

#ifdef MUSIC_PLAYER
	audio.setVolume(21); // 0...21
	audio.forceMono(true);
	audio.setBalance(16);
#endif
	ui_start_screen_init();
	lcd_1602_init();
    if(activeQRId != -1) {
        for(const auto& qr : qrList) {
            if(qr.id == activeQRId) {
                logMessage("Active QR: %d", qr.id);
                lv_print_qrcode(qr.qrText.c_str(),qr.bank.c_str(), qr.name.c_str(), qr.account.c_str());
                break;
            }
        }
    }
runMusic();

    // Thiết lập Ticker để log thông tin mỗi phút
    logTicker.attach(60, sendSystemInfo);  // 60 giây
}

void loop()
{
#ifndef LVGL_LOOP
	lv_timer_handler(); /* let the GUI do its work */
	delay(5);
#endif

    if (!mqttClient.connected()) {
        reconnect_mqtt();
    }
    mqttClient.loop();

	if(g_is_reverting) {
		lcd.clear();
		if(activeQRId != -1) {
        for(const auto& qr : qrList) {
            if(qr.id == activeQRId) {
                logMessage("Active QR: %d", qr.id);
                lv_print_qrcode(qr.qrText.c_str(),qr.bank.c_str(), qr.name.c_str(), qr.account.c_str());
				break;
				}
			}
		}
		lcd.clear();
		g_is_reverting = false;
	}
}
