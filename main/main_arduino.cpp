#include "BattUtils.h"
#include "DomoticzUI.h"
#include "ImageGetter.h"
#include "Logger.h"
#include "core/lv_obj.h"
#include "ui/ui.h"
#include "widgets/lv_label.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WebApp.h>
#include "Thermostat.h"
#include "DeviceFactory.h"
using namespace domoticz::devices;

#define LED_PIN    -1
#define BUTTON_PIN -1 // no button

WebAppHandler &app = WebAppHandler::getInstance(80);
EEPROMCfg &cfg     = EEPROMCfg::getInstance();

StaticJsonDocument<10000> json;
StaticJsonDocument<10000> json_log;

String strJson;
String strJsonLog;

#define DOMOTICZ_ID_SUFRAGERIE            85
#define DOMOTICZ_ID_AFARA                 52
#define DOMOTICZ_ID_CHEMNEY               29
#define DOMOTICZ_ID_BOILER                271
#define AUTO_BOILER                       146
#define AUTO_HEAT                         130
#define GENERAL_WEATHER                   259
#define DOMOTICZ_ID_POWER_USAGE           5
#define DOMOTICZ_ID_FL_SUFRAGERIE         182
#define DOMOTICZ_ID_FL_SUFRAGERIE_SP      178
#define DOMOTICZ_ID_FL_DORM1              206
#define DOMOTICZ_ID_FL_DORM1_SP           202
#define DOMOTICZ_ID_FL_DORM2              166
#define DOMOTICZ_ID_FL_DORM2_SP           162
#define DOMOTICZ_ID_FL_BAIE               194
#define DOMOTICZ_ID_FL_BAIE_SP            190
#define DOMOTICZ_LYRIC_TEMP               133
#define DOMOTICZ_LYRIC_TEMP_SP            136
#define DOMOTICZ_LYRIC_STATE              137
#define DOMOTICZ_ID_F_CRT_TEMP            274
#define DOMOTICZ_ID_PREDICT_TEMP_DAY0     282
#define DOMOTICZ_ID_PREDICT_TEMP_DAY0_MIN 283
#define DOMOTICZ_ID_PREDICT_TEMP_DAY1     289
#define DOMOTICZ_ID_PREDICT_TEMP_DAY1_MIN 290
#define DOMOTICZ_ID_PREDICT_TEMP_DAY2     296
#define DOMOTICZ_ID_PREDICT_TEMP_DAY2_MIN 297

#define DOMOTICZ_ID_PREDICT_TEMP_DAY3     303
#define DOMOTICZ_ID_PREDICT_TEMP_DAY3_MIN 304

#define DOMOTICZ_ID_PREDICT_TEMP_DAY4     310
#define DOMOTICZ_ID_PREDICT_TEMP_DAY4_MIN 311

#define DOMOTICZ_ID_SLEEP_DORM1 209
#define DOMOTICZ_ID_SLEEP_DORM2 210
#define DOMOTICZ_ID_SLEEP_SUFR  211
#define DOMOTICZ_ID_SLEEP_BAIE  212

#define DOMOTICZ_ID_DAY_DORM1 214
#define DOMOTICZ_ID_DAY_DORM2 215
#define DOMOTICZ_ID_DAY_SUFR  216
#define DOMOTICZ_ID_DAY_BAIE  213

#define DOMOTICZ_ID_COMF_DORM1 219
#define DOMOTICZ_ID_COMF_DORM2 220
#define DOMOTICZ_ID_COMF_SUFR  217
#define DOMOTICZ_ID_COMF_BAIE  218

#define DOMOTICZ_ID_AWAY_DORM1 224
#define DOMOTICZ_ID_AWAY_DORM2 221
#define DOMOTICZ_ID_AWAY_SUFR  222
#define DOMOTICZ_ID_AWAY_BAIE  223

DomoticzUI dui;
ImageGetter img_getter;

void onThermostatValueChanged(lv_event_t *e)
{
    // Your code here
    lv_obj_t *target = lv_event_get_target(e);
    int16_t value    = lv_arc_get_value(target);
    dui.setRangeValuebyObj(target, (float)value / 2, false);
}

void onThermostatValueDone(lv_event_t *e)
{
    // Your code here
    lv_obj_t *target = lv_event_get_target(e);
    int16_t value    = lv_arc_get_value(target);
    // dui.setRangeValue(DOMOTICZ_ID_FL_SUFRAGERIE_SP, value);
    dui.setRangeValuebyObj(target, (float)value / 2, true);
}

