// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: Lumia-ESP32

#ifndef _LUMIA_ESP32_UI_H
#define _LUMIA_ESP32_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

  void showNotification_Animation(lv_obj_t *TargetObject, int delay);

  extern lv_obj_t *ui_bootScreen;
  extern lv_obj_t *ui_bootLogo;
  extern lv_obj_t *ui_lockScreen;
  extern lv_obj_t *ui_lockScreenPanel;
  extern lv_obj_t *ui_lockScreenTime;
  extern lv_obj_t *ui_lockScreenDate;
  extern lv_obj_t *ui_lockPanel;
  extern lv_obj_t *ui_lockScreenCode;
  extern lv_obj_t *ui_statusBar;
  extern lv_obj_t *ui_batteryBar;
  extern lv_obj_t *ui_wifiIcon;
  extern lv_obj_t *ui_networkIcon;
  extern lv_obj_t *ui_lockTime;
  extern lv_obj_t *ui_startScreen;
  extern lv_obj_t *ui_Panel2;
  extern lv_obj_t *ui_startButton;
  extern lv_obj_t *ui_backButton;
  extern lv_obj_t *ui_searchButton;
  extern lv_obj_t *ui_startPanel;
  extern lv_obj_t *ui_systemKeyboard;
  extern lv_obj_t *ui_notificationPanel;
  extern lv_obj_t *ui_actionPanel;
  extern lv_obj_t *ui_actionDate;
  extern lv_obj_t *ui_actionBattery;
  extern lv_obj_t *ui_notificationText;
  extern lv_obj_t *ui_dragPanel;
  extern lv_obj_t *ui_Label7;
  extern lv_obj_t *ui_batteryIcon;
  extern lv_obj_t *ui_settingsScreen;
  extern lv_obj_t *ui_settingsScroll;
  extern lv_obj_t *ui_displayLabel;
  extern lv_obj_t *ui_brightnessLabel;
  extern lv_obj_t *ui_brightnessSlider;
  extern lv_obj_t *ui_timeoutLabel;
  extern lv_obj_t *ui_timeoutSelect;
  extern lv_obj_t *ui_storageLabel;
  extern lv_obj_t *ui_screenLock;
  extern lv_obj_t *ui_lockSwitch;
  extern lv_obj_t *ui_lockPass;
  extern lv_obj_t *ui_settingsKeypad;
  extern lv_obj_t *ui_deviceLabel;
  extern lv_obj_t *ui_storageBar;
  extern lv_obj_t *ui_storageText;
  extern lv_obj_t *ui_themeLabel;
  extern lv_obj_t *ui_themeWheel;
  extern lv_obj_t *ui_settingPanel;
  extern lv_obj_t *ui_settingsIcon;
  extern lv_obj_t *ui_settingsLabel;
  extern lv_obj_t *ui_aboutLabel;
  extern lv_obj_t *ui_aboutText;
  extern lv_obj_t *ui_alertPanel;
  extern lv_obj_t *ui_alertPanelDialog;
  extern lv_obj_t *ui_alertIcon;
  extern lv_obj_t *ui_alertTitle;
  extern lv_obj_t *ui_alertScroll;
  extern lv_obj_t *ui_alertMessage;
  extern lv_obj_t *ui_alertButton;
  extern lv_obj_t *ui_alertButtonText;
  extern lv_obj_t *ui_appWifi;
  extern lv_obj_t *ui_appWifiPanel;
  extern lv_obj_t *ui_appWifiStatus;
  extern lv_obj_t *ui_appWifiSwitch;
  extern lv_obj_t *ui_appWifiNo;
  extern lv_obj_t *ui_appWifiList;
  extern lv_obj_t *ui_appWifiName;
  extern lv_obj_t *ui_appWifiPass;
  extern lv_obj_t *ui_appWifiSelect;
  extern lv_obj_t *ui_appWifiButton;
  extern lv_obj_t *ui_appWifiButtonLabel;
  extern lv_obj_t *ui_appWifiTitle;
  extern lv_obj_t *ui_appWifiIcon;
  extern lv_obj_t *ui_appWifiLabel;
  extern lv_obj_t *ui_appScreen;
  extern lv_obj_t *ui_appPanel;
  extern lv_obj_t *ui_appCalendar;
  extern lv_obj_t *ui_appCalendarObj;
  extern lv_obj_t *ui_appHeader;
  extern lv_obj_t *ui_appIcon;
  extern lv_obj_t *ui_appLabel;
  extern lv_obj_t *ui_keyPad;
  extern lv_obj_t *ui_phoneNumber;
  extern lv_obj_t *ui_phoneNumberDelete;
  extern lv_obj_t *ui_messageSendText;
  extern lv_obj_t *ui_messageSendButton;

  extern lv_obj_t *ui_notificationAlert;
  extern lv_obj_t *ui_notificationAppIcon;
  extern lv_obj_t *ui_notificationAppTitle;
  extern lv_obj_t *ui_notificationContent;
  extern lv_obj_t *ui_weatherScreen;
  extern lv_obj_t *ui_weatherCity;
  extern lv_obj_t *ui_weatherTemperature;
  extern lv_obj_t *ui_weatherDescription;
  extern lv_obj_t *ui_weatherWindspeed;
  extern lv_obj_t *ui_callerScreen;
  extern lv_obj_t *ui_callerDetails;
  extern lv_obj_t *ui_callDetails;
  extern lv_obj_t *ui_callAnswerButton;
  extern lv_obj_t *ui_callAnswerLabel;
  extern lv_obj_t *ui_callMessageButton;
  extern lv_obj_t *ui_callMessageLabel;
  extern lv_obj_t *ui_callDeclineButton;
  extern lv_obj_t *ui_callDeclineText;