// #define DOMOTICZ_ID_SLEEP_DORM1 209
// #define DOMOTICZ_ID_SLEEP_DORM2 210
// #define DOMOTICZ_ID_SLEEP_SUFR  211
// #define DOMOTICZ_ID_SLEEP_BAIE  212

// #define DOMOTICZ_ID_DAY_DORM1 214
// #define DOMOTICZ_ID_DAY_DORM2 215
// #define DOMOTICZ_ID_DAY_SUFR  216
// #define DOMOTICZ_ID_DAY_BAIE  213

// #define DOMOTICZ_ID_COMF_DORM1 219
// #define DOMOTICZ_ID_COMF_DORM2 220
// #define DOMOTICZ_ID_COMF_SUFR  217
// #define DOMOTICZ_ID_COMF_BAIE  218

// #define DOMOTICZ_ID_AWAY_DORM1 224
// #define DOMOTICZ_ID_AWAY_DORM2 221
// #define DOMOTICZ_ID_AWAY_SUFR  222
// #define DOMOTICZ_ID_AWAY_BAIE  223

const int thermo_settings_table[][4] = {
    {DOMOTICZ_ID_FL_SUFRAGERIE_SP, DOMOTICZ_ID_FL_DORM1_SP, DOMOTICZ_ID_FL_DORM2_SP, DOMOTICZ_ID_FL_BAIE_SP},
    {DOMOTICZ_ID_AWAY_SUFR,        DOMOTICZ_ID_AWAY_DORM1,  DOMOTICZ_ID_AWAY_DORM2,  DOMOTICZ_ID_AWAY_BAIE },
    {DOMOTICZ_ID_SLEEP_SUFR,       DOMOTICZ_ID_SLEEP_DORM1, DOMOTICZ_ID_SLEEP_DORM2, DOMOTICZ_ID_SLEEP_BAIE},
    {DOMOTICZ_ID_DAY_SUFR,         DOMOTICZ_ID_DAY_DORM1,   DOMOTICZ_ID_DAY_DORM2,   DOMOTICZ_ID_DAY_BAIE  },
    {DOMOTICZ_ID_COMF_SUFR,        DOMOTICZ_ID_COMF_DORM1,  DOMOTICZ_ID_COMF_DORM2,  DOMOTICZ_ID_COMF_BAIE },
};

#define MODE_NORMAL   0
#define MODE_SET_AWAY 1

void setThermostats(int mode, bool update_now = false);

void onConfigChanged(lv_event_t *e)
{
    // Your code here
    lv_obj_t *target = lv_event_get_target(e);
    int value           = lv_dropdown_get_selected(target);
    Logger::Debug("Config dropdown selected: " + String(value));
    setThermostats(value, true);
}

void onConfigChanged2(lv_event_t *e)
{
    // Your code here
    lv_obj_t *target = lv_event_get_target(e);
    int value        = lv_dropdown_get_selected(target);
    Logger::Debug("Config dropdown2 selected: " + String(value));
    WADomoticzSetSelector(208, value * 10);

    // setThermostats(ID, true);
}

/* ToDo: Make it generic*/
void setThermostats(int mode, bool update_now)
{
    static int old_mode = 0;
    static lv_color_t color[4];

    if (mode >= sizeof(thermo_settings_table) / sizeof(thermo_settings_table[0])) {
        Logger::Error("Wrong mode ID");
        return;
    }
    if (mode != old_mode) {
        // remove old
        dui.removeSensor(thermo_settings_table[old_mode][0]);
        dui.removeSensor(thermo_settings_table[old_mode][1]);
        dui.removeSensor(thermo_settings_table[old_mode][2]);
        dui.removeSensor(thermo_settings_table[old_mode][3]);
        if (old_mode == 0) {
            color[0] = lv_obj_get_style_text_color(ui_TSetSufr, LV_PART_MAIN | LV_STATE_DEFAULT);
            color[1] = lv_obj_get_style_text_color(ui_TSetDorm1, LV_PART_MAIN | LV_STATE_DEFAULT);
            color[2] = lv_obj_get_style_text_color(ui_TSetDorm2, LV_PART_MAIN | LV_STATE_DEFAULT);
            color[3] = lv_obj_get_style_text_color(ui_TSetBaie, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        if (mode != 0) {
            lv_obj_set_style_text_color(ui_TSetSufr, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(ui_TSetDorm1, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(ui_TSetDorm2, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(ui_TSetBaie, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        else {
            lv_obj_set_style_text_color(ui_TSetSufr, color[0], LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(ui_TSetDorm1, color[1], LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(ui_TSetDorm2, color[2], LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(ui_TSetBaie, color[3], LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    dui.addDevice(thermo_settings_table[mode][0], TYPE_THERMOSTAT, "", {ui_TSetSufr, ui_TSufr});
    dui.addDevice(thermo_settings_table[mode][1], TYPE_THERMOSTAT, "", {ui_TSetDorm1, ui_TDorm1});
    dui.addDevice(thermo_settings_table[mode][2], TYPE_THERMOSTAT, "", {ui_TSetDorm2, ui_TDorm2});
    dui.addDevice(thermo_settings_table[mode][3], TYPE_THERMOSTAT, "", {ui_TSetBaie, ui_TBaie});

    if (update_now) {
        dui.updateSensor(thermo_settings_table[mode][0]);
        dui.updateSensor(thermo_settings_table[mode][1]);
        dui.updateSensor(thermo_settings_table[mode][2]);
        dui.updateSensor(thermo_settings_table[mode][3]);
    }
    old_mode = mode;
}
void iniSensorsConfigs(void)
{

}
void registerDomoticzSensors(void)
{
    // neet do bee initialized here!
    weather_config floresti_weather = {
    .crt_t = {DOMOTICZ_ID_F_CRT_TEMP, ui_FTemperatureCrt},
    .day0 = {
        .w  = {DOMOTICZ_ID_PREDICT_TEMP_DAY0, ui_FTemperatureDay0, ui_ImgWeatherDay0},
        .t = {DOMOTICZ_ID_PREDICT_TEMP_DAY0_MIN, ui_FTemperatureDay0min}
    },
    .day1 = {
        .w  = {DOMOTICZ_ID_PREDICT_TEMP_DAY1, ui_FTemperatureDay1, ui_ImgWeatherDay1},
        .t = {DOMOTICZ_ID_PREDICT_TEMP_DAY1_MIN, ui_FTemperatureDay1min}
    },
    .day2 = {
        .w  = {DOMOTICZ_ID_PREDICT_TEMP_DAY2, ui_FTemperatureDay2, ui_ImgWeatherDay2},
        .t = {DOMOTICZ_ID_PREDICT_TEMP_DAY2_MIN, ui_FTemperatureDay2min}
    },
    .day3 = {
        .w  = {DOMOTICZ_ID_PREDICT_TEMP_DAY3, ui_FTemperatureDay3, ui_ImgWeatherDay3},
        .t = {DOMOTICZ_ID_PREDICT_TEMP_DAY3_MIN, ui_FTemperatureDay3min}
    },
    .day4 = {
        .w  = {DOMOTICZ_ID_PREDICT_TEMP_DAY4, ui_FTemperatureDay4, ui_ImgWeatherDay4},
        .t = {DOMOTICZ_ID_PREDICT_TEMP_DAY4_MIN, ui_FTemperatureDay4min}
    },
    .day_arr = {ui_LabelFDay0, ui_LabelFDay1, ui_LabelFDay2, ui_LabelFDay3, ui_LabelFDay4 }
};
  weather_config risca_weather = {
    .crt_t = {260, ui_RTemperatureCrt},
    .day0 = {
        .w  = {338, ui_RTemperatureDay0, ui_RImgWeatherDay0},
        .t = {339, ui_RTemperatureDay0min}
    },
    .day1 = {
        .w  = {345, ui_RTemperatureDay1, ui_RImgWeatherDay1},
        .t = {346, ui_RTemperatureDay1min}
    },
    .day2 = {
        .w  = {352, ui_RTemperatureDay2, ui_RImgWeatherDay2},
        .t = {353, ui_RTemperatureDay2min}
    },
    .day3 = {
        .w  = {359, ui_RTemperatureDay3, ui_RImgWeatherDay3},
        .t = {360, ui_RTemperatureDay3min}
    },
    .day4 = {
        .w  = {366, ui_RTemperatureDay4, ui_RImgWeatherDay4},
        .t = {367, ui_RTemperatureDay4min}
    },
    .day_arr = {ui_LabelRDay0, ui_LabelRDay1, ui_LabelRDay2, ui_LabelRDay3, ui_LabelRDay4 }

    };
    img_getter.addImage(0, &ui_img_day_clear_png);
    img_getter.addImage(1, &ui_img_day_clear_png);
    img_getter.addImage(2, &ui_img_day_clear_png);
    img_getter.addImage(3, &ui_img_day_clear_png);
    img_getter.addImage(4, &ui_img_overcast_png);
    img_getter.addImage(5, &ui_img_day_partial_cloud_png);
    // Needs to be created before all objects.
    dui.setImageGetter(&img_getter);

    dui.addDevice(DOMOTICZ_ID_FL_SUFRAGERIE, TYPE_TEMPERATURE, "", {ui_TCrtSufr});
    dui.addDevice(DOMOTICZ_ID_FL_DORM1,      TYPE_TEMPERATURE, "", {ui_TCrtDorm1});
    dui.addDevice(DOMOTICZ_ID_FL_DORM2,      TYPE_TEMPERATURE, "", {ui_TCrtDorm2});
    dui.addDevice(DOMOTICZ_ID_FL_BAIE,       TYPE_TEMPERATURE, "", {ui_TCrtBaie});
    dui.addDevice(DOMOTICZ_LYRIC_TEMP,       TYPE_TEMPERATURE, "", {ui_TCrtThermostat});
    dui.addDevice(DOMOTICZ_LYRIC_TEMP_SP,    TYPE_THERMOSTAT, "", {ui_TSetThermostat, ui_TThermostat});

    dui.addDevice(183,                       TYPE_SELECTOR, "", {ui_TCrtSufr}, 1);
    dui.addDevice(207,                       TYPE_SELECTOR, "", {ui_TCrtDorm1}, 1);
    dui.addDevice(167,                       TYPE_SELECTOR, "", {ui_TCrtDorm2}, 1);
    dui.addDevice(195,                       TYPE_SELECTOR, "", {ui_TCrtBaie}, 1);

    dui.addDevice(DOMOTICZ_ID_FL_SUFRAGERIE, TYPE_TEMPERATURE, "", {ui_TCrtSufr});

    dui.addWeather(&floresti_weather);
    dui.addWeather(&risca_weather);
 
    dui.addDevice(208, TYPE_SELECTOR, "", {ui_Dropdown2});

    setThermostats(0);
}

void onMqttMesage(void *data, void *topic, void *payload) { dui.onMqttMesage(data, topic, payload); }

String &onGetLog(void)
{
    json_log.clear();
    strJsonLog = "";
    String str;
    int i = 0;
    while (Logger::log_queue.pop(str)) {
        json_log["log"][i++] = str;
    }
    serializeJson(json_log, strJsonLog);
    return strJsonLog;
}

/**
 * @brief Function called when /settings?name=value is triggered by the javascript
 *        APIs. This must have a javascript implementation pair.
 *
 * @param name The parameter name
 * @param value The value of that parameter
 * @return int 0 if correct, different if errors
 */
int onSettings(const String &name, const String &value)
{
    // return sensor.onSettings(name, value);
    return 0;
}

/**
 * @brief  Get the JSON string to be sent back to the web page requester.
 *
 * @return String&
 */

void onWiFiConnect(void *data)
{
    Logger::Debug("On Wifi Connected!");
    registerDomoticzSensors();
    dui.updateAll();
}

/**
 * @brief Iniialize everything
 *
 */
static void app_init(void)
{
    Logger::init(Logger::LOG_INFO, 115200);
    cfg.init();
    app.init(LED_PIN, BUTTON_PIN, "192.168.3.1", MODE_CLIENT);
    //   WASubscribeMQTT("domoticz/in");
    WASubscribeMQTT("domoticz/out");
    WARegisterEventCallback(onWiFiConnect, NULL, MSG_WIFI_CONNECTED);
}

/**
 * @brief Start each object
 *
 */
static void begin()
{
    cfg.begin();
    app.begin();
}

/**
 * @brief Set up components
 *
 */
void app_setup()
{

    app_init();
    begin();

    WASetRequestHook("/settings", onSettings);
    WASetRequestHook("/log", onGetLog);

    WiFi.setSleep(true);
    WARegisterMQTTMessageCallback(onMqttMesage, 0);
}

/**
 * @brief Main loop
 *
 */
void app_loop()
{
    while (1) {
        if (app.loop() == 0) {
            yield();
            delay(1);
        }
    }
}