#ifdef MUSIC_PLAYER
  extern lv_obj_t *ui_musicPanel;
  extern lv_obj_t *ui_musicArt;
  extern lv_obj_t *ui_musicTrack;
  extern lv_obj_t *ui_musicArtist;
  extern lv_obj_t *ui_musicAlbum;
  extern lv_obj_t *ui_musicNextBtn;
  extern lv_obj_t *ui_musicVolumeIcon;
  extern lv_obj_t *ui_musicPreviousBtn;
  extern lv_obj_t *ui_musicPlayBtn;
  extern lv_obj_t *ui_musicLikeBtn;
  extern lv_obj_t *ui_musicShuffleBtn;
  extern lv_obj_t *ui_musicProgress;
  extern lv_obj_t *ui_musicPlayTime;
  extern lv_obj_t *ui_musicTotalTime;
  extern lv_obj_t *ui_musicVolume;
  extern lv_obj_t *ui_musicArtPanel;
  extern lv_obj_t *ui_musicAlbumArt;
  extern lv_obj_t *ui_musicPanelMini;
  extern lv_obj_t *ui_musicMiniText;
  extern lv_obj_t *ui_musicMiniPlay;
  extern lv_obj_t *ui_musicMiniNext;
#endif
#ifdef PLUS
  extern lv_obj_t *ui_blogPanel;
  extern lv_obj_t *ui_blogSpinner;
  extern lv_obj_t *blogList;
#endif
  extern lv_obj_t *musicList;

  LV_IMG_DECLARE(techcombank);
  LV_IMG_DECLARE(tpbank);
  LV_IMG_DECLARE(mbbank);
  LV_IMG_DECLARE(viettinbank);
  
  LV_IMG_DECLARE(ui_img_windows_logo_png); // assets\windows_logo.png
  LV_IMG_DECLARE(ui_img_wallpaper_png);    // assets\wallpaper.png
  LV_IMG_DECLARE(ui_img_wifi_png);         // assets\wifi.png
  LV_IMG_DECLARE(ui_img_2009460128);       // assets\no-signal.png
  LV_IMG_DECLARE(ui_img_716248923);        // assets\windows-small.png
  LV_IMG_DECLARE(ui_img_back_png);         // assets\back.png
  LV_IMG_DECLARE(ui_img_1954556228);       // assets\search-icon.png
  LV_IMG_DECLARE(ui_img_1947241130);       // assets\battery-level (1).png
  LV_IMG_DECLARE(ui_img_1276322231);       // assets\phone-call.png
  LV_IMG_DECLARE(ui_img_237043237);        // assets\message (1).png
  LV_IMG_DECLARE(ui_img_calendar_png);     // assets\calendar.png
  LV_IMG_DECLARE(ui_img_cloud_png);        // assets\cloud.png
  LV_IMG_DECLARE(ui_img_1127648905);       // assets\cloudy-day.png
  LV_IMG_DECLARE(ui_img_email_png);        // assets\email.png
  LV_IMG_DECLARE(ui_img_gallery_png);      // assets\gallery.png
  LV_IMG_DECLARE(ui_img_gear_png);         // assets\gear.png
  LV_IMG_DECLARE(ui_img_846015263);        // assets\map (4).png
  LV_IMG_DECLARE(ui_img_microsoft_png);    // assets\microsoft.png
  LV_IMG_DECLARE(ui_img_359952343);        // assets\music (1).png
  LV_IMG_DECLARE(ui_img_people_png);       // assets\people.png
  LV_IMG_DECLARE(ui_img_571330079);        // assets\shopping-bag.png
  LV_IMG_DECLARE(ui_img_wallet_png);       // assets\wallet.png

  LV_IMG_DECLARE(ui_img_pause_32_png);    // assets\pause_32.png
  LV_IMG_DECLARE(ui_img_next_png);        // assets\next.png
  LV_IMG_DECLARE(ui_img_playlist_png);    // assets\playlist.png
  LV_IMG_DECLARE(ui_img_volume_png);      // assets\volume.png
  LV_IMG_DECLARE(ui_img_previous_png);    // assets\previous.png
  LV_IMG_DECLARE(ui_img_play_png);        // assets\play.png
  LV_IMG_DECLARE(ui_img_like_off_png);    // assets\like_off.png
  LV_IMG_DECLARE(ui_img_like_on_png);     // assets\like_on.png
  LV_IMG_DECLARE(ui_img_shuffle_off_png); // assets\shuffle_off.png
  LV_IMG_DECLARE(ui_img_shuffle_on_png);  // assets\shuffle_on.png
  LV_IMG_DECLARE(ui_img_album_play_png);  // assets\album_play.png
  LV_IMG_DECLARE(ui_img_folder_png);      // assets\folder.png

  LV_IMG_DECLARE(ui_img_files_png); // assets\files.png
  LV_IMG_DECLARE(ui_img_blog_png);  // assets\blog.png

  // user settings
  extern int brightness;
  extern uint32_t themeColor;
  extern long timeouts[4];
  extern long screenTime;

  void ui_init(void);

  void vibrate(long time);
  lv_obj_t *app_canvas();
  uint32_t uuid(uint16_t parent, uint16_t id);

  void openStart();
  void openSettings();
  void openLock();
  void openAppWifi();
  void openAppCalendar();
  void openAppStore();
  void openAppAbout();
  void openPhone();
  void openMessage();
  void openChat();
  void openMusic();
  void openFiles();
#ifdef PLUS
  void openBlog();
#endif
#ifdef MUSIC_PLAYER
  void runMusic();
  void playPause();
  void changeMusic(bool forward);
#endif

  void showAlert(const void *src, bool state, int size, uint16_t code);

  void saveWifiList();
  void saveSettings();
  void setWifi();

  void loadTestApp();
  void launchApp(const char *name, const void *src, bool header);

  void showCaller(const char *name, const char *call, bool incoming);
  void showNotification(const char *app, const void *src, const char *alert);

  void startVibrate();
  void endVibrate();

  void closeApp();
  extern void light_sleep();
  extern void deep_sleep();

  extern void getMusicList();

  extern void callFunction(uint8_t type);

  void playFile(const char *track);
  void setVolume(int volume);

  lv_obj_t *create_label(lv_obj_t *parent, const char *text, uint16_t yPos);
  lv_obj_t *create_button(lv_obj_t *parent, uint32_t id, const char *text, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
  lv_obj_t *create_slider(lv_obj_t *parent, uint32_t id, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
  lv_obj_t *create_switch(lv_obj_t *parent, uint32_t id, uint16_t xPos, uint16_t yPos);

  lv_obj_t *add_chat_item(lv_obj_t *parent, const char *message, bool dir);
#ifdef PLUS
  void add_file_item(lv_obj_t *parent, char *path, int size, bool file);

  extern void getFileList(lv_obj_t *parent, const char *path);

  void musicPlay_Animation(lv_obj_t *TargetObject, int delay);

  void add_blog_item(lv_obj_t *parent, const char *title, uint16_t id);
  void blog_panel(lv_obj_t *parent);
  lv_obj_t *blogText(lv_obj_t *parent, int y, char *text);
  lv_obj_t *blogTitle(lv_obj_t *parent, char *title);
#endif
  // lv_obj_t *activeApp[10];
  uint16_t appSize;

  extern bool runRequest();

  typedef struct RtcTime
  {
    int year;
    int month;
    int day;
  } Time;

  extern Time rtcTime;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
